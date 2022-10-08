#include "IGenerator.hpp"
#include "ObjectsStore.hpp"
#include "tinyformat.h"
#include <PatternFinder.hpp>

inline uintptr_t GetGObjectsOffset()
{
	return uintptr_t(__int64(GlobalObjects) - __int64(GetModuleHandle(0)));
}

inline uintptr_t GetFNameToStringOffset()
{
	return uintptr_t(__int64(FNameToString) - __int64(GetModuleHandle(0)));
}

class Generator : public IGenerator
{
public:
	bool Initialize(void* module) override
	{
		alignasClasses = {
			{ "ScriptStruct CoreUObject.Plane", 16 },
			{ "ScriptStruct CoreUObject.Quat", 16 },
			{ "ScriptStruct CoreUObject.Transform", 16 },
			{ "ScriptStruct CoreUObject.Vector4", 16 },

			{ "ScriptStruct Engine.RootMotionSourceGroup", 8 }
		};

		virtualFunctionPattern["Class CoreUObject.Object"] = {
			{ "\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5", "xxxxx????xxxx????xxx????xxx", 0x200, R"(	inline void ProcessEvent(class UFunction* function, void* parms)
	{
		return GetVFunction<void(*)(UObject*, class UFunction*, void*)>(this, %d)(this, function, parms);
	})" }
		};
		virtualFunctionPattern["Class CoreUObject.Class"] = {
			{ "\x41\x56\x41\x57\x48\x8D\xA8\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x45\x33\xED", "xxxxxxx????xxx????xxx????xxxxxx????xxx", 0x200, R"(	inline UObject* CreateDefaultObject()
	{
		return GetVFunction<UObject*(*)(UClass*)>(this, %d)(this);
	})" }
		};

		predefinedMembers["Class CoreUObject.Object"] = {
			{ "void**", "VFT" },
			{ "int32_t", "ObjectFlags" },
			{ "int32_t", "InternalIndex" },
			{ "class UClass*", "Class" },
			{ "FName", "Name" },
			{ "class UObject*", "Outer" }
		};
		predefinedStaticMembers["Class CoreUObject.Object"] = {
			{ "FChunkedFixedUObjectArray*", "GObjects" }
		};
		predefinedMembers["Class CoreUObject.Field"] = {
			{ "class UField*", "Next" },
			{ "void*", "Pad01" },
			{ "void*", "Pad02" }
		};
		predefinedMembers["Class CoreUObject.Struct"] = {
			{"UStruct*", "SuperStruct"},
			{"unsigned char", "Padding[0x60]"}
		};
		predefinedMembers["Class CoreUObject.Function"] = {
			{"void*", "Pad"},
			{"uint32_t", "FunctionFlags"},
			{"unsigned char", "Pad01[0x25]"}
		};

		predefinedMethods["ScriptStruct CoreUObject.Vector2D"] = {
			PredefinedMethod::Inline(R"(	inline FVector2D()
		: X(0), Y(0)
	{ })"),
			PredefinedMethod::Inline(R"(	inline FVector2D(float x, float y)
		: X(x),
		  Y(y)
	{ })")
		};
		predefinedMethods["ScriptStruct CoreUObject.LinearColor"] = {
			PredefinedMethod::Inline(R"(	inline FLinearColor()
		: R(0), G(0), B(0), A(0)
	{ })"),
			PredefinedMethod::Inline(R"(	inline FLinearColor(float r, float g, float b, float a)
		: R(r),
		  G(g),
		  B(b),
		  A(a)
	{ })")
		};

		predefinedMethods["Class CoreUObject.Object"] = {
			PredefinedMethod::Inline(R"(	static inline FChunkedFixedUObjectArray& GetGlobalObjects()
	{
		return *GObjects;
	})"),
			PredefinedMethod::Default("std::string GetName() const", R"(std::string UObject::GetName() const
{
	std::string name(Name.ToString());
	if (Name.Number > 0)
	{
		name += '_' + std::to_string(Name.Number);
	}

	auto pos = name.rfind('/');
	if (pos == std::string::npos)
	{
		return name;
	}
	
	return name.substr(pos + 1);
})"),
			PredefinedMethod::Default("std::string GetFullName() const", R"(std::string UObject::GetFullName() const
{
	std::string name;

	if (Class != nullptr)
	{
		std::string temp;
		for (auto p = Outer; p; p = p->Outer)
		{
			temp = p->GetName() + "." + temp;
		}

		name = Class->GetName();
		name += " ";
		name += temp;
		name += GetName();
	}

	return name;
})"),
			PredefinedMethod::Inline(R"(	template<typename T>
	static T* FindObject(const std::string& name)
	{
		for (int i = 0; i < GetGlobalObjects().Num(); ++i)
		{
			auto object = GetGlobalObjects().GetByIndex(i);
	
			if (object == nullptr)
			{
				continue;
			}
	
			if (object->GetFullName().find(name) != std::string::npos)
			{
				return static_cast<T*>(object);
			}
		}
		return nullptr;
	})"),
			PredefinedMethod::Inline(R"(	static UClass* FindClass(const std::string& name)
	{
		for (int i = 0; i < GetGlobalObjects().Num(); ++i)
		{
			auto object = GetGlobalObjects().GetByIndex(i);
	
			if (object == nullptr)
			{
				continue;
			}
	
			if (object->GetFullName() == name)
			{
				return reinterpret_cast<UClass*>(object);
			}
		}
		return nullptr;
	})"),
			PredefinedMethod::Inline(R"(	template<typename T>
	static T* GetObjectCasted(std::size_t index)
	{
		return static_cast<T*>(GetGlobalObjects().GetByIndex(index));
	})"),
			PredefinedMethod::Default("bool IsA(UClass* cmp) const", R"(bool UObject::IsA(UClass* cmp) const
{
	for (auto super = Class; super; super = static_cast<UClass*>(super->SuperField))
	{
		if (super == cmp)
		{
			return true;
		}
	}

	return false;
})")
		};
		predefinedMethods["Class CoreUObject.Class"] = {
			PredefinedMethod::Inline(R"(	template<typename T>
	inline T* CreateDefaultObject()
	{
		return static_cast<T*>(CreateDefaultObject());
	})")
		};

		return true;
	}

	std::string GetGameName() const override
	{
		return "Fortnite";
	}

	std::string GetGameNameShort() const override
	{
		return "FN";
	}

	std::string GetGameVersion() const override
	{
		return "17.50";
	}

	std::string GetNamespaceName() const override
	{
		return "SDK";
	}

	std::vector<std::string> GetIncludes() const override
	{
		return { };
	}

	std::string GetBasicDeclarations() const override
	{
		std::string RetValue = R"(
template<typename Fn>
inline Fn GetVFunction(const void *instance, std::size_t index)
{
	auto vtable = *reinterpret_cast<const void***>(const_cast<void*>(instance));
	return reinterpret_cast<Fn>(vtable[index]);
}

class UObject;

struct FUObjectItem
{
	UObject* Object;
	int32_t Flags;
	int32_t ClusterRootIndex;
	int32_t SerialNumber;
};

class FChunkedFixedUObjectArray
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};

	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32_t MaxElements;
	int32_t NumElements;
	int32_t MaxChunks;
	int32_t NumChunks;

public:
	FORCEINLINE int32_t Num() const
	{
		return NumElements;
	}

	FORCEINLINE int32_t Capacity() const
	{
		return MaxElements;
	}

	FORCEINLINE bool IsValidIndex(int32_t Index) const
	{
		return Index < Num() && Index >= 0;
	}

	FORCEINLINE UObject* GetObjectPtr(int32_t Index) const
	{
		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

		if (!IsValidIndex(Index)) return nullptr;
		if (ChunkIndex > NumChunks) return nullptr;
		if (Index > MaxElements) return nullptr;

		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;

		return (Chunk + WithinChunkIndex)->Object;
	}

	FORCEINLINE UObject* GetObjectPtr(int32_t Index)
	{
		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

		if (!IsValidIndex(Index)) return nullptr;
		if (ChunkIndex > NumChunks) return nullptr;
		if (Index > MaxElements) return nullptr;

		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;

		return (Chunk + WithinChunkIndex)->Object;
	}

	FORCEINLINE UObject* GetByIndex(int32_t Index)
	{
		return GetObjectPtr(Index);
	}

	FORCEINLINE UObject* GetByIndex(int32_t Index) const
	{
		return GetObjectPtr(Index);
	}

	FORCEINLINE UObject* operator[](int32_t Index)
	{
		return GetObjectPtr(Index);
	}

	FORCEINLINE UObject* operator[](int32_t Index) const
	{
		return GetObjectPtr(Index);
	}
};

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

struct FName
{
	uint32_t ComparisonIndex;
	uint32_t Number;

	inline std::string ToString() const
	{
)";

	RetValue += tfm::format(R"(
		static auto FNameToString = reinterpret_cast<void(*)(const FName*, FString&)>(uintptr_t(GetModuleHandle(0)) + 0x%X);
		static auto FreeMemory = reinterpret_cast<void(*)(__int64)>(uintptr_t(GetModuleHandle(0)) + 0x0); //Need to get

		FString OutStr;
		FNameToString(this, OutStr);

		std::string Ret(OutStr.ToString());

		//FreeMemory((__int64)OutStr.c_str());

		return Ret;
	})", GetFNameToStringOffset());

	RetValue += R"(
};

template<class TEnum>
class TEnumAsByte
{
public:
	inline TEnumAsByte()
	{
	}

	inline TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	inline operator TEnum() const
	{
		return (TEnum)value;
	}

	inline TEnum GetValue() const
	{
		return (TEnum)value;
	}

private:
	uint8_t value;
};

struct FText
{
	char UnknownData[0x18];
};

template<typename Key, typename Value>
class TMap
{
	char UnknownData[0x50];
};)";

		return RetValue;
	}

	std::string GetBasicDefinitions() const override
	{
		return tfm::format(R"(auto Base = (uintptr_t)GetModuleHandle(0);
FChunkedFixedUObjectArray* UObject::GObjects = decltype(UObject::GObjects)(Base + 0x%X);)", GetGObjectsOffset());
	}
};

Generator _generator;
IGenerator* generator = &_generator;

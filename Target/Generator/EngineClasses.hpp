#pragma once

#include <set>
#include <string>
#include <windows.h>
#include <locale>

struct FPointer
{
	uintptr_t Dummy;
};

struct FQWord
{
	int A;
	int B;
};

template<class T>
struct TArray
{
	friend struct FString;

public:
	TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	size_t Num() const
	{
		return Count;
	};

	T& operator[](size_t i)
	{
		return Data[i];
	};

	const T& operator[](size_t i) const
	{
		return Data[i];
	};

	bool IsValidIndex(size_t i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

template<typename KeyType, typename ValueType>
class TPair
{
public:
	KeyType   Key;
	ValueType Value;
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

inline void (*FNameToString)(void*, FString&);

struct FName
{
	int32_t ComparisonIndex;
	int32_t Number;

	inline std::string ToString()
	{
		FString OutStr;
		FNameToString(this, OutStr);

		std::string Ret(OutStr.ToString());

		return Ret;
	}
};

template<class InterfaceType>
class TScriptInterface : public FScriptInterface
{
public:
	InterfaceType* operator->() const
	{
		return (InterfaceType*)GetInterface();
	}

	InterfaceType& operator*() const
	{
		return *((InterfaceType*)GetInterface());
	}

	operator bool() const
	{
		return GetInterface() != nullptr;
	}
};

struct FText
{
	char UnknownData[0x18];
};

struct FWeakObjectPtr
{
	int32_t ObjectIndex;
	int32_t ObjectSerialNumber;
};

struct FStringAssetReference
{
	FString AssetLongPathname;
};

template<typename TObjectID>
class TPersistentObjectPtr
{
public:
	FWeakObjectPtr WeakPtr;
	int32_t TagAtLastTest;
	TObjectID ObjectID;
};

class FAssetPtr : public TPersistentObjectPtr<FStringAssetReference>
{

};

struct FGuid
{
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
};

struct FUniqueObjectGuid
{
	FGuid Guid;
};

class FLazyObjectPtr : public TPersistentObjectPtr<FUniqueObjectGuid>
{

};

struct FScriptDelegate
{
	unsigned char UnknownData[16];
};

struct FScriptMulticastDelegate
{
	unsigned char UnknownData[16];
};

class UClass;

class UObject
{
public:
	FPointer VTableObject;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UClass* Class;
	FName Name;
	UObject* Outer;
};

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

	FORCEINLINE UObject* operator[](int32_t Index)
	{
		return GetObjectPtr(Index);
	}

	FORCEINLINE UObject* operator[](int32_t Index) const
	{
		return GetObjectPtr(Index);
	}
};

inline FChunkedFixedUObjectArray* GlobalObjects = nullptr;

class FScriptInterface
{
private:
	UObject* ObjectPointer;
	void* InterfacePointer;

public:
	UObject* GetObject() const
	{
		return ObjectPointer;
	}

	UObject*& GetObjectRef()
	{
		return ObjectPointer;
	}

	void* GetInterface() const
	{
		return ObjectPointer != nullptr ? InterfacePointer : nullptr;
	}
};

class FField;

class FFieldVariant
{
public:
	union FFieldObjectUnion
	{
		FField* Field;
		UObject* Object;
	} Container;

	bool bIsUObject;
};

enum class FFieldClassID : uint64_t
{
	Int8 = 1llu << 1,
	Byte = 1llu << 6,
	Int = 1llu << 7,
	Float = 1llu << 8,
	UInt64 = 1llu << 9,
	Class = 1llu << 10,
	UInt32 = 1llu << 11,
	Interface = 1llu << 12,
	Name = 1llu << 13,
	String = 1llu << 14,
	Object = 1llu << 16,
	Bool = 1llu << 17,
	UInt16 = 1llu << 18,
	Struct = 1llu << 20,
	Array = 1llu << 21,
	Int64 = 1llu << 22,
	Delegate = 1llu << 23,
	SoftObject = 1llu << 27,
	LazyObject = 1llu << 28,
	WeakObject = 1llu << 29,
	Text = 1llu << 30,
	Int16 = 1llu << 31,
	Double = 1llu << 32,
	SoftClass = 1llu << 33,
	Map = 1llu << 46,
	Set = 1llu << 47,
	Enum = 1llu << 48,
	MulticastInlineDelegate = 1llu << 50,
	MulticastSparseDelegate = 1llu << 51,
	ObjectPointer = 1llu << 53
};

enum class EEnumFlags
{
	None,
	Flags = 0x00000001,
	NewerVersionExists = 0x00000002,
};

enum EClassFlags
{
	/** No Flags */
	CLASS_None = 0x00000000u,
	/** Class is abstract and can't be instantiated directly. */
	CLASS_Abstract = 0x00000001u,
	/** Save object configuration only to Default INIs, never to local INIs. Must be combined with CLASS_Config */
	CLASS_DefaultConfig = 0x00000002u,
	/** Load object configuration at construction time. */
	CLASS_Config = 0x00000004u,
	/** This object type can't be saved; null it out at save time. */
	CLASS_Transient = 0x00000008u,
	/** Successfully parsed. */
	CLASS_Parsed = 0x00000010u,
	/** */
	CLASS_MatchedSerializers = 0x00000020u,
	/** Indicates that the config settings for this class will be saved to Project/User*.ini (similar to CLASS_GlobalUserConfig) */
	CLASS_ProjectUserConfig = 0x00000040u,
	/** Class is a native class - native interfaces will have CLASS_Native set, but not RF_MarkAsNative */
	CLASS_Native = 0x00000080u,
	/** Don't export to C++ header. */
	CLASS_NoExport = 0x00000100u,
	/** Do not allow users to create in the editor. */
	CLASS_NotPlaceable = 0x00000200u,
	/** Handle object configuration on a per-object basis, rather than per-class. */
	CLASS_PerObjectConfig = 0x00000400u,

	/** Whether SetUpRuntimeReplicationData still needs to be called for this class */
	CLASS_ReplicationDataIsSetUp = 0x00000800u,

	/** Class can be constructed from editinline New button. */
	CLASS_EditInlineNew = 0x00001000u,
	/** Display properties in the editor without using categories. */
	CLASS_CollapseCategories = 0x00002000u,
	/** Class is an interface **/
	CLASS_Interface = 0x00004000u,
	/**  Do not export a constructor for this class, assuming it is in the cpptext **/
	CLASS_CustomConstructor = 0x00008000u,
	/** all properties and functions in this class are const and should be exported as const */
	CLASS_Const = 0x00010000u,

	/** Class flag indicating the class is having its layout changed, and therefore is not ready for a CDO to be created */
	CLASS_LayoutChanging = 0x00020000u,

	/** Indicates that the class was created from blueprint source material */
	CLASS_CompiledFromBlueprint = 0x00040000u,

	/** Indicates that only the bare minimum bits of this class should be DLL exported/imported */
	CLASS_MinimalAPI = 0x00080000u,

	/** Indicates this class must be DLL exported/imported (along with all of it's members) */
	CLASS_RequiredAPI = 0x00100000u,

	/** Indicates that references to this class default to instanced. Used to be subclasses of UComponent, but now can be any UObject */
	CLASS_DefaultToInstanced = 0x00200000u,

	/** Indicates that the parent token stream has been merged with ours. */
	CLASS_TokenStreamAssembled = 0x00400000u,
	/** Class has component properties. */
	CLASS_HasInstancedReference = 0x00800000u,
	/** Don't show this class in the editor class browser or edit inline new menus. */
	CLASS_Hidden = 0x01000000u,
	/** Don't save objects of this class when serializing */
	CLASS_Deprecated = 0x02000000u,
	/** Class not shown in editor drop down for class selection */
	CLASS_HideDropDown = 0x04000000u,
	/** Class settings are saved to <AppData>/..../Blah.ini (as opposed to CLASS_DefaultConfig) */
	CLASS_GlobalUserConfig = 0x08000000u,
	/** Class was declared directly in C++ and has no boilerplate generated by UnrealHeaderTool */
	CLASS_Intrinsic = 0x10000000u,
	/** Class has already been constructed (maybe in a previous DLL version before hot-reload). */
	CLASS_Constructed = 0x20000000u,
	/** Indicates that object configuration will not check against ini base/defaults when serialized */
	CLASS_ConfigDoNotCheckDefaults = 0x40000000u,
	/** Class has been consigned to oblivion as part of a blueprint recompile, and a newer version currently exists. */
	CLASS_NewerVersionExists = 0x80000000u,
};

enum EClassCastFlags : uint64_t
{
	CASTCLASS_None = 0x0000000000000000,
	CASTCLASS_UField = 0x0000000000000001,
	CASTCLASS_FInt8Property = 0x0000000000000002,
	CASTCLASS_UEnum = 0x0000000000000004,
	CASTCLASS_UStruct = 0x0000000000000008,
	CASTCLASS_UScriptStruct = 0x0000000000000010,
	CASTCLASS_UClass = 0x0000000000000020,
	CASTCLASS_FByteProperty = 0x0000000000000040,
	CASTCLASS_FIntProperty = 0x0000000000000080,
	CASTCLASS_FFloatProperty = 0x0000000000000100,
	CASTCLASS_FUInt64Property = 0x0000000000000200,
	CASTCLASS_FClassProperty = 0x0000000000000400,
	CASTCLASS_FUInt32Property = 0x0000000000000800,
	CASTCLASS_FInterfaceProperty = 0x0000000000001000,
	CASTCLASS_FNameProperty = 0x0000000000002000,
	CASTCLASS_FStrProperty = 0x0000000000004000,
	CASTCLASS_FProperty = 0x0000000000008000,
	CASTCLASS_FObjectProperty = 0x0000000000010000,
	CASTCLASS_FBoolProperty = 0x0000000000020000,
	CASTCLASS_FUInt16Property = 0x0000000000040000,
	CASTCLASS_UFunction = 0x0000000000080000,
	CASTCLASS_FStructProperty = 0x0000000000100000,
	CASTCLASS_FArrayProperty = 0x0000000000200000,
	CASTCLASS_FInt64Property = 0x0000000000400000,
	CASTCLASS_FDelegateProperty = 0x0000000000800000,
	CASTCLASS_FNumericProperty = 0x0000000001000000,
	CASTCLASS_FMulticastDelegateProperty = 0x0000000002000000,
	CASTCLASS_FObjectPropertyBase = 0x0000000004000000,
	CASTCLASS_FWeakObjectProperty = 0x0000000008000000,
	CASTCLASS_FLazyObjectProperty = 0x0000000010000000,
	CASTCLASS_FSoftObjectProperty = 0x0000000020000000,
	CASTCLASS_FTextProperty = 0x0000000040000000,
	CASTCLASS_FInt16Property = 0x0000000080000000,
	CASTCLASS_FDoubleProperty = 0x0000000100000000,
	CASTCLASS_FSoftClassProperty = 0x0000000200000000,
	CASTCLASS_UPackage = 0x0000000400000000,
	CASTCLASS_ULevel = 0x0000000800000000,
	CASTCLASS_AActor = 0x0000001000000000,
	CASTCLASS_APlayerController = 0x0000002000000000,
	CASTCLASS_APawn = 0x0000004000000000,
	CASTCLASS_USceneComponent = 0x0000008000000000,
	CASTCLASS_UPrimitiveComponent = 0x0000010000000000,
	CASTCLASS_USkinnedMeshComponent = 0x0000020000000000,
	CASTCLASS_USkeletalMeshComponent = 0x0000040000000000,
	CASTCLASS_UBlueprint = 0x0000080000000000,
	CASTCLASS_UDelegateFunction = 0x0000100000000000,
	CASTCLASS_UStaticMeshComponent = 0x0000200000000000,
	CASTCLASS_FMapProperty = 0x0000400000000000,
	CASTCLASS_FSetProperty = 0x0000800000000000,
	CASTCLASS_FEnumProperty = 0x0001000000000000,
	CASTCLASS_USparseDelegateFunction = 0x0002000000000000,
	CASTCLASS_FMulticastInlineDelegateProperty = 0x0004000000000000,
	CASTCLASS_FMulticastSparseDelegateProperty = 0x0008000000000000,
	CASTCLASS_FFieldPathProperty = 0x0010000000000000,
};

enum ELifetimeCondition
{
	COND_None = 0,
	COND_InitialOnly = 1,
	COND_OwnerOnly = 2,
	COND_SkipOwner = 3,
	COND_SimulatedOnly = 4,
	COND_AutonomousOnly = 5,
	COND_SimulatedOrPhysics = 6,
	COND_InitialOrOwner = 7,
	COND_Custom = 8,
	COND_ReplayOrOwner = 9,
	COND_ReplayOnly = 10,
	COND_SimulatedOnlyNoReplay = 11,
	COND_SimulatedOrPhysicsNoReplay = 12,
	COND_SkipReplay = 13,
	COND_Never = 15,
	COND_Max = 16,
};

class FFieldClass
{
public:

	FName Name;
	FFieldClassID Id;
	uint64_t CastFlags;
	EClassFlags ClassFlags;
	FFieldClass* SuperClass;
	FField* DefaultObject;
};

class FField
{
public:
	FPointer VTableObject;
	FFieldClass* ClassPrivate;
	FFieldVariant Owner;
	FField* Next;
	FName NamePrivate;
	int FlagsPrivate;
};

enum EPropertyType : uint8_t
{
	ByteProperty,
	BoolProperty,
	IntProperty,
	FloatProperty,
	ObjectProperty,
	NameProperty,
	DelegateProperty,
	DoubleProperty,
	ArrayProperty,
	StructProperty,
	StrProperty,
	TextProperty,
	InterfaceProperty,
	MulticastDelegateProperty,
	WeakObjectProperty,
	LazyObjectProperty,
	AssetObjectProperty,
	SoftObjectProperty,
	UInt64Property,
	UInt32Property,
	UInt16Property,
	Int64Property,
	Int16Property,
	Int8Property,
	MapProperty,
	SetProperty,
	EnumProperty,
	FieldPathProperty,
	EnumAsByteProperty,

	Unknown = 0xFF
};

template <class TEnum>
class TEnumAsByte
{
public:
	TEnumAsByte()
	{
	}

	TEnumAsByte(TEnum _value) : value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(int32_t _value) : value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(uint8_t _value) : value(_value)
	{
	}

	operator TEnum() const
	{
		return static_cast<TEnum>(value);
	}

	TEnum GetValue() const
	{
		return static_cast<TEnum>(value);
	}

private:
	uint8_t value;
};

class FProperty : public FField
{
public:
	int32_t ArrayDim;
	int32_t ElementSize;
	uint64_t PropertyFlags;
	uint16_t RepIndex;
	TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;
	int32_t Offset_Internal;
	FName RepNotifyFunc;
	FProperty* PropertyLinkNext;
	FProperty* NextRef;
	FProperty* DestructorLinkNext;
	FProperty* PostConstructLinkNext;
};

class FBoolProperty : public FProperty
{
public:
	uint8_t FieldSize;
	uint8_t ByteOffset;
	uint8_t ByteMask;
	uint8_t FieldMask;
};

class FObjectPropertyBase : public FProperty
{
public:

	UClass* PropertyClass;
};

enum class EArrayPropertyFlags
{
	None,
	UsesMemoryImageAllocator
};

class FArrayProperty : public FProperty
{
public:
	FProperty* Inner;
	EArrayPropertyFlags ArrayFlags;
};

class FStructProperty : public FProperty
{
public:
	class UStruct* Struct;
};

class UField : public UObject
{
public:
	UField* Next;
	void* padding;
	void* padding2;
};

class UEnum : public UObject
{
public:
	enum class ECppForm
	{
		Regular,
		Namespaced,
		EnumClass
	};

	void* Pad;
	FString CppType;
	TArray<TPair<FName, int64_t>> Names;
	ECppForm CppForm;
	EEnumFlags EnumFlags;

	auto GetCPPString()
	{
		return CppType.ToString();
	}

	auto GetEnumType()
	{
		uint64_t maxValue = 0;

		for (size_t i = 0; i < Names.Num(); i++)
			if (Names[i].Value > maxValue)
				maxValue = Names[i].Value;

		if (maxValue <= 0x100)
		{
			return "uint8_t";
		}
		else if (maxValue <= 0xFFFF)
		{
			return "uint16_t";
		}
		else if (maxValue <= 0xFFFFFFFF)
		{
			return "uint32_t";
		}
		else if (maxValue <= 0xFFFFFFFFFFFFFFFF)
		{
			return "uint64_t";
		}
	}
};

class FEnumProperty : public FProperty
{
public:
	FProperty* UnderlyingProp;
	UEnum* Enum;
};

class FByteProperty : public FProperty
{
public:
	UEnum* Enum;
};

class FInterfaceProperty : public FProperty
{
public:
	UClass* InterfaceClass;
};

class FMapProperty : public FProperty
{
public:
	FProperty* KeyProp;
	FProperty* ValueProp;
};

class FSetProperty : public FProperty
{
public:
	FProperty* ElementProp;
};

class FSoftClassProperty : public FProperty
{
public:
	UClass* MetaClass;
};

class UStruct : public UField
{
public:
	UStruct* SuperStruct;
	UField* Children;
	FField* ChildProperties;
	int32_t PropertiesSize;
	int32_t MinAlignment;
	TArray<uint8_t> Script;
	FProperty* PropertyLink;
	FProperty* RefLink;
	FProperty* DestructorLink;
	FProperty* PostConstructLink;
	TArray<UObject*> ScriptAndPropertyObjectReferences;
	void* UnresolvedScriptProperties;
};

class UFunction : public UStruct
{
public:
	void* Pad;
	uint32_t FunctionFlags;
	uint8_t NumParms;
	uint16_t ParmsSize;
	uint16_t ReturnValueOffset;
	uint16_t RPCId;
	uint16_t RPCResponseId;
	FProperty* FirstPropertyToInit;
	UFunction* EventGraphFunction;
	int32_t EventGraphCallOffset;
	void* Func;
};

class UClass : public UStruct
{
};

class UBlueprintGeneratedClass : UClass
{
public:
};

class UAnimBlueprintGeneratedClass : UClass
{
public:
};

enum EStructFlags
{
	// State flags.
	STRUCT_NoFlags = 0x00000000,
	STRUCT_Native = 0x00000001,

	/** If set, this struct will be compared using native code */
	STRUCT_IdenticalNative = 0x00000002,

	STRUCT_HasInstancedReference = 0x00000004,

	STRUCT_NoExport = 0x00000008,

	/** Indicates that this struct should always be serialized as a single unit */
	STRUCT_Atomic = 0x00000010,

	/** Indicates that this struct uses binary serialization; it is unsafe to add/remove members from this struct without incrementing the package version */
	STRUCT_Immutable = 0x00000020,

	/** If set, native code needs to be run to find referenced objects */
	STRUCT_AddStructReferencedObjects = 0x00000040,

	/** Indicates that this struct should be exportable/importable at the DLL layer.  Base structs must also be exportable for this to work. */
	STRUCT_RequiredAPI = 0x00000200,

	/** If set, this struct will be serialized using the CPP net serializer */
	STRUCT_NetSerializeNative = 0x00000400,

	/** If set, this struct will be serialized using the CPP serializer */
	STRUCT_SerializeNative = 0x00000800,

	/** If set, this struct will be copied using the CPP operator= */
	STRUCT_CopyNative = 0x00001000,

	/** If set, this struct will be copied using memcpy */
	STRUCT_IsPlainOldData = 0x00002000,

	/** If set, this struct has no destructor and non will be called. STRUCT_IsPlainOldData implies STRUCT_NoDestructor */
	STRUCT_NoDestructor = 0x00004000,

	/** If set, this struct will not be constructed because it is assumed that memory is zero before construction. */
	STRUCT_ZeroConstructor = 0x00008000,

	/** If set, native code will be used to export text */
	STRUCT_ExportTextItemNative = 0x00010000,

	/** If set, native code will be used to export text */
	STRUCT_ImportTextItemNative = 0x00020000,

	/** If set, this struct will have PostSerialize called on it after CPP serializer or tagged property serialization is complete */
	STRUCT_PostSerializeNative = 0x00040000,

	/** If set, this struct will have SerializeFromMismatchedTag called on it if a mismatched tag is encountered. */
	STRUCT_SerializeFromMismatchedTag = 0x00080000,

	/** If set, this struct will be serialized using the CPP net delta serializer */
	STRUCT_NetDeltaSerializeNative = 0x00100000,

	/** If set, this struct will be have PostScriptConstruct called on it after a temporary object is constructed in a running blueprint */
	STRUCT_PostScriptConstruct = 0x00200000,

	/** If set, this struct can share net serialization state across connections */
	STRUCT_NetSharedSerialization = 0x00400000,

	/** If set, this struct has been cleaned and sanitized (trashed) and should not be used */
	STRUCT_Trashed = 0x00800000,

	/** If set, this structure has been replaced via reinstancing */
	STRUCT_NewerVersionExists = 0x01000000,

	/** Struct flags that are automatically inherited */
	STRUCT_Inherit = STRUCT_HasInstancedReference | STRUCT_Atomic,

	/** Flags that are always computed, never loaded or done with code generation */
	STRUCT_ComputedFlags = STRUCT_NetDeltaSerializeNative | STRUCT_NetSerializeNative | STRUCT_SerializeNative | STRUCT_PostSerializeNative | STRUCT_CopyNative | STRUCT_IsPlainOldData | STRUCT_NoDestructor | STRUCT_ZeroConstructor | STRUCT_IdenticalNative | STRUCT_AddStructReferencedObjects | STRUCT_ExportTextItemNative | STRUCT_ImportTextItemNative | STRUCT_SerializeFromMismatchedTag | STRUCT_PostScriptConstruct | STRUCT_NetSharedSerialization
};

class UScriptStruct : UStruct
{
public:
	EStructFlags StructFlags;
	bool bPrepareCppStructOpsCompleted;
	void* CppStructOps;
};
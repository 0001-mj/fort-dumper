#pragma once

#include <string>
#include <vector>
#include <array>

#include "../Target/Generator/EngineClasses.hpp"
#include "PropertyFlags.hpp"
#include "FunctionFlags.hpp"
#include "../IGenerator.hpp"

class UObject;
class UEClass;

class UEObject
{
public:
	UEObject()
		: object(nullptr)
	{
	}
	UEObject(UObject *_object)
		: object(_object)
	{
	}

	bool IsValid() const
	{
		return object != nullptr;
	}

	size_t GetIndex() const;

	UEClass GetClass() const;

	UEObject GetOuter() const;

	std::string GetName() const;

	std::string GetFullName() const;

	std::string GetNameCPP() const;

	UEObject GetPackageObject() const;

	bool IsBlueprint() const
	{
		if (!IsValid())
			return true;

		if (object->Name.ToString().find("/Script/") != std::string::npos)
			return false;

		return true;
	}

	void* GetAddress() const;

	template<typename Base>
	Base Cast() const
	{
		return Base(object);
	}

	template<typename T>
	bool IsA() const;

	static UEClass StaticClass();

protected:
	UObject* object;
};

namespace std
{
	template<>
	struct hash<UEObject>
	{
		size_t operator()(const UEObject& obj) const
		{
			return std::hash<void*>()(obj.GetAddress());
		}
	};
}

inline bool operator==(const UEObject& lhs, const UEObject& rhs) { return rhs.GetAddress() == lhs.GetAddress(); }
inline bool operator!=(const UEObject& lhs, const UEObject& rhs) { return !(lhs == rhs); }

class UEField : public UEObject
{
public:
	using UEObject::UEObject;

	UEField GetNext() const;

	static UEClass StaticClass();
};

class UEEnum : public UEField
{
public:
	using UEField::UEField;

	std::vector<std::string> GetNames() const;

	static UEClass StaticClass();
};

class UEConst : public UEField
{
public:
	using UEField::UEField;

	std::string GetValue() const;

	static UEClass StaticClass();
};

class UEFField
{
public:
	FField* field;

	UEFField()
		: field(nullptr)
	{

	}

	UEFField(FField* _field)
		: field(_field)
	{

	}

	UEFField GetNext() const
	{
		return UEFField(field->Next);
	}

	bool IsValid() const
	{
		return field != nullptr;
	}

	template<typename Base>
	Base Cast() const
	{
		return Base(field);
	}

	std::string GetName() const
	{
		return field->NamePrivate.ToString();
	}

	std::string GetFullName() const
	{
		return field->ClassPrivate->Name.ToString() + " " + field->NamePrivate.ToString();
	}
};

class UEStruct : public UEField
{
public:
	using UEField::UEField;

	UEStruct GetSuper() const;
	
	UEField GetChildren() const;

	UEFField GetChildProperties() const;

	size_t GetPropertySize() const;

	static UEClass StaticClass();
};

class UEScriptStruct : public UEStruct
{
public:
	using UEStruct::UEStruct;

	static UEClass StaticClass();
};

class UEFunction : public UEStruct
{
public:
	using UEStruct::UEStruct;

	UEFunctionFlags GetFunctionFlags() const;

	static UEClass StaticClass();
};

class UEClass : public UEStruct
{
public:
	using UEStruct::UEStruct;

	static UEClass StaticClass();
};

class UEProperty : public UEFField
{
public:
	using UEFField::UEFField;

	size_t GetArrayDim() const;

	size_t GetElementSize() const;

	UEPropertyFlags GetPropertyFlags() const;

	size_t GetOffset() const;

	enum class PropertyType
	{
		Unknown,
		Primitive,
		PredefinedStruct,
		CustomStruct,
		Container
	};

	struct Info
	{
		PropertyType Type;
		size_t Size;
		bool CanBeReference;
		std::string CppType;

		static Info Create(PropertyType type, size_t size, bool reference, std::string&& cppType)
		{
			extern IGenerator* generator;

			return { type, size, reference, generator->GetOverrideType(cppType) };
		}
	};

	Info GetInfo() const;
};

class UEByteProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	bool IsEnum() const;

	UEEnum GetEnum() const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Byte;
	}
};

class UEUInt16Property : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::UInt16;
	}
};

class UEUInt32Property : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::UInt32;
	}
};

class UEUInt64Property : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::UInt64;
	}
};

class UEInt8Property : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Int8;
	}
};

class UEInt16Property : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Int16;
	}
};

class UEIntProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Int;
	}
};

class UEInt64Property : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Int64;
	}
};

class UEFloatProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Float;
	}
};

class UEDoubleProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Double;
	}
};

class UEBoolProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	bool IsNativeBool() const { return GetFieldMask() == 0xFF; }

	bool IsBitfield() const { return !IsNativeBool(); }

	uint8_t GetFieldSize() const;

	uint8_t GetByteOffset() const;

	uint8_t GetByteMask() const;

	uint8_t GetFieldMask() const;

	std::array<int, 2> GetMissingBitsCount(const UEBoolProperty& other) const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Bool;
	}
};

inline bool operator<(const UEBoolProperty& lhs, const UEBoolProperty& rhs)
{
	if (lhs.GetByteOffset() == rhs.GetByteOffset())
	{
		return lhs.GetByteMask() < rhs.GetByteMask();
	}
	return lhs.GetByteOffset() < rhs.GetByteOffset();
}

class UEObjectPropertyBase : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEClass GetPropertyClass() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Object;
	}
};

class UEObjectProperty : public UEObjectPropertyBase
{
public:
	using UEObjectPropertyBase::UEObjectPropertyBase;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Object;
	}
};

class UEClassProperty : public UEObjectProperty
{
public:
	using UEObjectProperty::UEObjectProperty;

	UEClass GetMetaClass() const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Class;
	}
};

class UEInterfaceProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEClass GetInterfaceClass() const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Interface;
	}
};

class UENameProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Name;
	}
};

class UEStructProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEScriptStruct GetStruct() const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Struct;
	}
};

class UEStrProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::String;
	}
};

class UETextProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Text;
	}
};

class UEArrayProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty GetInner() const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Array;
	}
};

class UEMapProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty GetKeyProperty() const;
	UEProperty GetValueProperty() const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Map;
	}
};

class UEEnumProperty : public UEProperty
{
public:
	using UEProperty::UEProperty;

	UEProperty GetUnderlyingProperty() const;
	UEEnum GetEnum() const;

	UEProperty::Info GetInfo() const;

	static FFieldClassID GetId()
	{
		return FFieldClassID::Enum;
	}
};

template<typename T>
bool UEObject::IsA() const
{
	if (!object)
		return false;

	if (!IsValid())
		return false;

	auto cmp = T::StaticClass();
	if (!cmp.IsValid())
	{
		return false;
	}

	for (auto super = GetClass(); super.IsValid(); super = super.GetSuper().Cast<UEClass>())
	{
		if (super.object == cmp.object)
		{
			return true;
		}
	}

	return false;
}
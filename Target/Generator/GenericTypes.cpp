#include "../IGenerator.hpp"
#include "GenericTypes.hpp"
#include "ObjectsStore.hpp"
#include "NameValidator.hpp"

#include "EngineClasses.hpp"

//---------------------------------------------------------------------------
//UEObject
//---------------------------------------------------------------------------
size_t UEObject::GetIndex() const
{
	return object->InternalIndex;
}
//---------------------------------------------------------------------------
UEClass UEObject::GetClass() const
{
	return UEClass(object->Class);
}
//---------------------------------------------------------------------------
UEObject UEObject::GetOuter() const
{
	return UEObject(object->Outer);
}
//---------------------------------------------------------------------------
std::string UEObject::GetName() const
{
	auto name = object->Name.ToString();
	if (object->Name.Number > 0)
	{
		name += '_' + std::to_string(object->Name.Number);
	}

	auto pos = name.rfind('/');
	if (pos == std::string::npos)
	{
		return name;
	}

	return name.substr(pos + 1);
}
//---------------------------------------------------------------------------
UEClass UEObject::StaticClass()
{
	static auto c = ObjectsStore().FindClass("Class CoreUObject.Object");
	return c;
}
//---------------------------------------------------------------------------
//UEField
//---------------------------------------------------------------------------
UEField UEField::GetNext() const
{
	return UEField(static_cast<UField*>(object)->Next);
}
//---------------------------------------------------------------------------
UEClass UEField::StaticClass()
{
	static auto c = ObjectsStore().FindClass("Class CoreUObject.Field");
	return c;
}
//---------------------------------------------------------------------------
//UEEnum
//---------------------------------------------------------------------------
std::vector<std::string> UEEnum::GetNames() const
{
	std::vector<std::string> buffer;
	auto& names = static_cast<UEnum*>(object)->Names;

	for (auto i = 0; i < names.Num(); ++i)
	{
		buffer.push_back(SimplifyEnumName(names[i].Key.ToString()));
	}

	return buffer;
}
//---------------------------------------------------------------------------
UEClass UEEnum::StaticClass()
{
	static auto c = ObjectsStore().FindClass("Class CoreUObject.Enum");
	return c;
}
//---------------------------------------------------------------------------
//UEConst
//---------------------------------------------------------------------------
std::string UEConst::GetValue() const
{
	throw;
}
//---------------------------------------------------------------------------
UEClass UEConst::StaticClass()
{
	//not supported by UE4
	return nullptr;
}
//---------------------------------------------------------------------------
//UEStruct
//---------------------------------------------------------------------------
UEStruct UEStruct::GetSuper() const
{
	return UEStruct(static_cast<UStruct*>(object)->SuperStruct);
}
//---------------------------------------------------------------------------
UEField UEStruct::GetChildren() const
{
	return UEField(static_cast<UStruct*>(object)->Children);
}
UEFField UEStruct::GetChildProperties() const
{
	return UEFField(static_cast<UStruct*>(object)->ChildProperties);
}
//---------------------------------------------------------------------------
size_t UEStruct::GetPropertySize() const
{
	return static_cast<UStruct*>(object)->PropertiesSize;
}
//---------------------------------------------------------------------------
UEClass UEStruct::StaticClass()
{
	static auto c = ObjectsStore().FindClass("Class CoreUObject.Struct");
	return c;
}
//---------------------------------------------------------------------------
//UEScriptStruct
//---------------------------------------------------------------------------
UEClass UEScriptStruct::StaticClass()
{
	static auto c = ObjectsStore().FindClass("Class CoreUObject.ScriptStruct");
	return c;
}
//---------------------------------------------------------------------------
//UEFunction
//---------------------------------------------------------------------------
UEFunctionFlags UEFunction::GetFunctionFlags() const
{
	return (UEFunctionFlags)static_cast<UFunction*>(object)->FunctionFlags;
}
//---------------------------------------------------------------------------
UEClass UEFunction::StaticClass()
{
	static auto c = ObjectsStore().FindClass("Class CoreUObject.Function");
	return c;
}
//---------------------------------------------------------------------------
//UEClass
//---------------------------------------------------------------------------
UEClass UEClass::StaticClass()
{
	static auto c = ObjectsStore().FindClass("Class CoreUObject.Class");
	return c;
}
//---------------------------------------------------------------------------
//UEProperty
//---------------------------------------------------------------------------
size_t UEProperty::GetArrayDim() const
{
	return static_cast<FProperty*>(field)->ArrayDim;
}
//---------------------------------------------------------------------------
size_t UEProperty::GetElementSize() const
{
	return static_cast<FProperty*>(field)->ElementSize;
}
//---------------------------------------------------------------------------
UEPropertyFlags UEProperty::GetPropertyFlags() const
{
	return (UEPropertyFlags)static_cast<FProperty*>(field)->PropertyFlags;
}
//---------------------------------------------------------------------------
size_t UEProperty::GetOffset() const
{
	return static_cast<FProperty*>(field)->Offset_Internal;
}
//---------------------------------------------------------------------------
//UEByteProperty
//---------------------------------------------------------------------------
UEEnum UEByteProperty::GetEnum() const
{
	return UEEnum(static_cast<FByteProperty*>(field)->Enum);
}
//---------------------------------------------------------------------------
UEProperty::Info UEByteProperty::GetInfo() const
{
	if (IsEnum())
	{
		return Info::Create(PropertyType::Primitive, sizeof(uint8_t), false, "TEnumAsByte<" + MakeUniqueCppName(GetEnum()) + ">");
	}
	return Info::Create(PropertyType::Primitive, sizeof(uint8_t), false, "unsigned char");
}
//---------------------------------------------------------------------------
//UEUInt16Property
//---------------------------------------------------------------------------
UEProperty::Info UEUInt16Property::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(uint16_t), false, "uint16_t");
}
//---------------------------------------------------------------------------
//UEUInt32Property
//---------------------------------------------------------------------------
UEProperty::Info UEUInt32Property::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(uint32_t), false, "uint32_t");
}
//---------------------------------------------------------------------------
//UEUInt64Property
//---------------------------------------------------------------------------
UEProperty::Info UEUInt64Property::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(uint64_t), false, "uint64_t");
}
//---------------------------------------------------------------------------
//UEInt8Property
//---------------------------------------------------------------------------
UEProperty::Info UEInt8Property::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(int8_t), false, "int8_t");
}
//---------------------------------------------------------------------------
//UEInt16Property
//---------------------------------------------------------------------------
UEProperty::Info UEInt16Property::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(int16_t), false, "int16_t");
}
//---------------------------------------------------------------------------
//UEIntProperty
//---------------------------------------------------------------------------
UEProperty::Info UEIntProperty::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(int), false, "int");
}
//---------------------------------------------------------------------------
//UEInt64Property
//---------------------------------------------------------------------------
UEProperty::Info UEInt64Property::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(int64_t), false, "int64_t");
}
//---------------------------------------------------------------------------
//UEFloatProperty
//---------------------------------------------------------------------------
UEProperty::Info UEFloatProperty::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(float), false, "float");
}
//---------------------------------------------------------------------------
//UEDoubleProperty
//---------------------------------------------------------------------------
UEProperty::Info UEDoubleProperty::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(double), false, "double");
}
//---------------------------------------------------------------------------
//UEBoolProperty
//---------------------------------------------------------------------------
uint8_t UEBoolProperty::GetFieldSize() const
{
	return static_cast<FBoolProperty*>(field)->FieldSize;
}
//---------------------------------------------------------------------------
uint8_t UEBoolProperty::GetByteOffset() const
{
	return static_cast<FBoolProperty*>(field)->ByteOffset;
}
//---------------------------------------------------------------------------
uint8_t UEBoolProperty::GetByteMask() const
{
	return static_cast<FBoolProperty*>(field)->ByteMask;
}
//---------------------------------------------------------------------------
uint8_t UEBoolProperty::GetFieldMask() const
{
	return static_cast<FBoolProperty*>(field)->FieldMask;
}
//---------------------------------------------------------------------------
UEProperty::Info UEBoolProperty::GetInfo() const
{
	if (IsNativeBool())
	{
		return Info::Create(PropertyType::Primitive, sizeof(bool), false, "bool");
	}
	return Info::Create(PropertyType::Primitive, sizeof(unsigned char), false, "unsigned char");
}
//---------------------------------------------------------------------------
//UEObjectPropertyBase
//---------------------------------------------------------------------------
UEClass UEObjectPropertyBase::GetPropertyClass() const
{
	return UEClass(static_cast<FObjectPropertyBase*>(field)->PropertyClass);
}
//---------------------------------------------------------------------------
//UEObjectProperty
//---------------------------------------------------------------------------
UEProperty::Info UEObjectProperty::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(void*), false, "class " + MakeValidName(GetPropertyClass().GetNameCPP()) + "*");
}
//---------------------------------------------------------------------------
//UEClassProperty
//---------------------------------------------------------------------------
UEClass UEClassProperty::GetMetaClass() const
{
	return UEClass(static_cast<FObjectPropertyBase*>(field)->PropertyClass);
}
//---------------------------------------------------------------------------
UEProperty::Info UEClassProperty::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(void*), false, "class " + MakeValidName(GetMetaClass().GetNameCPP()) + "*");
}
//---------------------------------------------------------------------------
//UEInterfaceProperty
//---------------------------------------------------------------------------
UEClass UEInterfaceProperty::GetInterfaceClass() const
{
	return UEClass(static_cast<FInterfaceProperty*>(field)->InterfaceClass);
}
//---------------------------------------------------------------------------
UEProperty::Info UEInterfaceProperty::GetInfo() const
{
	return Info::Create(PropertyType::PredefinedStruct, sizeof(FScriptInterface), true, "TScriptInterface<class " + MakeValidName(GetInterfaceClass().GetNameCPP()) + ">");
}
//---------------------------------------------------------------------------
//UENameProperty
//---------------------------------------------------------------------------
UEProperty::Info UENameProperty::GetInfo() const
{
	return Info::Create(PropertyType::PredefinedStruct, sizeof(FName), true, "struct FName");
}
//---------------------------------------------------------------------------
//UEStructProperty
//---------------------------------------------------------------------------
UEScriptStruct UEStructProperty::GetStruct() const
{
	return UEScriptStruct(static_cast<FStructProperty*>(field)->Struct);
}
//---------------------------------------------------------------------------
UEProperty::Info UEStructProperty::GetInfo() const
{
	return Info::Create(PropertyType::CustomStruct, GetElementSize(), true, "struct " + MakeUniqueCppName(GetStruct()));
}
//---------------------------------------------------------------------------
//UEStrProperty
//---------------------------------------------------------------------------
UEProperty::Info UEStrProperty::GetInfo() const
{
	return Info::Create(PropertyType::PredefinedStruct, sizeof(FString), true, "struct FString");
}
//---------------------------------------------------------------------------
//UETextProperty
//---------------------------------------------------------------------------
UEProperty::Info UETextProperty::GetInfo() const
{
	return Info::Create(PropertyType::PredefinedStruct, sizeof(FText), true, "struct FText");
}
//---------------------------------------------------------------------------
//UEArrayProperty
//---------------------------------------------------------------------------
UEProperty UEArrayProperty::GetInner() const
{
	return UEProperty(static_cast<FArrayProperty*>(field)->Inner);
}
//---------------------------------------------------------------------------
UEProperty::Info UEArrayProperty::GetInfo() const
{
	const auto inner = GetInner().GetInfo();
	if (inner.Type != PropertyType::Unknown)
	{
		extern IGenerator* generator;

		return Info::Create(PropertyType::Container, sizeof(TArray<void*>), false, "TArray<" + generator->GetOverrideType(inner.CppType) + ">");
	}
	
	return { PropertyType::Unknown };
}
//---------------------------------------------------------------------------
//UEMapProperty
//---------------------------------------------------------------------------
UEProperty UEMapProperty::GetKeyProperty() const
{
	return UEProperty(static_cast<FMapProperty*>(field)->KeyProp);
}
//---------------------------------------------------------------------------
UEProperty UEMapProperty::GetValueProperty() const
{
	return UEProperty(static_cast<FMapProperty*>(field)->ValueProp);
}
//---------------------------------------------------------------------------
UEProperty::Info UEMapProperty::GetInfo() const
{
	const auto key = GetKeyProperty().GetInfo();
	const auto value = GetValueProperty().GetInfo();
	if (key.Type != PropertyType::Unknown && value.Type != PropertyType::Unknown)
	{
		extern IGenerator* generator;

		return Info::Create(PropertyType::Container, 0x50, false, "TMap<" + generator->GetOverrideType(key.CppType) + ", " + generator->GetOverrideType(value.CppType) + ">");
	}

	return { PropertyType::Unknown };
}
//---------------------------------------------------------------------------
//UEEnumProperty
//---------------------------------------------------------------------------
UEProperty UEEnumProperty::GetUnderlyingProperty() const
{
	return UEProperty(static_cast<FEnumProperty*>(field)->UnderlyingProp);
}
//---------------------------------------------------------------------------
UEEnum UEEnumProperty::GetEnum() const
{
	return UEEnum(static_cast<FEnumProperty*>(field)->Enum);
}
//---------------------------------------------------------------------------
UEProperty::Info UEEnumProperty::GetInfo() const
{
	return Info::Create(PropertyType::Primitive, sizeof(uint8_t), false, MakeUniqueCppName(GetEnum()));
}
//---------------------------------------------------------------------------
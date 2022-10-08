#include "GenericTypes.hpp"

void* UEObject::GetAddress() const
{
	return object;
}

UEObject UEObject::GetPackageObject() const
{
	UEObject package(nullptr);

	for (auto outer = GetOuter(); outer.IsValid(); outer = outer.GetOuter())
	{
		package = outer;
	}

	return package;
}

std::string UEObject::GetFullName() const
{
	if (GetClass().IsValid())
	{
		std::string temp;

		for (auto outer = GetOuter(); outer.IsValid(); outer = outer.GetOuter())
		{
			temp = outer.GetName() + "." + temp;
		}

		std::string name = GetClass().GetName();
		name += " ";
		name += temp;
		name += GetName();

		return name;
	}

	return std::string("(null)");
}

std::string UEObject::GetNameCPP() const
{
	std::string name;

	if (IsA<UEClass>())
	{
		auto c = Cast<UEClass>();
		while (c.IsValid())
		{
			const auto className = c.GetName();
			if (className == "Actor")
			{
				name += "A";
				break;
			}
			if (className == "Object")
			{
				name += "U";
				break;
			}

			c = c.GetSuper().Cast<UEClass>();
		}
	}
	else
	{
		name += "F";
	}

	name += GetName();

	return name;
}

UEProperty::Info UEProperty::GetInfo() const
{
	if (IsValid())
	{
		if (field->ClassPrivate->Id == FFieldClassID::Byte)
		{
			return Cast<UEByteProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::UInt16)
		{
			return Cast<UEUInt16Property>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::UInt32)
		{
			return Cast<UEUInt32Property>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::UInt32)
		{
			return Cast<UEUInt64Property>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Int8)
		{
			return Cast<UEInt8Property>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Int16)
		{
			return Cast<UEInt16Property>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Int)
		{
			return Cast<UEIntProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Int64)
		{
			return Cast<UEInt64Property>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Float)
		{
			return Cast<UEFloatProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Double)
		{
			return Cast<UEDoubleProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Bool)
		{
			return Cast<UEBoolProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Object || field->ClassPrivate->Id == FFieldClassID::ObjectPointer)
		{
			return Cast<UEObjectProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Class)
		{
			return Cast<UEClassProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Name)
		{
			return Cast<UENameProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Struct)
		{
			return Cast<UEStructProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::String)
		{
			return Cast<UEStrProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Text)
		{
			return Cast<UETextProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Array)
		{
			return Cast<UEArrayProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Map)
		{
			return Cast<UEMapProperty>().GetInfo();
		}
		if (field->ClassPrivate->Id == FFieldClassID::Enum)
		{
			return Cast<UEEnumProperty>().GetInfo();
		}
	}
	return { PropertyType::Unknown };
}

//---------------------------------------------------------------------------
//UEByteProperty
//---------------------------------------------------------------------------
bool UEByteProperty::IsEnum() const
{
	return GetEnum().IsValid();
}
//---------------------------------------------------------------------------
//UEBoolProperty
//---------------------------------------------------------------------------

int GetBitPosition(uint8_t value)
{
	int i4 = !(value & 0xf) << 2;
	value >>= i4;

	int i2 = !(value & 0x3) << 1;
	value >>= i2;

	int i1 = !(value & 0x1);

	int i0 = (value >> i1) & 1 ? 0 : -8;

	return i4 + i2 + i1 + i0;
}

std::array<int, 2> UEBoolProperty::GetMissingBitsCount(const UEBoolProperty& other) const
{
	// If there is no previous bitfield member, just calculate the missing bits.
	if (!other.IsValid())
	{
		return { GetBitPosition(GetByteMask()), -1 };
	}

	// If both bitfield member belong to the same byte, calculate the bit position difference.
	if (GetOffset() == other.GetOffset())
	{
		return { GetBitPosition(GetByteMask()) - GetBitPosition(other.GetByteMask()) - 1, -1 };
	}

	// If they have different offsets, we need two distances
	// |00001000|00100000|
	// 1.   ^---^
	// 2.       ^--^
	
	return { std::numeric_limits<uint8_t>::digits - GetBitPosition(other.GetByteMask()) - 1, GetBitPosition(GetByteMask()) };
}
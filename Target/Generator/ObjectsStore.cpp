#include <windows.h>

#include "PatternFinder.hpp"
#include "ObjectsStore.hpp"

#include "EngineClasses.hpp"

bool ObjectsStore::Initialize()
{
	auto pGObjects = FindPattern("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1", true, 3);
	auto pFNameToString = FindPattern("48 89 5C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 19 48 8B F2 0F B7 FB 4C 8B F1 E8 ? ? ? ? 44 8B C3 8D 1C 3F 49 C1 E8 10 33 FF 4A 03 5C C0 ? 41 8B 46 04");

	if (!pGObjects)
	{
		MessageBoxA(0, "GObjects", "Failed to find", MB_ICONERROR);
		return false;
	}

	if (!pFNameToString)
	{
		MessageBoxA(0, "FName::ToString()", "Failed to find", MB_ICONERROR);
		return false;
	}

	GlobalObjects = decltype(GlobalObjects)(pGObjects);
	FNameToString = decltype(FNameToString)(pFNameToString);

	return true;
}

void* ObjectsStore::GetAddress()
{
	return GlobalObjects;
}

size_t ObjectsStore::GetObjectsNum() const
{
	return GlobalObjects->Num();
}

UEObject ObjectsStore::GetById(size_t id) const
{
	return GlobalObjects->GetObjectPtr(id);
}

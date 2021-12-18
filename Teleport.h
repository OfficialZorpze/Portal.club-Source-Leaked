#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <iostream>
#include "Defines.h"
#include "SDK.h"
#include "detours.h"


class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};


inline bool ProcessEvent(uintptr_t address, void* fnobject, void* parms)
{
	UObject* addr = reinterpret_cast<UObject*>(address); if (!addr) return false;
	auto vtable = *reinterpret_cast<void***>(addr); if (!vtable) return false;
	auto processEventFn = static_cast<void(*)(void*, void*, void*)>(vtable[0x44]); if (!processEventFn) return false;
	SpoofCall(processEventFn, (void*)addr, (void*)fnobject, (void*)parms);
	return true;
}

//----------------------------------------------------------------------------------

static void FreeObjectName(__int64 address) {
	auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(FreeFn);
	SpoofCall(func, address);
}

static const char* GetUintObjectName(uintptr_t Object)
{
	if (Object == NULL) return "";

	auto fGetObjName = reinterpret_cast<SDK::Structs::FString * (__fastcall*)(int* index, SDK::Structs::FString * res)> (GetNameByIndex);

	int index = *(int*)(Object + 0x18);

	SDK::Structs::FString result;
	SpoofCall(fGetObjName, &index, &result);

	if (result.c_str() == NULL) return "";

	auto result_str = result.ToString();

	if (result.c_str() != NULL)
		FreeObjectName((__int64)result.c_str());

	return result_str.c_str();
}


static const char* GetUObjectName(UObject* Object)
{
	if (Object == NULL) return "";

	auto fGetObjName = reinterpret_cast<SDK::Structs::FString * (__fastcall*)(int* index, SDK::Structs::FString * res)>(GetNameByIndex);

	int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);

	SDK::Structs::FString result;
	SpoofCall(fGetObjName, &index, &result);

	if (result.c_str() == NULL) return "";

	auto result_str = result.ToString();

	if (result.c_str() != NULL)
		FreeObjectName((__int64)result.c_str());

	return result_str.c_str();
}


static const char* GetUObjectNameLoop(UObject* Object) {
	std::string name("");
	for (auto i = 0; Object; Object = Object->Outer, ++i) {

		auto fGetObjName = reinterpret_cast<SDK::Structs::FString * (__fastcall*)(int* index, SDK::Structs::FString * res)>(GetNameByIndex);

		int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);

		SDK::Structs::FString fObjectName;
		SpoofCall(fGetObjName, &index, &fObjectName);

		if (!fObjectName.IsValid()) {
			break;
		}

		auto objectName = fObjectName.ToString();


		name = objectName + std::string(i > 0 ? "." : "") + name;
		FreeObjectName((uintptr_t)fObjectName.c_str());
	}

	return name.c_str();
}




PVOID FindObject(std::string name) {
	static bool once = false;
	if (!once)
	{

		auto UObjectPtr = MemoryHelper::PatternScan("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1");
		UObjectPtr = decltype(UObjectPtr)(RVA(UObjectPtr, 7));

		once = true;
	}


	for (auto array : GObjects->ObjectArray->Objects) {
		auto fuObject = array;
		std::cout << "";
		for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject) {
			std::cout << "";
			auto object = fuObject->Object;
			if (object->ObjectFlags != 0x41) {
				continue;
			}
			std::cout << "";

			if (strstr(GetUObjectNameLoop(object), name.c_str())) {
				return object;
			}
		}
	}
	return 0;
}

//------------------------------------------------------------------------------------------------ -

namespace FNObjects {
	PVOID GetPlayerName;
	PVOID FOV;
	PVOID K2_SetActorLocation;
	PVOID K2_TeleportTo;
}

void ScanFNObjects() {
	FNObjects::GetPlayerName = FindObject(xorstr("GetPlayerName"));
	std::cout << xorstr("GetPlayerName: ") << FNObjects::GetPlayerName << "\n";

	FNObjects::FOV = FindObject(xorstr("FOV"));
	std::cout << xorstr("FOV: ") << FNObjects::FOV << xorstr("\n");

	FNObjects::K2_SetActorLocation = FindObject(xorstr("K2_SetActorLocation"));
	std::cout << xorstr("K2_SetActorLocation: ") << FNObjects::K2_SetActorLocation << xorstr("\n");

	FNObjects::K2_TeleportTo = FindObject(xorstr("K2_TeleportTo"));
	std::cout << xorstr("K2_TeleportTo: ") << FNObjects::K2_TeleportTo << xorstr("\n");
}
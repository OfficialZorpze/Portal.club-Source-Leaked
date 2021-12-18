#pragma once
#include <Windows.h>

namespace StaticOffsets {
	uintptr_t OwningGameInstance = 0x190;
	uintptr_t LocalPlayers = 0x38;
	uintptr_t PlayerController = 0x30;
	uintptr_t PlayerCameraManager = 0x2C0;
	uintptr_t AcknowledgedPawn = 0x2A8;
	uintptr_t PrimaryPickupItemEntry = 0x2A8;
	uintptr_t ItemDefinition = 0x98;
	uintptr_t DisplayName = 0x88;
	uintptr_t Tier = 0x6C;
	uintptr_t WeaponData = 0x378;
	uintptr_t LastFireTime = 0x98;
	uintptr_t LastFireTimeVerified = 0x9E8;
	uintptr_t LastFireAbilityTime = 0x1238;
	uintptr_t CurrentWeapon = 0x5F8;
    uintptr_t bADSWhileNotOnGround = 0x3E51;
	uintptr_t AmmoCount = 0xA80;
	uintptr_t CurrentAmmoCount = 0xED0;
	uintptr_t bShouldDisplayAmmoCounter = 0x14A4;
	uintptr_t ClipSize = 0x11C;
	uintptr_t bIsReloading = 0x6D8;

    uintptr_t CostumTimeDilation = 0x9E4;
	uintptr_t Levels = 0x148;
	uintptr_t PersistentLevel = 0x30;
	uintptr_t AActors = 0x98;
	uintptr_t ActorCount = 0xA0;

	uintptr_t bIsReloadingWeapon = 0x2B9;
    uintptr_t ReviveFromDBNOTime = 0x3758;
	uintptr_t RootComponent = 0x130;
	uintptr_t FireStartLoc = 0xB90;
	uintptr_t RelativeLocation = 0x11C;
	uintptr_t RelativeRotation = 0x128;
	uintptr_t PlayerState = 0x238;
	uintptr_t Mesh = 0x280;
	uintptr_t TeamIndex = 0xF28;
	uintptr_t VehicleGravity = 0x74;
	uintptr_t StaticMesh = 0x488;
	PVOID GetPlayerName = 0;
	PVOID VehicleTargetPawn = nullptr;
	uintptr_t ClosestPlayerCoords;
	PVOID ClosestVehicle = nullptr;
}

PVOID(*GetWeaponStats)(PVOID) = nullptr;




namespace addresses
{
}



class UActorComponent {
public:
	uintptr_t RelativeLocation = 0x11C;
};
UActorComponent* USceneComponent;




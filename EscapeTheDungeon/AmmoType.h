#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm		UMETA(DisplayName = "9mm"),
	EAT_AR		UMETA(DisplayName = "AssaultRifle"),
	EAT_776		UMETA(DisplayName = "Rifles"),
	EAT_Shells	UMETA(DisplayName = "Shotgun Shells"),

	EAT_MAX		UMETA(DisplayName = "DefaultMAX")
};
#pragma once

UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	EWT_SubmachineGun	UMETA(DisplayName = "SubmachinGun"),
	EWT_AssaultRifle	UMETA(DisplayName = "AssaultRifle"),
	EWT_Pistol			UMETA(DisplayName = "Pistol"),
	EWT_ShotGun			UMETA(DisplayName = "Shotgun"),
	EWT_SpecialAR		UMETA(DisplayName = "SpecialAR"),
	EWT_NPCPistol		UMETA(DisplayName = "NPCPistol"),
	EWT_NPCGun			UMETA(DisplayName = "NPCgun"),
	EWT_PlayerAR		UMETA(DisplayName = "PlayerAR"),


	EWT_MAX				UMETA(DisplayName = "DefaultMax")
};
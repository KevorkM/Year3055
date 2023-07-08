// This game is fully owned by KMG


#include "Guns.h"
#include "MainCharacter.h"
#include "MySaveGame.h"

#include <Components/SkeletalMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/SkeletalMeshSocket.h>
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystemComponent.h>
#include <DrawDebugHelpers.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <TimerManager.h>

AGuns::AGuns()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClipBoneName = TEXT("MAG");

	ThrowWeaponTime = 0.75f;

	bFalling = false;

	Ammo = 30;
	MagazineCapacity = 30;

	WeaponType = EWeaponType::EWT_SubmachineGun;
	AmmoType = EAmmoType::EAT_AR;

	ReloadMontageSection = FName(TEXT("Reload_AR"));

}

void AGuns::BeginPlay()
{
	Super::BeginPlay();
}

void AGuns::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Keept the weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	
	}
}

void AGuns::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	// Direction in which we throw the Weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation{ 30.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer( ThrowWeaponTimer, this, &AGuns::StopFalling, ThrowWeaponTime);
}

void AGuns::DecrementAmmo()
{
	if (Ammo - 1 <= 0) {
		Ammo = 0;
	}
	else {
		--Ammo;
	}
}

void AGuns::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attempted to reload with more that magazine capacity!"));

	Ammo += Amount;
}

bool AGuns::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AGuns::StopFalling()
{
	bFalling = false;

	SetItemState(EItemState::EIS_Pickup);
}

void AGuns::OnConstruction(const FTransform& Transform)
{
	const FString WeaponTablePath{ TEXT("DataTable'/Game/_DataTable/DT_WeaponDataTable.DT_WeaponDataTable'") };
	UDataTable* WeaponTableObject = Cast <UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject) {

		FWeaponDataType* WeaponDataRow = nullptr;
		switch (WeaponType) {

		case EWeaponType::EWT_SubmachineGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataType>(FName("Submachine"), TEXT(""));
			
			break;
		case EWeaponType::EWT_AssaultRifle:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataType>(FName("AssaultRiffle"), TEXT(""));
			break;
		case EWeaponType::EWT_Pistol:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataType>(FName("Pistol"), TEXT(""));
			break;
		case EWeaponType::EWT_SpecialAR:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataType>(FName("SpecialAR"), TEXT(""));
			break;
		case EWeaponType::EWT_PlayerAR:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataType>(FName("PlayerAR"), TEXT(""));
			break;
		case EWeaponType::EWT_NPCPistol:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataType>(FName("NPCPistol"), TEXT(""));
			break;
		case EWeaponType::EWT_NPCGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataType>(FName("NPCGun"), TEXT(""));
			break;
		}
		if (WeaponDataRow) {

			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazineCapacity;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquippedSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->SkeletalMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetIconItem(WeaponDataRow -> InventoryIcon);
			SetAmmoItem(WeaponDataRow->AmmoIcon);
			SetClipBoneName(WeaponDataRow->ClipBoneName);
			SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			FireSound = WeaponDataRow->FireSound;
			Damage = WeaponDataRow->Damage;
			HeadShotDamage = WeaponDataRow->HeadShotDamage;
			FireCamShake = WeaponDataRow->CameraShake;
		}
	}
}

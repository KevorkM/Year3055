// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "AmmoType.h"
#include "WeaponType.h"
#include "Engine/DataTable.h"
#include "Guns.generated.h"


USTRUCT(BlueprintType)
struct FWeaponDataType :public FTableRowBase {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
		FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<UAnimInstance> AnimBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float HeadShotDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> CameraShake;

};

UCLASS()
class ESCAPETHEDUNGEON_API AGuns : public APickupItem
{
	GENERATED_BODY()

		AGuns();

	/* Variables*/
public:

	/*Customize Socket name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FName RightHand;

	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
		FString GunName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon||Camera")
		TSubclassOf<class UCameraShakeBase>FireCamShake;

	/* The Type of weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
		EWeaponType WeaponType;

protected:

	FTimerHandle ThrowWeaponTimer;

	float ThrowWeaponTime;

	bool bFalling;

	/* Ammo count for this weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
		int32 Ammo;

	/* Max ammo that our weapon can hold*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	int32 MagazineCapacity;

	/* Type of ammo for this weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType;

	/* Data table for weapon properties*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data Table", meta = (AllowPrivateAccess = true))
	UDataTable* WeaponDataTable;

	/* FName for the reload montage section*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	FName ReloadMontageSection;

	/* true when moving the clip while reloading*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	bool bMovingClip;

	/* Name for the clip Bone*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	FName ClipBoneName;

	/* The speed which Auto fire happens*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	float AutoFireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
		class UParticleSystem* MuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
		USoundCue* FireSound;

	/* Amount of damage caused by bullet*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	float Damage;

	/* Amount of damage when the bullet hits a critical spot*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	float HeadShotDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		AMainCharacter* Main;

public:
	//ForceInline

	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE void SetReloadMontageSection(FName Name) { ReloadMontageSection = Name; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE void SetClipBoneName(FName Name) { ClipBoneName = Name; }

	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }

	FORCEINLINE float GetDamage() const { return Damage; }

	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

protected:

	/* Functions*/
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

/* Functions*/
public:

	bool ClipIsFull();


protected:

	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	/*Adds impule to the weapon */
	void ThrowWeapon();

	/* Called from Character class when firing weapon*/
	void DecrementAmmo();

	void ReloadAmmo(int32 Amount);

};

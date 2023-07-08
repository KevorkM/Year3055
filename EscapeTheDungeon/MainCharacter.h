// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "WeaponType.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal		UMETA(DisplayName = "Normal"),
	EMS_Sprinting	UMETA(DisplayName = "Sprinting"),
	EMS_Crouching	UMETA(DisplayName = "Crouching"),
	EMS_Dead		UMETA(DisplayName = "Dead"),

	EMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus :uint8
{
	ESS_Normal				UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum		UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted			UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX					UMETA(DisplayName = "DefaultMax")

};


UENUM(BlueprintType)
enum class EEquippedState : uint8 {

	EES_Equipped		UMETA(DisplatName = "Equipped"),
	EES_UnEquipped		UMETA(DisplatName = "UnEquipped"),

	EES_MAX				UMETA(DisplatName = "Max")

};

UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_Unoccupied				UMETA(DisplayName = " Unoccupied"),
	ECS_FireTimerInProgress		UMETA(DisplayName = " FireTimerInProgress"),
	ECS_Reloading				UMETA(DisplayName = " Reloading"),
	ECS_Equipping				UMETA(DisplayName = " Equipping"),


	ECS_MAX						UMETA(DisplayName = " MAX")
};

USTRUCT(BlueprintType)
struct FInterpLocation 
{
	GENERATED_BODY()

	// Scene Component to use for its location for interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHightlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);



UCLASS()
class ESCAPETHEDUNGEON_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	/** Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* ImpactParticles;

	/* Save Storage*/
	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
		TSubclassOf<class AItemStorage> WeaponStorage;

private:
	/*Variables */
	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComp;

	float CurrentHealth;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent", meta = (AllowPrivateAccess = "true"))
	//	float DefaultHealth;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent", meta = (AllowPrivateAccess = "true"))
	//	float Health;

	class AMainPlayerController* PlayerController;

	//character movement

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseLookUpRate;

	/* Turn while not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipTurnRate;

	/* Look up while not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipLookupRate;

	/* Turn while aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingTurnRate;

	/* Look up while aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingLookupRate;

	/* Scaling factor for mouse look sensitivity. Turn rate when not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), meta = (AllowPrivateAccess = "true"))
		float MouseHipTurnRate;

	/* Scaling factor for mouse look sensitivity. Turn rate when aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), meta = (AllowPrivateAccess = "true"))
		float MouseAimingTurnRate;

	/* Scaling factor for mouse look sensitivity. Look up rate when not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), meta = (AllowPrivateAccess = "true"))
		float MouseHipLookupRate;

	/* Scaling factor for mouse look sensitivity. Look up rate when aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), meta = (AllowPrivateAccess = "true"))
		float MouseAimingLookupRate;


	bool bMovingForward;
	bool bMovingRight;
	/* Equipping using a key*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims", meta = (AllowPrivateAccess = "true"))
		bool bCanPickup;

	/* Equipping*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		class AGuns* EquippedGun;

	/* Enemy*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		class AEnemyCharacter* Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AGuns> DefaultWeaponClass;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		class APickupItem* ActiveOverlappingItem;

	/* Effect*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* MuzzleFlashSocket;

	/* Montage for firing from the hip*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* HipRifleFireMontage;

	/* Montage for firing while aiming*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AimRifleFireMontage;

	/* Montage for firing with the pistol*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* PistolFireMontage;

	/* Montage for reload animations*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* ReloadMontage;

	/* Montage for reload animations*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* EquipMontage;

	/* Montage for when the character is dead*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* DeathMontage;

	/* To keep track of the different ammo types*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|AmmoType", meta = (AllowPrivateAccess = "true"))
		TMap<EAmmoType, int32>AmmoMap;

	/* Starting amount of ammo for the AR*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|AmmoType", meta = (AllowPrivateAccess = "true"))
		int32 StartingARAmmo;

	/* Starting amount of ammo for the 9mm*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|AmmoType", meta = (AllowPrivateAccess = "true"))
		int32 Starting9mmAmmo;

	/* Starting amount of ammo for the 7.76*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|AmmoType", meta = (AllowPrivateAccess = "true"))
		int32 Starting776Ammo;

	/* Starting amount of ammo for the Shotgun shells*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|AmmoType", meta = (AllowPrivateAccess = "true"))
		int32 StartingShellAmmo;

	/* Combat State can only fire or reload if Unoccupied*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat", meta = (AllowPrivateAccess = "true"))
		ECombatState CombatState;


	/* Combat State can only fire or reload if Unoccupied*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat", meta = (AllowPrivateAccess = "true"))
	EEquippedState EquippedState;

	/* The Type of weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
		EWeaponType WeaponType;

	//Enum variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enum|Movement", meta = (AllowPrivateAccess = "true"))
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enum|Stamina", meta = (AllowPrivateAccess = "true"))
	EStaminaStatus StaminaStatus;

	/* Shooting effects*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Name", meta = (AllowPrivateAccess = "true"))
		FName RightHandSocketName;

	/* Shooting effects*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Name", meta = (AllowPrivateAccess = "true"))
		FName MuzzleSocketName;

	///** Flash spawned at BarrelSocket */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//	class UParticleSystem* MuzzleFlash;

	/** Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* BeamParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		bool bAiming;

	/* Shooting style*/
	// Left mouse button to fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bFireButtonPress;

	// True when we can fire. False when waiting for the timer
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bShouldFire;

	//// Rate of automatic gun fire
	//float AutomaticFireRate;

	// Set a timer between gunshots
	FTimerHandle AutoFireTimer;

	// Bullet spread in degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
		float BulletSpread;

	/* Default camera field of view value*/
	float CameraDefaultFOV;

	/* FOV value when zoomed in*/
	float CameraZoomedFOV;

	/* Current field of view this frame*/
	float CameraCurrentFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float ZoomInterpSpeed;

	/* Determines the spread of the crosshairs*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
		float CrosshairSpreadMultiplier;

	/* Velocity component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
		float CrosshairVelocityFactor;

	/* InAir component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
		float CrosshairInAirFactor;

	/* Aim component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
		float CrosshairAimFactor;

	/* Shooting component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
		float CrosshairShootingFactor;

	float ShootTimeDuration;

	bool bFiringBullet;

	FTimerHandle CrosshairShootTimer;

	bool bShouldTraceForItems;

	/* Number of overlappedItems APickupItems*/
	int8 OverlappedItemCount;

	/******/
	/* The Item that we hit last frame*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		class APickupItem* TraceHitItemLastFrame;

	/* The item currently hit by our trace in TraceForItems (Could be null)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		APickupItem* TraceHitItem;

	/* Distance outward from the camera for the interp destination*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
		float CameraInterpDistance;

	/* Distance upward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
		float CameraInterpElevation;

	/* Transform of the clip when we first grab the clip during reloading*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		FTransform ClipTransform;

	/* Scene component to attach to the characters hand during reloading*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		USceneComponent* HandSceneComponent;

	///* Sounds*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	//	class USoundCue* FireSound;

	///* Regular movement Speed*/
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	//	float BaseMovementSpeed;

	/* Crouch Movement Speed*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchMovementSpeed;

	/* Current Half Height for the capsule*/
	float CurrentCapsuleHalfHeight;

	/* Half height of the capsule when standing*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float StandingCapsuleHalfHeight;

	/* Half height of the capsule when Crouching*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchingCapsuleHalfHeight;

	/* Ground friction while not crouching*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float BaseGroundFriction;

	/* Ground friction while crouching*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchingGroundFriction;

	/* Used for when the aiming button is pressed */
	bool bAimingButtonPressed;

	/* Array of interp location structs*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;

	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	/* Blood splatter particle for when hit*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* BloodParticle;

	/* Amount of damage when the bullet hits a critical spot*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
		float HeadShotDamage;

	/* Name of the head bone | Critical area*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		FString HeadBone;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Health")
		bool bDied;

	/* True When crouching*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bCrouching;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Line trace for items under crosshairs*/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* WeaponInterpComp;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		float PickupSoundResetTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		float EquipSoundResetTime;

	/* An Array of APickupItems for the inventory*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		TArray<APickupItem*> Inventory;

	const int32 INVENTORY_CAPACITY{ 6 };

	/* Delegate for sending slot information to the InventoryBar when equipping */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
		FEquipItemDelegate EquipItemDelegate;


	/* Delegate for sending slot information for playing the icon animation */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
		FHightlightIconDelegate HightlightIconDelegate;

	/* the index for the currently highlighted slot*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		int32 HightlightedSlot;

	/* Game mechanics*/
	/* Keys you need to open a door */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
		int32 Keys;

	TArray<FVector> PickupLocations;

	// Sprinting Stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float SprintingSpeed;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	//float WalkingSpeed;

	// Stamina

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
		float MaxStamina;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats’")
		float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float MinSprintStamina;


	bool bIsAccelerating;

public:
	/* Force Inline*/

	FORCEINLINE UCameraComponent* GetCameraComponent() { return CameraComp; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* InCamera) { CameraComp = InCamera; }

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArmComp; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* InSpringArm) { SpringArmComp = InSpringArm; }

	//Equipping
	FORCEINLINE AGuns* GetEquippedGun() { return EquippedGun; }

	FORCEINLINE void SetActiveOverlappingItem(APickupItem* Item) { ActiveOverlappingItem = Item; }

	FORCEINLINE FName GetHandBoneName() const { return RightHandSocketName; }
	FORCEINLINE void SetHandBoneName(FName Name) { RightHandSocketName = Name; }

	FORCEINLINE FString GetHeadBone() const { return HeadBone; }

	FORCEINLINE TArray<FVector> SetPickupLocation() const { return PickupLocations; }

	// Aiming Get
	FORCEINLINE bool GetAiming() const { return bAiming; }
	
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const{ return OverlappedItemCount; }

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE EEquippedState GetEquippedState() const { return EquippedState; }

	FORCEINLINE bool GetCrouching() const { return bCrouching; }

	FInterpLocation GetInterpLocation(int32 Index);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }

	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	FORCEINLINE bool GetCharacterDied() const { return bDied; }
	FORCEINLINE void SetCharacterDied(bool isDead) { bDied = isDead; }

	FORCEINLINE UParticleSystem* GetBloodParticles() const { return BloodParticle; }

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }


public:

	//Setters and Inlines
	/* Equipping*/
	void EquipWeapon(AGuns* GunsToSet, bool bSwapping = false);

	/* Adds / Subtracts to/from OverlappedItemCount and updates bShouldTraceForItems*/
	void IncrementOverlappedItemCount(int8 Amount);

	// No Longer needed; APickupItem has GetInterpLocation
	//FVector GetCameraInterpLocation();

	void GetPickupItem(APickupItem* Item);

	void StartPickupSoundTimer();

	void StartEquipSoundTimer();

		//Health
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//Items
	UFUNCTION(BlueprintCallable)
	void IncrementKeys(int32 Amount);
	
	/*UFUNCTION(BlueprintCallable, Category = "HealthComponent")
		void IncrementHealth(float Amount);*/

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	//	float HealAmount;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	//	uint8 TeamNum;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Trace for items if OverlappedItemCount >0 */
	void TraceForItems();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Returns the index in the InterpLocations array with the lowest ItemCount */
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	void UnHightlightInventorySlot();

	/* Switching the level*/
	UFUNCTION(BlueprintCallable)
	void SwitchLevel(FName LevelName);

	/* Save the game*/
	UFUNCTION(BlueprintCallable)
	void SaveGame(bool SetPosition);

	/* Load the game*/
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	void LoadGameNoSwitch();

	UFUNCTION()
		void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


protected:
	//Function

	///Movement
	void MoveForward(float Value);
	void MoveBackward(float Value);
	void MoveRight(float Value);
	void MoveLeft(float Value);

	/** Called via input to turn at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void LookUpAtRate(float Rate);

	void SetLookRates();

	void Turn(float Value);

	void LookUp(float Value);

	//Actions
	virtual void Jump() override;

	void ToggleCrouch();

	AGuns* SpawnDefaultWeapon();

	void Fire();

	void FireButtonPressed();

	void FireButtonReleased();

	/* Initialize the ammo map with ammo values*/
	void InitializeAmmoMap();

	/* Check to make sure our weapon has ammo*/
	bool WeaponHasAmmo();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	///* Equipping using a key*/
	void PressEButton();

	void ReleaseEButton();
	
	
	void SelectButtonPressed();

	void SelectButtonReleased();

	/* Detach weapon and let it fall to the ground*/
	void DropWeapon();

	/* Drops currently equipped weapon and equips TraceHitItem*/
	void SwapWeapon(AGuns* WeaponToSwap);

	// The direction of the gun shots
	virtual FVector GetPawnViewLocation() const override;

	void StartMontageAnimation();

	/* Set bAiming to true or false with button press*/
	void AimingButtonPressed();
	void Aim();
	void AimingButtonReleased();
	void StopAiming();
	void CameraInterpZoom(float DeltaTime);
	void CalculatedCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	/* Fire Weapon Functions*/
	void PlayFireSound();

	void SendBullet();

	/* Bound to the R key*/
	void ReloadButtonPressed();
	/*Handle the reloading of the weapon */
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
		void FinishEquipping();

	/* Checks to see if we have ammo of the EquippedGun's ammo type*/
	bool CarryingAmmo();

	/* Called from Animation BP with Grab Clip Notify*/
	UFUNCTION(BlueprintCallable)
		void GrabClip();

	/* Called from Animation BP with Release Clip notify*/
	UFUNCTION(BlueprintCallable)
		void ReleaseClip();

	/* Interps capsule half height when Crouching or Standing*/
	void InterpCapsuleHalfHeight(float DeltaTime);

	void PickupAmmo(class AAmmo* Ammo);

	void PickupItems(class AKeyItems* KeyItems);

	void InitializeInterpLocations();

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	void HightlightInventorySlot();

	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	/* Sprinting Status*/
	void ShiftKeyDown();
	void ShiftKeyUp();

	void SetMovementStatus(EMovementStatus Status);

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();
};

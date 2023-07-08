// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Guns.h"
#include "PickupItem.h"
#include "Ammo.h"
#include "BulletHitInterface.h"
#include "EnemyCharacter.h"
#include "EnemyMeleeCharacter.h"
#include "SpecialBots.h"
#include "EnemyController.h"
#include "AIEnemyController.h"
#include "MySaveGame.h"
#include "ItemStorage.h"
#include "MainPlayerController.h"
#include "KeyItems.h"
#include "HealthComponent.h"
#include "EscapeTheDungeon.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/PlayerController.h>
#include <Engine/World.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Engine/SkeletalMeshSocket.h>
#include <DrawDebugHelpers.h>
#include <Particles/ParticleSystemComponent.h>
#include <Components/WidgetComponent.h>
#include <Components/SphereComponent.h>
#include <Components/BoxComponent.h>
#include <Curves/CurveFloat.h>
#include <Components/CapsuleComponent.h>
#include "GameFramework/DamageType.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	///Spring Arm
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 600.f;
	SpringArmComp->SetupAttachment(RootComponent);

	///Camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	/* Attach the camera to the end of SprinArmComp and let it adjust
	to match the controller orientation*/
	CameraComp->bUsePawnControlRotation = false;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	// Don't rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CrouchMovementSpeed = 350.f;
	StandingCapsuleHalfHeight = 88.f;
	CrouchingCapsuleHalfHeight = 44.f;
	BaseGroundFriction = 2.f;
	CrouchingGroundFriction = 100.f;


	///Character movement values
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	// Aiming movement values
	HipTurnRate = 90.f;
	HipLookupRate = 90.f;
	AimingTurnRate = 20.f;
	AimingLookupRate = 20.f;
	// Mouse look sensitivity scale factors
	MouseHipLookupRate = 1.0f;
	MouseHipTurnRate = 1.0;
	MouseAimingLookupRate = .6f;
	MouseAimingTurnRate = .6f;


	/* Field Of View*/
	//Gun Values FOV
	CameraDefaultFOV = 0.f; // Set in BeginPlay
	CameraZoomedFOV = 60.f;
	CameraCurrentFOV = 0.f;
	ZoomInterpSpeed = 20.f;
	// Crosshair spread factors
	CrosshairAimFactor = 0.f;
	CrosshairInAirFactor = 0.f;
	CrosshairShootingFactor = 0.f;
	CrosshairSpreadMultiplier = 0.f;
	CrosshairVelocityFactor = 0.f;

	//Gun Values
	StartingARAmmo = 30;
	Starting9mmAmmo = 25;
	Starting776Ammo = 30;
	StartingShellAmmo = 4;

	// Bullet fire Timer variables
	ShootTimeDuration = 0.05f;
	/*AutomaticFireRate = 0.1f;*/
	BulletSpread = 1.f;

	// Boolean
	bMovingForward = false;
	bMovingRight = false;
	bCanPickup = false;
	bAiming = false;
	bFiringBullet = false;
	bShouldFire = false;
	bFireButtonPress = false;
	bShouldTraceForItems = false;
	bCrouching = false;
	bAimingButtonPressed = false;


	//Pickup sound timer properties
	bShouldPlayPickupSound = true;
	bShouldPlayEquipSound = true;

	/*Camera Interping the location for item */
	CameraInterpDistance = 250.f;
	CameraInterpElevation = 65.f;

	// Create hand scene component
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	// Combat variables
	CombatState = ECombatState::ECS_Unoccupied;

	
	//bIsAccelerating = false;

	MovementStatus = EMovementStatus::EMS_Normal;

	//Sprinting
	RunningSpeed = 600.f;
	SprintingSpeed = 800.f;
	CrouchMovementSpeed = 80.f;

	bIsAccelerating = false;

	//Health
	//Health = 100.f;

	//HealAmount = 25.0f;
	//Stamina
	StaminaStatus = EStaminaStatus::ESS_Normal;

	MaxStamina = 100.f;
	Stamina = 100.f;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;


	// Create Interpolation Components
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponInterpolationComponents"));
	WeaponInterpComp->SetupAttachment(GetCameraComponent());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
	InterpComp1->SetupAttachment(GetCameraComponent());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
	InterpComp2->SetupAttachment(GetCameraComponent());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
	InterpComp3->SetupAttachment(GetCameraComponent());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
	InterpComp4->SetupAttachment(GetCameraComponent());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
	InterpComp5->SetupAttachment(GetCameraComponent());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
	InterpComp6->SetupAttachment(GetCameraComponent());

	PickupSoundResetTime = 0.2f;
	EquipSoundResetTime = 0.2f;

	HightlightedSlot = -1;

	// FNames
	RightHandSocketName = FName("");
	MuzzleSocketName = ("");
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (CameraComp) {
		CameraDefaultFOV = GetCameraComponent()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &AMainCharacter::OnHealthChanged);

	//Spawn the default weapon and attach it to the mesh
	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedGun);
	EquippedGun->SetSlotIndex(0);
	EquippedGun->SetCharacter(this);

	InitializeAmmoMap();

	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

	// Create FInterpLocation structs for each interp location. Add to array
	InitializeInterpLocations();

//	LoadGameNoSwitch();

}

bool AMainCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	float HalfRad = FMath::DegreesToRadians(BulletSpread);
	FVector ShotDirection = FMath::VRandCone(CrosshairWorldDirection, HalfRad, HalfRad);

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		ShotDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward
		const FVector Start{ CrosshairWorldPosition }; // Eye Location
		const FVector End{ Start + ShotDirection * 50000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}


float AMainCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle Interpolation for zoom when aiming 
	CameraInterpZoom(DeltaTime);

	//Change look Sensitivity based on aiming
	SetLookRates();

	//Calculate crosshair spread multiplier
	CalculatedCrosshairSpread(DeltaTime);

	/* check OverlappItemCount, Then trace for items*/
	TraceForItems();

	//Interpolate the capsule Half Height based on Crouching/Standing
	InterpCapsuleHalfHeight(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bIsAccelerating)
		{

				if (Stamina - DeltaStamina <= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else
				{
					Stamina -= DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
				/*if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}*/
			}
			else // Shift key up
			{
				if (Stamina + DeltaStamina >= MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bIsAccelerating)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
				/*if (bMovingForward || bMovingRight)

					{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}*/
			}
		}
		else // Shift key up
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bIsAccelerating)
		{
			Stamina = 0.f;
		}
		else // Shift key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
		
	}
}

void AMainCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<APickupItem>(ItemTraceResult.Actor);

			const auto TraceHitWeapon = Cast<AGuns>(TraceHitItem);

			if (TraceHitWeapon) 
			{
				if (HightlightedSlot == -1) {
					// Not currently hightlighting a lot, hightlight one
					HightlightInventorySlot();
				}
			}
			else {
				//Is a slot being Hightlighted?
				if (HightlightedSlot != 1) {
					// Unhight the slot
					UnHightlightInventorySlot();
				}
			}

			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping) {
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				// Show Item's Pickup Widget
				TraceHitItem->GetPickupWidget()->SetVisibility(true);

				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					// Inventory is full
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else {
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}

			// We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// We are hitting a different AItem this frame from last frame
					// Or AItem is null.
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}
			// Store a reference to HitItem for next frame
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		// No longer overlapping any items,
		// Item last frame should not show widget
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	//Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveBackward", this, &AMainCharacter::MoveBackward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveLeft", this, &AMainCharacter::MoveLeft);
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);


	//Actions
	//Jumping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Crouching
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::ToggleCrouch);

	//to fire the weapon
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AMainCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AMainCharacter::FireButtonReleased);
	
	///* To Throw */
	//PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AMainCharacter::PressEButton);
	//PlayerInputComponent->BindAction("Throw", IE_Released, this, &AMainCharacter::ReleaseEButton);

	/* SwapWeapon*/
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AMainCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Pickup", IE_Released, this, &AMainCharacter::SelectButtonReleased);


	/* To aim*/
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMainCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMainCharacter::AimingButtonReleased);

	/* To Reload*/
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMainCharacter::ReloadButtonPressed);

	
	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AMainCharacter::FKeyPressed);
	
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AMainCharacter::OneKeyPressed);
	
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AMainCharacter::TwoKeyPressed);
	
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AMainCharacter::ThreeKeyPressed);
	
	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &AMainCharacter::FourKeyPressed);
	
	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &AMainCharacter::FiveKeyPressed);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);

}

void AMainCharacter::OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied) {
		Health = 0.f;

		CurrentHealth = Health;

		bDied = true;

		Die();

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(2.f);

	}
	//else {
	////	Health -= DamageAmount;
	////}
}

void AMainCharacter::MoveForward(float Value)
{
	bMovingForward = false;
	if ((Controller != nullptr) && (Value != 0.0f) && (!bDied)) {

		//AddMovementInput(GetActorForwardVector() * Value);

		//finding out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveBackward(float Value)
{
	bMovingForward = false;
	if ((Controller != nullptr) && (Value != 0.0f) && (!bDied)) {

		//AddMovementInput(GetActorForwardVector() * Value);

		//finding out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	bMovingRight = false;

	if ((Controller != nullptr) && (Value != 0.f) && (!bDied)) {

//		AddMovementInput(GetActorRightVector() * Value);

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveLeft(float Value)
{
	bMovingRight = false;

	if ((Controller != nullptr) && (Value != 0.f) && (!bDied)) {

		// AddMovementInput(GetActorRightVector() * Value);

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::Turn(float Value)
{
	float TurnScaleFactor{};

	if (bAiming) {
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else {
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AMainCharacter::LookUp(float Value)
{

	float LookupScaleFactor{};

	if (bAiming) {
		LookupScaleFactor = MouseAimingLookupRate;
	}
	else {
		LookupScaleFactor = MouseHipLookupRate;
	}
	AddControllerPitchInput(Value * LookupScaleFactor);
}

void AMainCharacter::SetLookRates()
{

	if (bAiming) {
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookupRate;
	}
	else {
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookupRate;
	}
}

void AMainCharacter::Jump()
{


	if (bCrouching) {
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
	else {
		ACharacter::Jump();
	}
	
}

void AMainCharacter::ToggleCrouch()
{

	if (!GetCharacterMovement()->IsFalling()) {
		bCrouching = !bCrouching;
	}
	if (bIsAccelerating) {
		bCrouching = !bCrouching;
	}

	if (bCrouching) {

		bCrouching = true;
		MovementStatus = EMovementStatus::EMS_Crouching;
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		bCrouching = false;
		MovementStatus = EMovementStatus::EMS_Normal;
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

AGuns* AMainCharacter::SpawnDefaultWeapon()
{
	// Check the TSubclassOf variable
	if (DefaultWeaponClass)
	{
		// Spawn the Weapon
		return GetWorld()->SpawnActor<AGuns>(DefaultWeaponClass);
	}
	return nullptr;
}

void AMainCharacter::Fire()
{

	if (EquippedGun == nullptr)return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (WeaponHasAmmo()) 
	{
		PlayFireSound();
		SendBullet();

		//Subtracts 1 from the weapon's Ammo;
		EquippedGun->DecrementAmmo();

		StartFireTimer();

		//Start Bullet fire timer for crosshair
		StartCrosshairBulletFire();
	}
}

void AMainCharacter::FireButtonPressed()
{
	bShouldFire = true;
	if (bShouldFire) {
		bFireButtonPress = true;

		Fire();
	}
	else {
		bFireButtonPress = false;
	}

}

void AMainCharacter::FireButtonReleased()
{
	bFireButtonPress = false;
}

void AMainCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
	AmmoMap.Add(EAmmoType::EAT_776, Starting776Ammo);
	AmmoMap.Add(EAmmoType::EAT_Shells, StartingShellAmmo);
}

bool AMainCharacter::WeaponHasAmmo()
{

	 if(EquippedGun == nullptr) return false;

	 return EquippedGun->GetAmmo() > 0;
}

void AMainCharacter::StartFireTimer()
{
	if (EquippedGun == nullptr) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AMainCharacter::AutoFireReset, EquippedGun->GetAutoFireRate());
}

void AMainCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo()) {
		if (bFireButtonPress && EquippedGun->GetAmmo()>0) {
			Fire();
		}
	}
	else 
	{
		bFireButtonPress = false;
		//Reload Weapon
		ReloadWeapon();
	}
}

bool AMainCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;

	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		//Tentatice beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else // no crosshair trace hit
	{
		//outbeamlocation is the end location for the line trace
		
	}

	//perform a second trace, this time from the gun barrel

	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };

	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (!OutHitResult.bBlockingHit) // Object Between barrel and BeamEndPoint?
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}

	return true;
}

void AMainCharacter::PressEButton()
{
	GetEquippedGun()->ThrowWeapon();
	DropWeapon();
}

void AMainCharacter::ReleaseEButton()
{

}

FInterpLocation AMainCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}

	return FInterpLocation();
}
/**/
void AMainCharacter::EquipWeapon(AGuns* GunsToSet, bool bSwapping)
{
	if (GunsToSet) {

		// Get the Hand Socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(
			RightHandSocketName);

		if (HandSocket)
		{
			// Attach the Weapon to the hand socket RightHandSocket
			HandSocket->AttachActor(GunsToSet, GetMesh());
		}
		
		if(EquippedGun == nullptr)
		{
			//-1 == no EquippedGun yet. No need to reserve the icon animation
			EquipItemDelegate.Broadcast(-1, GunsToSet->GetSlotIndex());

		}
		else if (!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquippedGun->GetSlotIndex(), GunsToSet->GetSlotIndex());
		}
		// Set EquippedWeapon to the newly spawned Weapon
		EquippedGun = GunsToSet;
		EquippedGun->SetItemState(EItemState::EIS_Equipped);
		}
	
}

void AMainCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0) {
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else 
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}
/* pick up the item*/
void AMainCharacter::GetPickupItem(APickupItem* Item)
{

	Item->PlayEquipSound();

	auto Weapon = Cast<AGuns>(Item);
	if (Weapon) 
	{
		if (Inventory.Num() < INVENTORY_CAPACITY) {
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_Pickedup);
		}
		else {
			SwapWeapon(Weapon);
		}
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo) {
		PickupAmmo(Ammo);
	}

	auto KeyItems = Cast<AKeyItems>(Item);
	if (KeyItems) {
		PickupItems(KeyItems);
	}
}

void AMainCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;

	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AMainCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void AMainCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = true;

	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AMainCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void AMainCharacter::Die() 
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage) {
		AnimInstance->Montage_Play(DeathMontage);
	}
}

void AMainCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC) {
		DisableInput(PC);
	}

}

void AMainCharacter::ShiftKeyDown()
{
	bIsAccelerating = true;
	bCrouching = false;
}

void AMainCharacter::ShiftKeyUp()
{
	bIsAccelerating = false;
	
}

void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;

	if (Status == EMovementStatus::EMS_Sprinting) {
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}

	if (Status == EMovementStatus::EMS_Crouching) {
		bCrouching = true;
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
	}


}

void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World) {
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);

		if (CurrentLevelName != LevelName) {

			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMainCharacter::SaveGame(bool SetPosition)
{
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	SaveGameInstance->CharacterStates.Health = CurrentHealth;
	SaveGameInstance->CharacterStates.DefaultHealth = HealthComp->DefaultHealth;
	SaveGameInstance->CharacterStates.Keys = Keys;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveGameInstance->CharacterStates.LevelName = MapName;

	if (EquippedGun) {
		SaveGameInstance->CharacterStates.WeaponName = EquippedGun->GunName;
	}

	if (SetPosition) {
		SaveGameInstance->CharacterStates.Location = GetActorLocation();
		SaveGameInstance->CharacterStates.Rotation = GetActorRotation();
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);

}

void AMainCharacter::LoadGame(bool SetPosition)
{

	UMySaveGame* LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	CurrentHealth = LoadGameInstance->CharacterStates.Health;
	HealthComp->DefaultHealth = LoadGameInstance->CharacterStates.DefaultHealth;
	Keys = LoadGameInstance->CharacterStates.Keys;

	if (WeaponStorage) {
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons) {

			FString WeaponName = LoadGameInstance->CharacterStates.WeaponName;
			if (Weapons->WeaponMap.Contains(WeaponName)) {
				AGuns* WeaponToEquip = GetWorld()->SpawnActor<AGuns>(Weapons->WeaponMap[WeaponName]);
				EquipWeapon(EquippedGun, true);

			}
		}
	}

	if (LoadGameInstance->CharacterStates.LevelName != TEXT("")) {

		FName LevelName(*LoadGameInstance->CharacterStates.LevelName);

		SwitchLevel(LevelName);
	}

	if (SetPosition) {
		SetActorLocation(LoadGameInstance->CharacterStates.Location);
		SetActorRotation(LoadGameInstance->CharacterStates.Rotation);
		UE_LOG(LogTemp, Warning, TEXT("Loading Location"));

	}

	/*bDied = false;*/
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}

void AMainCharacter::LoadGameNoSwitch()
{
	UMySaveGame* LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	CurrentHealth = LoadGameInstance->CharacterStates.Health;
	HealthComp->DefaultHealth = LoadGameInstance->CharacterStates.DefaultHealth;
	Keys = LoadGameInstance->CharacterStates.Keys;

	if (WeaponStorage) {
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons) {

			FString WeaponName = LoadGameInstance->CharacterStates.WeaponName;
			if (Weapons->WeaponMap.Contains(WeaponName)) {
				AGuns* WeaponToEquip = GetWorld()->SpawnActor<AGuns>(Weapons->WeaponMap[WeaponName]);
				EquipWeapon(EquippedGun, true);

			}
		}
	}

	/*bDied = false;*/
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}

//float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//
//	return DamageAmount;
//}

void AMainCharacter::IncrementKeys(int32 Amount)
{
	Keys += Amount;
}

//void AMainCharacter::IncrementHealth(float Amount)
//{
//	if (Health + Amount >= DefaultHealth) {
//		Health = DefaultHealth;
//
//	}
//	else {
//		Health += Amount;
//	}
//
//	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);
//
//}

void AMainCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied)return;

	if (TraceHitItem) {
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
}

void AMainCharacter::SelectButtonReleased()
{
	
}

void AMainCharacter::DropWeapon()
{
	if (EquippedGun) {
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);

		EquippedGun->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedGun->SetItemState(EItemState::EIS_Falling);
		EquippedGun->ThrowWeapon();
	}
}

void AMainCharacter::SwapWeapon(AGuns* WeaponToSwap)
{

	if (Inventory.Num() - 1 >= EquippedGun->GetSlotIndex()) 
	{
		Inventory[EquippedGun->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedGun->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

FVector AMainCharacter::GetPawnViewLocation() const
{
	if (CameraComp) {
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void AMainCharacter::StartMontageAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	switch (WeaponType) {

	case EWeaponType::EWT_PlayerAR:
		if (AnimInstance && HipRifleFireMontage) {
			bFireButtonPress = true;
			AnimInstance->Montage_Play(HipRifleFireMontage);
			AnimInstance->Montage_JumpToSection(FName("Attack01"));
		}
		else if (bAiming) {
			bFireButtonPress = true;
			AnimInstance->Montage_Play(AimRifleFireMontage);
			AnimInstance->Montage_JumpToSection(FName("Attack03"));
		}
		break;
	case EWeaponType::EWT_Pistol:
		if (AnimInstance && PistolFireMontage) {
			bFireButtonPress = true;
			AnimInstance->Montage_Play(PistolFireMontage);
			AnimInstance->Montage_JumpToSection(FName("Attack01"));
		}
		break;

	}

}

void AMainCharacter::AimingButtonPressed()
{


	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading) {
		Aim();
	}
}

void AMainCharacter::Aim()
{
	bAimingButtonPressed = true;
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AMainCharacter::AimingButtonReleased()
{


	bAimingButtonPressed = false;
	StopAiming();
}

void AMainCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching) {
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::CameraInterpZoom(float DeltaTime)
{
	// Set Current camera field of view
	if (bAiming) {
		// Interpolate to Zoomed FOV
		CameraCurrentFOV = FMath::FInterpConstantTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);

	}
	else {
		// Interpolate to Default FOV
		CameraCurrentFOV = FMath::FInterpConstantTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetCameraComponent()->SetFieldOfView(CameraCurrentFOV);
}

void AMainCharacter::CalculatedCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	// Calculate crosshair velocity Factor
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// Calculate Crosshair in air Factor
	if (GetCharacterMovement()->IsFalling()) // is in air
	{
		// Spread the crosshairs slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else {
		//shrink the crosshairs fast while falling
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	//calculate the crosshair aim factor
	if (bAiming) // are we aiming?
	{
		// Shrink Crosshairs a small amount very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.9f, DeltaTime, 30.f);
	}
	else // not aiming
	{
		// Shrink Crosshairs back to normal very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	// True 0.05 second after firing
	if (bFiringBullet) {
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else {
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;

}

void AMainCharacter::StartCrosshairBulletFire()
{
	if (EquippedGun) {

		bFiringBullet = true;

		GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AMainCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
	}
}

void AMainCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AMainCharacter::PlayFireSound()
{
	// Play fire sound
	if (EquippedGun->GetFireSound()) {
		UGameplayStatics::PlaySound2D(this, EquippedGun->GetFireSound());
	}
}

void AMainCharacter::SendBullet()
{
	// Send Bullet
	const USkeletalMeshSocket* BarrelSocket = EquippedGun->GetItemMesh()->GetSocketByName(MuzzleSocketName);

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(
			EquippedGun->GetItemMesh());

		if (EquippedGun->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedGun->GetMuzzleFlash(), SocketTransform);

			//Camera Shake
			APawn* MyOwner = Cast<APawn>(GetOwner());
			if (MyOwner)
			{
				APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
				if (PC)
				{
					PC->ClientStartCameraShake(EquippedGun->FireCamShake);
				}
			}
		}
		//
		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);
		if (bBeamEnd)
		{
			// Does hit actor implement BulletHitInterface
			if (BeamHitResult.Actor.IsValid()) {

				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());

				if (BulletHitInterface) {
					BulletHitInterface->BulletHit_Implementation(BeamHitResult, this, GetController());
				}

				AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(BeamHitResult.Actor.Get());
				//AEnemyMeleeCharacter* HitMeleeEnemy = Cast<AEnemyMeleeCharacter>(BeamHitResult.Actor.Get());

				if (HitEnemy)
				{
					int32 Damage{};
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone()) {

						// HeadShot

						Damage = EquippedGun->GetHeadShotDamage();

						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());

						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, true);

					}
					else
					{
						Damage = EquippedGun->GetDamage();

						//BodyShot 
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());

						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, false);
					}
				}
				//if (HitMeleeEnemy)
				//{
				//	int32 Damage{};
				//	if (BeamHitResult.BoneName.ToString() == HitMeleeEnemy->GetHeadBone()) {

				//		// HeadShot

				//		Damage = EquippedGun->GetHeadShotDamage();

				//		UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());

				//		HitMeleeEnemy->ShowHitNumber(Damage, BeamHitResult.Location, true);
				//	}
				//	else
				//	{
				//		Damage = EquippedGun->GetDamage();

				//		//BodyShot 
				//		UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());

				//		HitMeleeEnemy->ShowHitNumber(Damage, BeamHitResult.Location, false);
				//	}

				//}
			}
			else {
				// Spawn Default Particles
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamHitResult.Location);
				}
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
			}
		}
	}
}

void AMainCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AMainCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied)return;


	if (EquippedGun == nullptr) return;

	//do we have ammo of the correct type
	if (CarryingAmmo() && !EquippedGun->ClipIsFull())
	{
		if (bAiming) {
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && ReloadMontage) {

			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedGun->GetReloadMontageSection());
		}
	}
}

void AMainCharacter::FinishReloading()
{
	//Update the combat state
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed) {
		Aim();
	}

	if (EquippedGun == nullptr) return;

	const auto AmmoType{ EquippedGun->GetAmmoType() };

	//Update the AmmoMap
	if (AmmoMap.Contains(AmmoType)) 
	{
		//Amount of ammo the Character is carrying of the EquippedGun type
		int32 CarriedAmmo = AmmoMap[AmmoType];

		//Space left in the magazine
		const int32 MagEmptySpace = EquippedGun->GetMagazineCapacity() - EquippedGun->GetAmmo();
	
		if (MagEmptySpace > CarriedAmmo)
		{
			//Reload the magazin with all the ammo we are carrying
			EquippedGun->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;

			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else {

			// Fill the magazine
			EquippedGun->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}


}

void AMainCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

bool AMainCharacter::CarryingAmmo()
{
	if (EquippedGun == nullptr)return false;

	auto AmmoType = EquippedGun->GetAmmoType();

	if (AmmoMap.Contains(AmmoType)) {
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AMainCharacter::GrabClip()
{
	if (EquippedGun == nullptr) { return; }
	if (HandSceneComponent == nullptr) { return; }

	// Index for the clip bone on the EquippedGun
	int32 ClipBoneIndex{ EquippedGun->GetItemMesh()->GetBoneIndex(EquippedGun->GetClipBoneName()) };
	
	//Store the transform of the clip
	ClipTransform = EquippedGun->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);

	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedGun->SetMovingClip(true);

}

void AMainCharacter::ReleaseClip()
{

	EquippedGun->SetMovingClip(false);

}

void AMainCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};

	if (bCrouching) {
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else {
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f) };

	// Negative value if crouching, positive value if standing
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };

	const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };

	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AMainCharacter::PickupAmmo(AAmmo* Ammo)
{
	// check to see if AmmoMap contains Ammo's AmmoType
	if (AmmoMap.Find(Ammo->GetAmmoType())) 
	{
		// Get amount of ammo in our AmmoMap for Ammo's type
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()]};
		AmmoCount += Ammo->GetItemCount();

		// Set the amount of ammo in the map for this type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedGun->GetAmmoType() == Ammo->GetAmmoType()) 
	{
		// Check to see if the gun is empty
		if (EquippedGun->GetAmmo() == 0) { ReloadWeapon(); }
	}
	Ammo->Destroy();
}

void AMainCharacter::PickupItems(AKeyItems* KeyItems)
{
	KeyItems->Destroy();
}

void AMainCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{ WeaponInterpComp, 0 };
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ InterpComp1, 0 };
	InterpLocations.Add(InterpLoc1);
	FInterpLocation InterpLoc2{ InterpComp2, 0 };
	InterpLocations.Add(InterpLoc2);
	FInterpLocation InterpLoc3{ InterpComp3, 0 };
	InterpLocations.Add(InterpLoc3);
	FInterpLocation InterpLoc4{ InterpComp4, 0 };
	InterpLocations.Add(InterpLoc4);
	FInterpLocation InterpLoc5{ InterpComp5, 0 };
	InterpLocations.Add(InterpLoc5);
	FInterpLocation InterpLoc6{ InterpComp6, 0 };
	InterpLocations.Add(InterpLoc6);
}

void AMainCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AMainCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

void AMainCharacter::FKeyPressed()
{
	if (EquippedGun->GetSlotIndex() == 0) return;

	ExchangeInventoryItems(EquippedGun->GetSlotIndex(), 0);
}

void AMainCharacter::OneKeyPressed()
{
	if (EquippedGun->GetSlotIndex() == 1)return;

	ExchangeInventoryItems(EquippedGun->GetSlotIndex(), 1);
}

void AMainCharacter::TwoKeyPressed()
{
	if (EquippedGun->GetSlotIndex() == 2)return;

	ExchangeInventoryItems(EquippedGun->GetSlotIndex(), 2);
}

void AMainCharacter::ThreeKeyPressed()
{
	if (EquippedGun->GetSlotIndex() == 3)return;

	ExchangeInventoryItems(EquippedGun->GetSlotIndex(), 3);
}

void AMainCharacter::FourKeyPressed()
{
	if (EquippedGun->GetSlotIndex() == 4)return;

	ExchangeInventoryItems(EquippedGun->GetSlotIndex(), 4);
}

void AMainCharacter::FiveKeyPressed()
{
	if (EquippedGun->GetSlotIndex() == 5)return;

	ExchangeInventoryItems(EquippedGun->GetSlotIndex(), 5);
}

void AMainCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	const bool bCanExchangeItems = (CurrentItemIndex != NewItemIndex) && (NewItemIndex < Inventory.Num()) && (CombatState == ECombatState::ECS_Unoccupied || CombatState == ECombatState::ECS_Equipping);

	if (bCanExchangeItems)
	{

		auto OldEquippedWeapon = EquippedGun;
		auto NewWeapon = Cast<AGuns>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon);

		OldEquippedWeapon->SetItemState(EItemState::EIS_Pickedup);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);

		CombatState = ECombatState::ECS_Equipping;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && EquipMontage) {

			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}

		NewWeapon->PlayEquipSound(true);

	}
}

int32 AMainCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++) {
		if (Inventory[i] == nullptr) {
			return i;
		}
	}

	if (Inventory.Num()<INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}

	return -1;//inventory is full
}

void AMainCharacter::HightlightInventorySlot()
{
	const int32 EmptySlot{ GetEmptyInventorySlot() };

	HightlightIconDelegate.Broadcast(EmptySlot, true);
	HightlightedSlot = EmptySlot;
}

EPhysicalSurface AMainCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f,0.f,-400.f) };
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;


	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void AMainCharacter::UnHightlightInventorySlot()
{
	HightlightIconDelegate.Broadcast(HightlightedSlot, false);
	HightlightedSlot = -1;
}

int32 AMainCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AMainCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index) {

		InterpLocations[Index].ItemCount += Amount;
	}
}

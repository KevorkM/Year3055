// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "EnemyShooterCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API AEnemyShooterCharacter : public AEnemyCharacter
{
	GENERATED_BODY()


		AEnemyShooterCharacter();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class APickupItem* Item;

protected:

	/* Sound to play when shooting*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class USoundCue* OnShootingSound;

	// Weapon attachment
/* related to Attaching the weapon to player. */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
		TSubclassOf<class AGuns>StarterWeaponClass;

	/* Montage containing variety kinds of attack montages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		UAnimMontage* CombatGunMontage;

	/* Shooting type*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		AGuns* CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		FName WeaponAttachmentSocketName;

	//Shooting Timer
	FTimerHandle TimerHandle_TimeBetweenShoots;

	float LastFireTime;

	//RPM - Bullets per minute fired from the gun
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float RateOfFire;

	//Derived from RateOfFire
	float TimeBetweenShots;

	/* Effects when shooting*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		FName MuzzleSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		FName TracerTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* BeamParticles;

	/* When the enemy aims accuratly to get better shots */
	//Zoom
	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
		float ZoomInterpSpeed;

	// Default FOV set during begin play
	float DefaultFOV;

public:

	FORCEINLINE AGuns* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

	/*Shooting effects */
	void PlayFireEffects(FVector TraceEnd);

	//to shoot
	UFUNCTION(BlueprintCallable, Category = "Enemy")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
		void StopFire();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	/* Line trace for items under crosshairs*/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	//To Aim
	UFUNCTION(BlueprintCallable, Category = "Player")
		void BeginZoom();
	UFUNCTION(BlueprintCallable, Category = "Player")
		void EndZoom();

	bool CanShoot();

	UFUNCTION(BlueprintPure)
		FName GetAttackSectionName();

};

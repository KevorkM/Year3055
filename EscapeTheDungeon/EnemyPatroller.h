// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "EnemyPatroller.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API AEnemyPatroller : public AEnemyCharacter
{
	GENERATED_BODY()

		AEnemyPatroller();

//Variables
public:

	/* Sound to play when shooting*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class USoundCue* OnShootingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		FName WeaponAttachmentSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class APickupItem* Item;

protected:

	UFUNCTION(BlueprintPure)
		FName GetAttackSectionName();

	//UFUNCTION(BlueprintCallable)
	//	void PlayAttackMontage(FName Section, float PlayRate);

	/* Types of attacks*/
	FName Attack01;

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

//Functions
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	//to shoot
	UFUNCTION(BlueprintCallable, Category = "Enemy")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
		void StopFire();

	bool CanShoot();

protected:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

	/*Shooting effects */
	void PlayFireEffects(FVector TraceEnd);

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	/* Line trace for items under crosshairs*/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);
};

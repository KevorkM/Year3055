// This game is fully owned by KMG


#include "EnemyPatroller.h"
#include "AIEnemyController.h"
#include "EscapeTheDungeon.h"
#include "KeyItems.h"
#include "Ammo.h"

#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystemComponent.h>
#include "BehaviorTree/BlackboardComponent.h"

AEnemyPatroller::AEnemyPatroller() {

	///Weapon
//Attachment
	WeaponAttachmentSocketName = "BarrelSocket";
	TracerTargetName = "Target";

	RateOfFire = 100.f;

	BulletSpread = 2.0f;

	// Attacks
	Attack01 = (TEXT("Attack01"));

}

FName AEnemyPatroller::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(0,1) };

	switch (Section) {

	case 1:

		SectionName = Attack01;
		StartFire();

		break;

	}

	return SectionName;
}

void AEnemyPatroller::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 10 / RateOfFire;
}

void AEnemyPatroller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnemyPatroller::Fire()
{
	//Trace the world, from the eyes of the pawn to crosshairs location
	AActor* MyOwner = GetOwner();

	if (MyOwner) {
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(Item);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		//Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, Collision_Weapon, QueryParams)) {
			//Blocking hit! Process damage
			AActor* HitActor = Hit.GetActor();

			if (HitActor == nullptr) return;
			//damage
			/* MyOwner*/
			UGameplayStatics::ApplyPointDamage(HitActor, BaseDamage, ShotDirection, Hit, EnemyController, this, UDamageType::StaticClass());

			UE_LOG(LogTemp, Warning, TEXT("Enemy is firing"));

			TracerEndPoint = Hit.ImpactPoint;

			PlayFireEffects(TracerEndPoint);

			if (OnShootingSound) UGameplayStatics::PlaySound2D(this, OnShootingSound);
			LastFireTime = GetWorld()->TimeSeconds;
		}
	}
}

void AEnemyPatroller::StartFire()
{
	if (!bDied && bCanBeSeen) {
		bCanAttack = true;
		bCanBeSeen = true;

		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShoots, this, &AEnemyPatroller::Fire, TimeBetweenShots, true, FirstDelay);

		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
		}

		UE_LOG(LogTemp, Warning, TEXT("Enemy is Shooting"));
	}
}

void AEnemyPatroller::StopFire()
{
	//	&& !bDied
	if (bDied && !bCanBeSeen)
	{
		bCanAttack = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShoots);

		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
		}

		UE_LOG(LogTemp, Warning, TEXT("Enemy is not firing"));
	}
}

bool AEnemyPatroller::CanShoot()
{
	return false;
}

void AEnemyPatroller::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GetMesh(), MuzzleSocketName);
	}
	if (BeamParticles) {
		FVector MuzzleLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
}

bool AEnemyPatroller::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
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

bool AEnemyPatroller::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
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

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50000.f };
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

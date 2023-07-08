// This game is fully owned by KMG


#include "EnemyShooterCharacter.h"
#include "Guns.h"
#include "AIEnemyController.h"
#include "EscapeTheDungeon.h"
#include "KeyItems.h"
#include "Ammo.h"

#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystemComponent.h>
#include "BehaviorTree/BlackboardComponent.h"

AEnemyShooterCharacter::AEnemyShooterCharacter() {

	///Weapon
	//Attachment
	WeaponAttachmentSocketName = "RightHandSocket";
	TracerTargetName = "Target";

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	RateOfFire = 100.f;

	BulletSpread = 2.0f;

	Attack01 = (TEXT("Attack1"));

}

void AEnemyShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Spawning the weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<AGuns>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon) {
		CurrentWeapon->SetOwner(this);//Since we set the fire function in AGuns to GetOwner
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttachmentSocketName);
	}
	/* to Aim*/
	DefaultFOV = CameraComp->FieldOfView;

	TimeBetweenShots = 10 / RateOfFire;
}

void AEnemyShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* To Aim*/
	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

void AEnemyShooterCharacter::Fire()
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

void AEnemyShooterCharacter::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, CurrentWeapon->GetItemMesh(), MuzzleSocketName);
	}
	if (BeamParticles) {
		FVector MuzzleLocation = CurrentWeapon->GetItemMesh()->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
}

void AEnemyShooterCharacter::StartFire()
{

	if (CurrentWeapon && !bDied && bCanBeSeen) {
		bCanAttack = true;
		bCanBeSeen = true;

		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShoots, this, &AEnemyShooterCharacter::Fire, TimeBetweenShots, true, FirstDelay);

		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
		}

		UE_LOG(LogTemp, Warning, TEXT("Enemy is Shooting"));
	}
}

void AEnemyShooterCharacter::StopFire()
{

	//	&& !bDied
	if (CurrentWeapon && bDied && !bCanBeSeen)
	{
		bCanAttack = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShoots);

		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
		}

		UE_LOG(LogTemp, Warning, TEXT("Enemy is not firing"));
	}
}

bool AEnemyShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
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

bool AEnemyShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
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

void AEnemyShooterCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void AEnemyShooterCharacter::EndZoom()
{
	bWantsToZoom = false;
}

bool AEnemyShooterCharacter::CanShoot()
{
	return false;
}

FName AEnemyShooterCharacter::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(0,1) };

	switch (Section) {

	case 1:

		SectionName = Attack01;

		break;

	}
	return SectionName;
}

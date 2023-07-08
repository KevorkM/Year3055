// This game is fully owned by KMG


#include "SpecialBots.h"
#include "MainCharacter.h"
#include "EnemyCharacter.h"
#include "AIEnemyController.h"
#include "HealthComponent.h"

#include <Components/SphereComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystemComponent.h>
#include <Blueprint/UserWidget.h>
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Perception/PawnSensingComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/CapsuleComponent.h>
#include "GameFramework/Character.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include <Engine/SkeletalMeshSocket.h>
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
ASpecialBots::ASpecialBots()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASpecialBots::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;

	ExplosionDamage = 60;
	ExplosionRadius = 350;

	SelfDamageInterval = 0.25f;

//	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	//Health = 100.f;

	HealthBarDisplayTime = 4.f;

	bCanReact = true;

	HitReactTimeMin = .5f;

	HitReactTimeMax = 3.f;

	HitNumberDestroyTime = 1.5f;

	bStunned = false;

	StunChance = .25f;

	BaseDamage = 20.f;

	DeathTime = 4.0;


}

// Called when the game starts or when spawned
void ASpecialBots::BeginPlay()
{
	Super::BeginPlay();
	
	//PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnPawnSeen);

	// Get the AI controller
	EnemyController = Cast<AAIEnemyController>(GetController());

	//HealthComp->OnHealthChanged.AddDynamic(this, &ASpecialBots::OnHealthChanged);

	////Patrol Points
	//const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	//const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	/* TODO*/
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsVector(FName("TargetDestination"), FVector(0, 0, 0));
	}

	//if (EnemyController) {
	//	EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	//}

	//if (EnemyController) {
	//	EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);

	//	EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);


	//	EnemyController->RunBehaviorTree(BehaviorTree);
	//}

	if (HasAuthority())
	{
		// Find initial move-to
		NextPathPoint = GetNextPathPoint();

		// Every second we update our power-level based on nearby bots (CHALLENGE CODE)
		FTimerHandle TimerHandle_CheckPowerLevel;
		GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &ASpecialBots::OnCheckNearbyBots, 1.0f, true);
	}

}

// Called every frame
void ASpecialBots::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (/*HasAuthority() &&*/ !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			// Keep moving towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		}
	}


	UpdateHitNumber();
}

void ASpecialBots::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ImpactParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

}

//bool ASpecialBots::IsFriendly(AActor* ActorA, AActor* ActorB)
//{
//	if (ActorA == nullptr || ActorB == nullptr)
//	{
//		// Assume Friendly
//		return true;
//	}
//
//	AMainCharacter* HealthCompA = Cast<AMainCharacter>(ActorA->GetComponentByClass(AMainCharacter::StaticClass()));
//	AEnemyCharacter* HealthCompB = Cast<AEnemyCharacter>(ActorB->GetComponentByClass(AEnemyCharacter::StaticClass()));
//
//	if (HealthCompA == nullptr || HealthCompB == nullptr)
//	{
//		// Assume friendly
//		return true;
//	}
//
//	return HealthCompA->TeamNum == HealthCompB->TeamNum;
//}

//float ASpecialBots::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//	if (Health - DamageAmount <= 0.f) {
//		Health = 0.f;
//
//		bDied = true;
//
//		Die();
//
//		GetMovementComponent()->StopMovementImmediately();
//		/*GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
//
//		DetachFromControllerPendingDestroy();
//
//		//CurrentWeapon->Destroy();
//
//		//StopFire();
//		SetLifeSpan(1.5f);
//
//	}
//	else {
//		Health -= DamageAmount;
//	}
//
//	ShowHealthBar();
//
//	////Determinal whether bullet hit stuns.
//	//const float Stunned = FMath::FRandRange(0.f, 1.f);
//	//if (Stunned <= StunChance) {
//
//	//	//Stun the enemy
//	//	PlayHitMontage(FName("HitFront"));
//	//	SetStunned(true);
//	//}
//	return DamageAmount;
//}

void ASpecialBots::HandleTakeDamage(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	// Explode on hitpoints == 0
	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

void ASpecialBots::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	// Increase damage based on the power level (challenge code)
	float ActualDamage = ExplosionDamage + (ExplosionDamage * PowerLevel);

	// Apply Damage!
	UGameplayStatics::ApplyRadialDamage(this, ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	SetLifeSpan(2.0f);

	/*if (HasAuthority())
	{

	}*/
}


void ASpecialBots::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ASpecialBots::HideHealthBar, HealthBarDisplayTime);
}

void ASpecialBots::Die()
{
	if (bDied) return;

	bDied = true;

	HideHealthBar();

	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	//if (AnimInstance) {
	//	AnimInstance->Montage_Play(DeathMontage);
	//}

	//if (EnemyController) {
	//	EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	//}

	EnemyController->StopMovement();
}

void ASpecialBots::ResetHitReactTimer()
{
	bCanReact = true;
}

void ASpecialBots::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;

	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDestroyTime, false);
}

void ASpecialBots::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void ASpecialBots::UpdateHitNumber()
{
	for (auto& HitPair : HitNumbers) {

		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };

		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);

		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

void ASpecialBots::FinishDeath()
{
	//GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &ASpecialBots::DestroyEnemy, DeathTime);
}

void ASpecialBots::DestroyEnemy()
{
	Destroy();
}

FVector ASpecialBots::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;
		if (TestPawn == nullptr || UHealthComponent::IsFriendly(TestPawn, this))
		{
			continue;
		}

		UHealthComponent* TestPawnHealthComp = Cast<UHealthComponent>(TestPawn->GetComponentByClass(UHealthComponent::StaticClass()));
		if (TestPawnHealthComp && TestPawnHealthComp->Health > 0.0f)
		{
			float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();

			if (Distance < NearestTargetDistance)
			{
				BestTarget = TestPawn;
				NearestTargetDistance = Distance;
			}
		}
	}

	if (BestTarget)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ASpecialBots::RefreshPath, 5.0f, false);

		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			// Return next point in the path
			return NavPath->PathPoints[1];
		}
	}

	// Failed to find path
	return GetActorLocation();
}

void ASpecialBots::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASpecialBots::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedSelfDestruction && !bExploded)
	{
		AMainCharacter* PlayerPawn = Cast<AMainCharacter>(OtherActor);
		if (PlayerPawn && !UHealthComponent::IsFriendly(OtherActor, this))
		{
			// We overlapped with a player!

			/*if (HasAuthority())
			{

			}*/

			// Start self destruction sequence
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASpecialBots::DamageSelf, SelfDamageInterval, true, 0.0f);

			bStartedSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}

void ASpecialBots::OnCheckNearbyBots()
{
	// distance to check for nearby bots
	const float Radius = 600;

	// Create temporary collision shape for overlaps
	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);

	// Only find Pawns (eg. players and AI bots)
	FCollisionObjectQueryParams QueryParams;
	// Our tracker bot's mesh component is set to Physics Body in Blueprint (default profile of physics simulated actors)
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	int32 NrOfBots = 0;
	// loop over the results using a "range based for loop"
	for (FOverlapResult Result : Overlaps)
	{
		// Check if we overlapped with another tracker bot (ignoring players and other bot types)
		ASpecialBots* Bot = Cast<ASpecialBots>(Result.GetActor());
		// Ignore this trackerbot instance
		if (Bot && Bot != this)
		{
			NrOfBots++;
		}
	}

	const int32 MaxPowerLevel = 4;

	// Clamp between min=0 and max=4
	PowerLevel = FMath::Clamp(NrOfBots, 0, MaxPowerLevel);

	// Update the material color
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		// Convert to a float between 0 and 1 just like an 'Alpha' value of a texture. Now the material can be set up without having to know the max power level 
		// which can be tweaked many times by gameplay decisions (would mean we need to keep 2 places up to date)
		float Alpha = PowerLevel / (float)MaxPowerLevel;
		// Note: (float)MaxPowerLevel converts the int32 to a float, 
		//	otherwise the following happens when dealing when dividing integers: 1 / 4 = 0 ('PowerLevel' int / 'MaxPowerLevel' int = 0 int)
		//	this is a common programming problem and can be fixed by 'casting' the int (MaxPowerLevel) to a float before dividing.

		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}
}

void ASpecialBots::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

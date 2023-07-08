// This game is fully owned by KMG


#include "EnemyCharacter.h"
#include "AIEnemyController.h"
#include "EnemyController.h"
#include "Guns.h"
#include "MainCharacter.h"
#include "HealthComponent.h"
#include "EscapeTheDungeon.h"

#include <Components/SphereComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystemComponent.h>
#include <Blueprint/UserWidget.h>
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Engine/SkeletalMeshSocket.h>
#include <Components/CapsuleComponent.h>
#include "PhysicalMaterials/PhysicalMaterial.h"
#include <Perception/PawnSensingComponent.h>


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	///Spring Arm
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 0.f;
	SpringArmComp->SetupAttachment(RootComponent);

	///Camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	/* Attach the camera to the end of SprinArmComp and let it adjust
	to match the controller orientation*/
	CameraComp->bUsePawnControlRotation = false;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	//allowing to crouch
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	bShiftKeyDown = false;
	bCTRLKeyDown = false;

	EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
	GuardState = EEnemyAIState::EES_Idle;

	WalkingSpeed = 300.f;
	RunningSpeed = 650.f;

	//Health = 100.f;

	bMovingForward = false;
	bMovingRight = false;

	bLMBDown = false;

	HealthBarDisplayTime = 4.f;

	bCanReact = true;

	HitReactTimeMin = .5f;

	HitReactTimeMax = 3.f;

	HitNumberDestroyTime = 2.f;

	bStunned = false;

	StunChance = .25f;

	BaseDamage = 20.f;

	DeathTime = 4.0;

	bCanBeSeen = false;

	bCanAttack = false;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnPawnSeen);

	HealthComp->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::OnHealthChanged);

	// Get the AI controller
	EnemyController = Cast<AAIEnemyController>(GetController());

	//Patrol Points
	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	/* TODO*/
	if (EnemyController) {

		EnemyController->GetBlackboardComponent()->SetValueAsVector(FName("TargetDestination"), FVector(0,0,0));
	}

	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
	}

	if (EnemyController) {

		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);

		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);

		EnemyController->RunBehaviorTree(BehaviorTree);

	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHitNumber();
}

FVector AEnemyCharacter::GetPawnViewLocation() const
{
	if (CameraComp) {
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void AEnemyCharacter::ResetHitReactTimer()
{
	bCanReact = true;
}

void AEnemyCharacter::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;

	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDestroyTime, false);

}

void AEnemyCharacter::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();

}

void AEnemyCharacter::UpdateHitNumber() {

	for (auto& HitPair : HitNumbers) {

		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };

		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);

		HitNumber->SetPositionInViewport(ScreenPosition);
	}

}

void AEnemyCharacter::SetStunned(bool Stunned)
{
	bStunned = Stunned;

	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}

}

/*TODO*/
void AEnemyCharacter::SpawnBlood(AMainCharacter* Victim, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };

	if (TipSocket) {

		const FTransform SocketTransform{ TipSocket->GetSocketTransform(GetMesh()) };

		if (Victim->GetBloodParticles()) {

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Victim->GetBloodParticles(), SocketTransform);
		}
	}
}


void AEnemyCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemyCharacter::DestroyEnemy, DeathTime);

}

void AEnemyCharacter::DestroyEnemy()
{
	Destroy();
}

void AEnemyCharacter::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr) { return; }

	//if (OtherActor == nullptr) return;
	auto Character = Cast<APawn>(SeenPawn);

	bCanBeSeen = true;

	// Set the Value of the target Blackboard Key
	if (SeenPawn /*&& bCanBeSeen*/ ) {

		if (EnemyController) {
			if (EnemyController->GetBlackboardComponent()) {

				EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), SeenPawn);
				EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
			}
		}
	}
	else if (!bCanBeSeen){
		bCanBeSeen = false;
		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
		}
	}

}

void AEnemyCharacter::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}
}

EPhysicalSurface AEnemyCharacter::GetEnemySurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f,0.f,-400.f) };
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;


	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ImpactParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

}

void AEnemyCharacter::OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.f && !bDied) {
		Health = 0.f;

		bDied = true;

		Die();

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		//CurrentWeapon->Destroy();

		//StopFire();
		SetLifeSpan(1.5f);

	}

	ShowHealthBar();

	//Determinal whether bullet hit stuns.
	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance) {

		//Stun the enemy
		PlayHitMontage(FName("HitFront"));
		SetStunned(true);
	}
}

//float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//	//if (Health - DamageAmount <= 0.f) {
//	//	Health = 0.f;
//
//	//	bDied = true;
//
//	//	Die();
//
//	//	GetMovementComponent()->StopMovementImmediately();
//	//	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//	//	DetachFromControllerPendingDestroy();
//
//	//	//CurrentWeapon->Destroy();
//
//	//	//StopFire();
//	//	SetLifeSpan(1.5f);
//
//	//}
//	//else {
//	//	Health -= DamageAmount;
//	//}
//
//	//ShowHealthBar();
//
//	////Determinal whether bullet hit stuns.
//	//const float Stunned = FMath::FRandRange(0.f, 1.f);
//	//if (Stunned <= StunChance) {
//
//	//	//Stun the enemy
//	//	PlayHitMontage(FName("HitFront"));
//	//	SetStunned(true);
//	//}
//	//return DamageAmount;
//}

void AEnemyCharacter::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemyCharacter::HideHealthBar, HealthBarDisplayTime);
}

void AEnemyCharacter::Die()
{
	if (bDied) return;

	bDied = true;

	HideHealthBar();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance) {
		AnimInstance->Montage_Play(DeathMontage);
	}

	//if (EnemyController) {
	//	EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	//}

	EnemyController->StopMovement();
}

void AEnemyCharacter::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanReact) {

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance) {
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}
	}

	bCanReact = false;

	const float HitReactTime{ FMath::FRandRange(HitReactTimeMin, HitReactTimeMax) };

	GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemyCharacter::ResetHitReactTimer, HitReactTime);


}

void AEnemyCharacter::MoveForward(float Value)
{
	bMovingForward = false;
	if ((EnemyController != nullptr) && (Value != 0.0f) && (!bDied)) {
		AddMovementInput(GetActorForwardVector() * Value);
		bMovingForward = true;
	}
}

void AEnemyCharacter::MoveRight(float Value)
{
	bMovingRight = false;

	if ((EnemyController != nullptr) && (Value != 0.f) && (!bDied)) {
		AddMovementInput(GetActorRightVector() * Value);
		bMovingRight = true;
	}
}

void AEnemyCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AEnemyCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AEnemyCharacter::Jump()
{
	Super::Jump();
}

void AEnemyCharacter::BeginCrouch()
{
	Crouch();
}

void AEnemyCharacter::EndCrouch()
{
	UnCrouch();
}

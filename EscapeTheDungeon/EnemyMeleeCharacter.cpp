// This game is fully owned by KMG


#include "EnemyMeleeCharacter.h"
#include "MainCharacter.h"
#include "AIEnemyController.h" 

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystemComponent.h>
#include <Blueprint/UserWidget.h>
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Components/SphereComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/BoxComponent.h>
#include <Engine/SkeletalMeshSocket.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "PhysicalMaterials/PhysicalMaterial.h"


// Sets default values
AEnemyMeleeCharacter::AEnemyMeleeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	
	//// Create AgroSphere
	//AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	//AgroSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRange"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	// Construct left and right weapon collision boxes
	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponBox"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("hand_l"));

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponBox"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("hand_r"));

	LeftWeaponSocket = (TEXT("FX_Trail_L_01"));

	RightWeaponSocket = (TEXT("FX_Trail_R_01"));

	// Attacks
	Attack01 = (TEXT("Attack01"));
	DoubleAttack = (TEXT("DoubleAttack"));
	TwoRightAttacks = (TEXT("2RightAttacks"));
	AttackFinisher = (TEXT("FinishAttack1"));

	
	//AttackWaitTime = 1.f;

	//bDying = false;

	//DeathTime = 4.0;

}

// Called when the game starts or when spawned
void AEnemyMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMeleeCharacter::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyMeleeCharacter::CombatRangeEndOverlap);

	//Bind functions to overlap events for weapon boxes
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMeleeCharacter::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMeleeCharacter::OnRightWeaponOverlap);

	//Set Collision Presets for weapon boxes
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}

}

// Called every frame
void AEnemyMeleeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyMeleeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyMeleeCharacter::PlayCombatMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CombatMontage) {
		AnimInstance->Montage_Play(CombatMontage);
		AnimInstance->Montage_JumpToSection(Section, CombatMontage);
	}
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AEnemyMeleeCharacter::ResetCanAttack, AttackWaitTime);

	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
	}
}

FName AEnemyMeleeCharacter::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(1,3) };

	switch (Section) {

		case 1:

			SectionName = Attack01;

			break;
		case 2:

			SectionName = DoubleAttack;

			break;
		case 3:

			SectionName = TwoRightAttacks;

			break;
	}

	return SectionName;
}

void AEnemyMeleeCharacter::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(
		AttackWaitTimer,
		this,
		&AEnemyMeleeCharacter::ResetCanAttack,
		AttackWaitTime
	);
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(
			FName("CanAttack"),
			false);
	}
}

void AEnemyMeleeCharacter::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;

	auto Character = Cast<AMainCharacter>(OtherActor);

	if (Character) {

		bInAttackRange = true;
		UE_LOG(LogTemp, Warning, TEXT("In Range"));


		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void AEnemyMeleeCharacter::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{


	auto Character = Cast<AMainCharacter>(OtherActor);

	if (Character) {

		bInAttackRange = false;

		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemyMeleeCharacter::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AMainCharacter>(OtherActor);

	if (Character) {

		DoDamage(Character);

		SpawnBlood(Character, LeftWeaponSocket);
	}
}

void AEnemyMeleeCharacter::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AMainCharacter>(OtherActor);

	if (Character) {

		DoDamage(Character);

		SpawnBlood(Character, RightWeaponSocket);
	}
}

void AEnemyMeleeCharacter::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyMeleeCharacter::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyMeleeCharacter::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyMeleeCharacter::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyMeleeCharacter::DoDamage(AActor* Victim)
{
	if (Victim == nullptr) return;

	UGameplayStatics::ApplyDamage(Victim, BaseDamage, EnemyController, this, UDamageType::StaticClass());
}

//void AEnemyMeleeCharacter::SpawnBlood(AMainCharacter* Victim, FName SocketName)
//{
//	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };
//
//	if (TipSocket) {
//
//		const FTransform SocketTransform{ TipSocket->GetSocketTransform(GetMesh()) };
//
//		if (Victim->GetBloodParticles()) {
//
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Victim->GetBloodParticles(), SocketTransform);
//		}
//
//	}
//}
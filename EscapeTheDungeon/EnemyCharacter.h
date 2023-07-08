// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	EES_Idle					UMETA(DisplayName = "Idle"),
	EES_Suspicious				UMETA(DisplayName = "Suspicious"),
	EES_Alerted					UMETA(DisplayName = "Alerted"),
	

	EMS_MAX						UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EEMS_Idle					UMETA(DisplayName = "Idle"),
	EEMS_Walking				UMETA(DisplayName = "Walking"),
	EEMS_Attack					UMETA(DisplayName = "Attack"),
	EEMS_Dead					UMETA(DisplayName = "Dead"),

	EMS_MAX						UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class ESCAPETHEDUNGEON_API AEnemyCharacter : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();



	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArmComp;
	
	//health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Health")
		class UHealthComponent* HealthComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Health")
		class UPawnSensingComponent* PawnSensingComp;

	bool bCanBeSeen;

	///* Max health of the character*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	//	float DefaultHealth;

	///* Current health of the character*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	//	float Health;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	//	uint8 TeamNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Health")
		bool bDied;

	//Enum
//Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EEnemyMovementStatus EnemyMovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EEnemyAIState GuardState;

	////Set movement Status and running speed
	//void SetMovementStatus(EEnemyMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float WalkingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float RunningSpeed;

	///Booleens
	bool bShiftKeyDown;
	bool bCTRLKeyDown;

	bool bMovingForward;
	bool bMovingRight;

	bool bLMBDown;

	//character movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/* Character Health*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float BaseDamage;

protected:

	//Impact FX and SFX
	/* Particles to spawn when hit by bullets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class UParticleSystem* ImpactParticles;

	/* Sound to play when hit*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class USoundCue* ImpactSound;

	// Bullet spread in degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (AllowPrivateAccess = true), meta = (ClampMin = 0.0f))
		float BulletSpread;

	//Health
	/* Amount of damage when the bullet hits a critical spot*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
		float HeadShotDamage;

	/* Name of the head bone | Critical area*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		FString HeadBone;


	/* Time to display HP once shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float HealthBarDisplayTime;

	FTimerHandle HealthBarTimer;

	///* Damage with bullet*/
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	//	class TSubclassOf<class UDamageType>DamageType;

	/* Montage containing hit and death montages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		UAnimMontage* HitMontage;

	/* Montage containing Combat animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		UAnimMontage* DeathMontage;

	UFUNCTION(BlueprintCallable)
		EPhysicalSurface GetEnemySurfaceType();

	/* True when Enemy can attack*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		bool bCanAttack;

	FTimerHandle AttackWaitTimer;

	/* Minimum wait time between attacks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float AttackWaitTime;

	FName Attack01;

//protected:
	
	FTimerHandle HitReactTimer;

	bool bCanReact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float HitReactTimeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float HitReactTimeMax;

	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = true))
		TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = true))
		float HitNumberDestroyTime;

	//Behavior tree
		/* Behaviour tree for the AI  Character*/
	UPROPERTY(EditAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = true))
		class UBehaviorTree* BehaviorTree;


	// Patrol Points

	/* Points for the enemt to move to*/
	UPROPERTY(EditAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
		FVector PatrolPoint;

	/* Second Point for the enemy to move to*/
	UPROPERTY(EditAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
		FVector PatrolPoint2;

	//Enemy Controller
	class AAIEnemyController* EnemyController;

	///* Agroing the enemy*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
	//	class USphereComponent* AgroSphere;

	/* Stunning the player*/

		/* true when playing the get hit animation*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
		bool bStunned;

	/* Chance of being stunned. 0: no stun Chance, 1: 100% stun chance*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
		float StunChance;


	/* Attacking the player*/
		/* True in attack range? Time to attack!*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
		bool bInAttackRange;

	class AMainCharacter* Main;

	FTimerHandle DeathTimer;

	/* Time after death before it's destroyed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float DeathTime;

	/* FoceInline*/
public:

	FORCEINLINE FString GetHeadBone() const { return HeadBone; }

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController) override;

	//Health
	//bool IsFriendly(AActor* ActorA, AActor* ActorB);

	UFUNCTION()
		void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	UFUNCTION(BlueprintNativeEvent)
		void ShowHealthBar();

	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
		void HideHealthBar();

	void Die();


	void PlayHitMontage(FName Section, float PlayRate = 1.0f);

	//Movement
	//Character movement
	void MoveForward(float Value);//forward and backwards movement
	void MoveRight(float Value);//left and right movement
	void TurnAtRate(float Rate);
	void LookUpRate(float Rate);

	virtual void Jump() override;
	UFUNCTION()
		void BeginCrouch();
	UFUNCTION()
		void EndCrouch();


	///Shooting functions
// The direction of the gun shots
	virtual FVector GetPawnViewLocation() const override;

	void ResetHitReactTimer();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

	UFUNCTION(BlueprintCallable)
		void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
		void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumber();

	///** Called when something is overlaps with the AgroSphere*/
	//UFUNCTION()
	//	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Calling the stun*/
	UFUNCTION(BlueprintCallable)
		void SetStunned(bool Stunned);

	///* Calling the attack range*/
	//UFUNCTION()
	//	void CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION()
	//	void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//void PlayCombatMontage(FName Section, float PlayRate = 1.0f);

	void SpawnBlood(AMainCharacter* Victim, FName SocketName);

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	UFUNCTION()
	void DestroyEnemy();

	UFUNCTION(BlueprintCallable)
		void OnPawnSeen(APawn* SeenPawn);

	void ResetCanAttack();

};

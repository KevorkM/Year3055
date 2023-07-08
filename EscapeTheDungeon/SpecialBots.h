// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BulletHitInterface.h"
#include "SpecialBots.generated.h"

UCLASS()
class ESCAPETHEDUNGEON_API ASpecialBots : public APawn, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpecialBots();
public:

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		class USphereComponent* SphereComp;

	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComp;

	class ATracker* Tracker;

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

	/* Character Health*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float BaseDamage;

	// Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

protected:

	//Impact FX and SFX
	/* Particles to spawn when hit by bullets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class UParticleSystem* ImpactParticles;

	/* Sound to play when hit*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class USoundCue* ImpactSound;

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

	/* True when Enemy can attack*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		bool bCanAttack;

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


	//// Patrol Points
	///* Points for the enemt to move to*/
	//UPROPERTY(EditAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
	//	FVector PatrolPoint;

	///* Second Point for the enemy to move to*/
	//UPROPERTY(EditAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = true, MakeEditWidget = "true"))
	//	FVector PatrolPoint2;

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

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController) override;

	////Health
	//bool IsFriendly(AActor* ActorA, AActor* ActorB);


	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
		void HandleTakeDamage(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintNativeEvent)
		void ShowHealthBar();

	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
		void HideHealthBar();

	void Die();

	void SelfDestruct();

	void ResetHitReactTimer();

	UFUNCTION(BlueprintImplementableEvent)
		void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

	UFUNCTION(BlueprintCallable)
		void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
		void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumber();

	UFUNCTION(BlueprintCallable)
		void FinishDeath();

	UFUNCTION()
		void DestroyEnemy();

	//UFUNCTION(BlueprintCallable)
	//	void OnPawnSeen(APawn* SeenPawn);

	/**************************************************/

	FVector GetNextPathPoint();

	// Next point in navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		UParticleSystem* ExplosionEffect;

	bool bExploded;

	// Did we already kick off self destruct timer
	bool bStartedSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float SelfDamageInterval;

	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		USoundCue* ExplodeSound;

public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	//virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:

	// CHALLENGE CODE	

	// Find nearby enemies and grow in 'power level' based on the amount.
	void OnCheckNearbyBots();

	// the power boost of the bot, affects damaged caused to enemies and color of the bot (range: 1 to 4)
	int32 PowerLevel;

	FTimerHandle TimerHandle_RefreshPath;

	void RefreshPath();

};

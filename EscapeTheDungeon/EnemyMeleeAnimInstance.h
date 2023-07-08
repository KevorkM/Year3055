// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyMeleeAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API UEnemyMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

protected:

	/* Lateral Movement Speed*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	class AEnemyMeleeCharacter* MeleeEnemy;
	class AEnemyCharacter* ShooterEnemy;

	// Functions
public:

	UFUNCTION(BlueprintCallable, Category = "AnimationProperties")
		void UpdateAnimationProperties(float DeltaTime);
};

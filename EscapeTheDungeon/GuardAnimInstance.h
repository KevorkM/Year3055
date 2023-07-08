// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GuardAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API UGuardAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

protected:

	/* Lateral Movement Speed*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsDead;

	class AEnemyCharacter* Enemy;

	class AActor* Pawn;

	// Functions
public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "AnimationProperties")
		void UpdateAnimationProperties(float DeltaTime);
};

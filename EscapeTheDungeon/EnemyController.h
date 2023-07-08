// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:

	AEnemyController();

private:
	/* BB Component for this enemy*/
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = true))
		class UBlackboardComponent* BlackboardComponent;

	/* BT Component for this enemy*/
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = true))
		class UBehaviorTreeComponent* BehaviorTree;

	//ForceInlines
public:
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }

	FORCEINLINE UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return BehaviorTree; }


protected:

public:

	virtual void OnPossess(APawn* InPawn) override;

};

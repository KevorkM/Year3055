// This game is fully owned by KMG


#include "EnemyController.h"
#include "EnemyMeleeCharacter.h"
#include "EnemyCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AEnemyController::AEnemyController() {

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

		BehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree"));
	check(BehaviorTree);


}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr)return;

	AEnemyMeleeCharacter* Enemy = Cast<AEnemyMeleeCharacter>(InPawn);

	if (Enemy) {
		if (Enemy->GetBehaviorTree()) {
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}
	}
}

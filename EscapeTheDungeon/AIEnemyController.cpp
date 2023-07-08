
// This game is fully owned by KMG

#include "AIEnemyController.h"
#include "EnemyCharacter.h"
#include "EnemyMeleeCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AAIEnemyController::AAIEnemyController() {

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree"));
	check(BehaviorTree);
}

void AAIEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr)return;

	AEnemyCharacter* ShooterEnemy = Cast<AEnemyCharacter>(InPawn);
	AEnemyMeleeCharacter* MeleeEnemy = Cast<AEnemyMeleeCharacter>(InPawn);

		if (ShooterEnemy) {
			if (ShooterEnemy->GetBehaviorTree()) {
				BlackboardComponent->InitializeBlackboard(*(ShooterEnemy->GetBehaviorTree()->BlackboardAsset));
			}
		}
		if (MeleeEnemy) {
			if (MeleeEnemy->GetBehaviorTree()) {
				BlackboardComponent->InitializeBlackboard(*(MeleeEnemy->GetBehaviorTree()->BlackboardAsset));
			}
		}
}

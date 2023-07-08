// This game is fully owned by KMG


#include "EnemyMeleeAnimInstance.h"
#include "EnemyMeleeCharacter.h"
#include "EnemyCharacter.h"

void UEnemyMeleeAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MeleeEnemy == nullptr) {
		MeleeEnemy = Cast<AEnemyMeleeCharacter>(TryGetPawnOwner());
	}

	if (MeleeEnemy) {
		FVector Velocity{ MeleeEnemy->GetVelocity() };

		Velocity.Z = 0.f;

		Speed = Velocity.Size();
	}

	if (ShooterEnemy == nullptr) {
		ShooterEnemy = Cast<AEnemyCharacter>(TryGetPawnOwner());
	}

	if (ShooterEnemy) {
		FVector Velocity{ ShooterEnemy->GetVelocity() };

		Velocity.Z = 0.f;

		Speed = Velocity.Size();
	}


}

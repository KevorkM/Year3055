// This game is fully owned by KMG


#include "GuardAnimInstance.h"
#include "EnemyCharacter.h"

#include <GameFramework/CharacterMovementComponent.h>

void UGuardAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
		if (Pawn) {
			Enemy = Cast<AEnemyCharacter>(Pawn);
		}
	}
}

void UGuardAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Pawn == nullptr) {
		Pawn = Cast<AEnemyCharacter>(TryGetPawnOwner());
	}

	if (Pawn) {
		FVector Velocity{ Pawn->GetVelocity() };

		Velocity.Z = 0.f;

		Speed = Velocity.Size();
	}
}

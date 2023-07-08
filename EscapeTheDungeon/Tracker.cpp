// This game is fully owned by KMG


#include "Tracker.h"
#include "EnemyCharacter.h"
#include "MainCharacter.h"
#include "SpecialBots.h"
#include "HealthComponent.h"


// Sets default values
ATracker::ATracker()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


//float ATracker::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//
//	if (MatInst == nullptr)
//	{
//		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
//	}
//
//	if (MatInst)
//	{
//		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
//	}
//
//	// Explode on hitpoints == 0
//	if (Health <= 0.0f)
//	{
//		SelfDestruct();
//	}
//	return DamageAmount;
//}

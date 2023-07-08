// This game is fully owned by KMG


#include "MyPickup.h"
#include "MainCharacter.h"

#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>
#include <Sound/SoundCue.h>

AMyPickup::AMyPickup() {

	KeyCount = 1;

}

void AMyPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);

		if (Main) {
			OnPickUpBP(Main);
			Main->SetPickupLocation().Add(GetActorLocation());

			Main->IncrementKeys(KeyCount);

			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			Destroy();
		}
	}
}

void AMyPickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

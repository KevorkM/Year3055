// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "MyPickup.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API AMyPickup : public AItem
{
	GENERATED_BODY()

		AMyPickup();

	//Variables
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keys")
		int32 KeyCount;

protected:
	/* Sound played when item is picked up*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Sound", meta = (AllowPrivateAccess = "true"))
		USoundCue* PickupSound;

	//Functions
protected:

	//// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "PickUp")
		void OnPickUpBP(class AMainCharacter* Target);

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

};

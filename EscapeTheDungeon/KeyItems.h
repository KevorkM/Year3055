// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "KeyItems.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API AKeyItems : public APickupItem
{
	GENERATED_BODY()
	
//variables
public: 

	AKeyItems();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keys")
		int32 KeyCount;

	UFUNCTION(BlueprintImplementableEvent, Category = "PickUp")
		void OnPickUpBP(class AMainCharacter* Target);

private:

	/* Mesh for the Ammo pickup*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Mesh;

	/* overlapp sphere for picking up the Ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* MeshCollisionSphere;

	/*Inlines*/
public:
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return Mesh; }
	FORCEINLINE USphereComponent* GetSphereMesh() const { return MeshCollisionSphere; }

protected:
	virtual void BeginPlay() override;

	/* override of SetItemProperties for our Ammo*/
	virtual void SetItemProperties(EItemState State) override;

	UFUNCTION()
		void MeshSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

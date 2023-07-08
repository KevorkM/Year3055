// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API AAmmo : public APickupItem
{
	GENERATED_BODY()
	

// Variables
public: 
	AAmmo();


	/* Mesh for the Ammo pickup*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AmmoMesh;

	/* Mesh for the Ammo pickup*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* AmmoBox;

	/* Ammo type for the ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo|Type", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/* The texture for the ammo icon*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo|Type", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	/* overlapp sphere for picking up the Ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AmmoCollisionSphere;

/*Inlines*/
public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }


// Functions
public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/* override of SetItemProperties for our Ammo*/
	virtual void SetItemProperties(EItemState State) override;

	UFUNCTION()
	void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

};

// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class ESCAPETHEDUNGEON_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Base shape collision  */
/** Base shape collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionVolume;

	/** Base Mesh Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Mesh;
	/** Base shape collision  */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* IdleParticlesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* OverlapParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds", meta = (AllowPrivateAccess = "true"))
		class USoundCue* OverlapSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties", meta = (AllowPrivateAccess = "true"))
		bool bRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties", meta = (AllowPrivateAccess = "true"))
		float RotationRate;

	/* the name that appears on the widget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		FName ItemName;

	/* Item Count; Ammo, etc*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items", meta = (AllowPrivateAccess = "true"))
		int32 ItemCount;

	/* picking up Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	//Force Inlines
public:

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

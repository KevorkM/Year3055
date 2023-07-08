// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenDoorWithSwitch.generated.h"

UCLASS()
class ESCAPETHEDUNGEON_API AOpenDoorWithSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDoorWithSwitch();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		class UStaticMeshComponent* FloorSwitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		UStaticMeshComponent* Door;

	/** Initial location for the door */
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
		FVector InitialDoorLocation;

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
		FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		float SwitchTime;

	bool bCharacterOnSwitch;

	void CloseDoor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//functions
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
		void UpdateDoorLocation(float Z);

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
		void UpdateFloorSwitchLocation(float Z);

};

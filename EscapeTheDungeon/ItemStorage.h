// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStorage.generated.h"

UCLASS()
class ESCAPETHEDUNGEON_API AItemStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemStorage();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TMap<FString, TSubclassOf<class AGuns>>WeaponMap;

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
		TMap<FString, TSubclassOf<class AAmmo>>AmmoMap;

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
		TMap<FString, TSubclassOf<class AKeyItems>>KeyMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	


};

// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"


USTRUCT(BlueprintType)
struct FCharacterStates
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		float DefaultHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		int32 Keys;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		int32 Ammo;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		int32 AmmoClip;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		FString WeaponName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		FString LevelName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
		FRotator Rotation;
};


UCLASS()
class ESCAPETHEDUNGEON_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UMySaveGame();

// Variables
public:

	UPROPERTY(VisibleAnywhere, Category = "Basic")
		FCharacterStates CharacterStates;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
		FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
		uint32 UserIndex;

protected:

//	Inlines

public:


//	Functions

public:

protected:


};

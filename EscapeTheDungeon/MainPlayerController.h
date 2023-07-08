// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPETHEDUNGEON_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()


public:

	AMainPlayerController();

	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets|BP")
		TSubclassOf<class UUserWidget> HUDOverlayClass;

	/** Variable to hold the widget after creating it */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
		UUserWidget* HUDOverlay;

public:

	virtual void BeginPlay() override;

};

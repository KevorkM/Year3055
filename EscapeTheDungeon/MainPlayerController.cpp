// This game is fully owned by KMG


#include "MainPlayerController.h"

#include <Blueprint/UserWidget.h>
#include <Components/WidgetComponent.h>

AMainPlayerController::AMainPlayerController() {

}

void AMainPlayerController::BeginPlay()
{
	//Super::BeginPlay();

	// Check our hud overlay class TSubclassOf variable

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if (HUDOverlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hud Overlay"));
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}

}

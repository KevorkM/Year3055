// This game is fully owned by KMG


#include "LevelTransitionVolume.h"
#include "MainCharacter.h"


#include <Components/BoxComponent.h>
#include <Components/BillboardComponent.h>
#include <Kismet/GameplayStatics.h>


// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TransitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Transition"));
	RootComponent = TransitionVolume;

	BillBoard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillBoard->SetupAttachment(GetRootComponent());

	TransitionLevelName = "";

}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();
	
	TransitionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnOverlapBegin);

}

// Called every frame
void ALevelTransitionVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTransitionVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);

		if (Main) {
			Main->SwitchLevel(TransitionLevelName);
		}
	}

}


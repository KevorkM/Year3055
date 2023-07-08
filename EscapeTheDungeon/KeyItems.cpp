// This game is fully owned by KMG


#include "KeyItems.h"
#include "PickupItem.h"
#include "MainCharacter.h"

#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>

AKeyItems::AKeyItems() {

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetPickupWidget()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());

	MeshCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MeshCollisionSphere"));
	MeshCollisionSphere->SetupAttachment(GetRootComponent());
	MeshCollisionSphere->SetSphereRadius(50.f);

	KeyCount = 1;
}


void AKeyItems::BeginPlay()
{
	Super::BeginPlay();

	MeshCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AKeyItems::MeshSphereOverlap);
}

void AKeyItems::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);

	switch (State)
	{
	case EItemState::EIS_Pickup:
		// Set mesh properties
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetVisibility(true);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		// Set mesh properties
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetVisibility(true);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Falling:
		// Set mesh properties
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);
		Mesh->SetEnableGravity(true);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);

		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		// Set mesh properties
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetVisibility(true);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	}
}

void AKeyItems::MeshSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {

		auto OverlappedCharacter = Cast<AMainCharacter>(OtherActor);

		if (OverlappedCharacter) {

			OnPickUpBP(OverlappedCharacter);
			OverlappedCharacter->IncrementKeys(KeyCount);

			StartItemCurve(OverlappedCharacter);
			MeshCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

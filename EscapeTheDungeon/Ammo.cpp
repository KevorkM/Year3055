// This game is fully owned by KMG


#include "Ammo.h"
#include "PickupItem.h"
#include "MainCharacter.h"

#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>

AAmmo::AAmmo() {

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	AmmoBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoBox"));
	AmmoBox->SetupAttachment(GetRootComponent());

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetPickupWidget()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());

	AmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AmmoCollisionSphere"));
	AmmoCollisionSphere->SetupAttachment(GetRootComponent());
	AmmoCollisionSphere->SetSphereRadius(50.f);
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	AmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::AmmoSphereOverlap);

}

void AAmmo::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);

	switch (State)
	{
	case EItemState::EIS_Pickup:
		// Set mesh properties
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Boxmesh properties
		AmmoBox->SetSimulatePhysics(false);
		AmmoBox->SetEnableGravity(false);
		AmmoBox->SetVisibility(true);
		AmmoBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		// Set mesh properties
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set mesh properties
		AmmoBox->SetSimulatePhysics(false);
		AmmoBox->SetEnableGravity(false);
		AmmoBox->SetVisibility(true);
		AmmoBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		break;
	case EItemState::EIS_Falling:
		// Set mesh properties
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetEnableGravity(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);

		// Set mesh properties
		AmmoBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoBox->SetSimulatePhysics(true);
		AmmoBox->SetEnableGravity(true);
		AmmoBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
	
		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		// Set mesh properties
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Box mesh properties
		AmmoBox->SetSimulatePhysics(false);
		AmmoBox->SetEnableGravity(false);
		AmmoBox->SetVisibility(true);
		AmmoBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		break;
	}

}

void AAmmo::AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		auto OverlappedCharacter = Cast<AMainCharacter>(OtherActor);

		if (OverlappedCharacter) {
			StartItemCurve(OverlappedCharacter);
			AmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

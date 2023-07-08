// This game is fully owned by KMG


#include "PickupItem.h"
#include "MainCharacter.h"

#include <Components/SphereComponent.h>
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/WidgetComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>

// Sets default values
APickupItem::APickupItem() :
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),
	//Item Interp variables
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterp(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	InterpLocIndex(0),
	SlotIndex(0),
	bCharacterInventoryFull(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(SkeletalMesh);

	CollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionVolume->SetupAttachment(GetRootComponent());
	CollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	ItemType = EItemType::EIT_Max;
}

void APickupItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnOverlapBegin);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &APickupItem::OnOverlapEnd);

	if (PickupWidget) {

		//Hide Pickup Widget
		PickupWidget->SetVisibility(false);

	}
	//set active stars array based on Item rarity
	SetActiveStars();

	//Set Item properties based on ItemState
	SetItemProperties(ItemState);
}

// Called every frame
void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle Item Interpin when in the EquipInterping State
	ItemInterp(DeltaTime);

}

void APickupItem::StartItemCurve(AMainCharacter* Char, bool bForcePlaySound)
{
	// Store a handle to the Character
	Character = Char;

	// Getarray index in interp Locations with the lowest item count
	InterpLocIndex = Character->GetInterpLocationIndex();

	// Add 1 to the Item Count for this interp Locations
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	PlayPickupSound();
	

	// Store initial location of the Item
	ItemInterpStartLocation = GetActorLocation();
	bInterp = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer,
		this,
		&APickupItem::FinishInterping,
		ZCurveTime);

	// Get initial Yaw of the Camera
	const float CameraRotationYaw{ Character->GetCameraComponent()->GetComponentRotation().Yaw };
	// Get initial Yaw of the Item
	const float ItemRotationYaw{ GetActorRotation().Yaw };
	// Initial Yaw offset between Camera and Item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;
}

void APickupItem::PlayPickupSound(bool bForcePlaySound)
{
	if (Character) {
		if (bForcePlaySound) {
			if (PickupSound) {
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		else if (Character->ShouldPlayPickupSound()) {
			
			Character->StartPickupSoundTimer();
			if (PickupSound) {
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void APickupItem::PlayEquipSound(bool bForcePlaySound)
{
	if (Character) {
		if (bForcePlaySound) {
			if (EquipSound) {
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
		else if (Character->ShouldPlayEquipSound()) {
			Character->StartEquipSoundTimer();
			if (EquipSound) {
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

void APickupItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);

		if (Main)
		{
			Main->IncrementOverlappedItemCount(1);
		}
	}
}

void APickupItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);

		if (Main)
		{
			Main->IncrementOverlappedItemCount(-1);
			Main->UnHightlightInventorySlot();
		}
	}
}

void APickupItem::SetActiveStars()
{
	// The 0 element is not used.
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);

	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;

	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	}
}

void APickupItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		
		// Set mesh properties
		SkeletalMesh->SetSimulatePhysics(false);
		SkeletalMesh->SetEnableGravity(false);
		SkeletalMesh->SetVisibility(true);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// Set CollisionBox properties
		CollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionVolume->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;
	case EItemState::EIS_Equipped:

		PickupWidget->SetVisibility(false);
		// Set mesh properties
		SkeletalMesh->SetSimulatePhysics(false);
		SkeletalMesh->SetEnableGravity(false);
		SkeletalMesh->SetVisibility(true);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionVolume properties
		CollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Falling:
		
		// Set mesh properties
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetEnableGravity(true);
		SkeletalMesh->SetVisibility(true);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionVolume properties
		CollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_EquipInterping:

		PickupWidget->SetVisibility(false);
		// Set mesh properties
		SkeletalMesh->SetSimulatePhysics(false);
		SkeletalMesh->SetEnableGravity(false);
		SkeletalMesh->SetVisibility(true);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionVolume properties
		CollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Pickedup:

		PickupWidget->SetVisibility(false);
		// Set mesh properties
		SkeletalMesh->SetSimulatePhysics(false);
		SkeletalMesh->SetEnableGravity(false);
		SkeletalMesh->SetVisibility(false);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionVolume properties
		CollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void APickupItem::FinishInterping()
{
	bInterp = false;
	if (Character)
	{
		// Subtract 1 from the item Count of the interp Location struct
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
		Character->GetPickupItem(this);

		Character->UnHightlightInventorySlot();
	}
	//Set Scale back to normal
	SetActorScale3D(FVector(1.f));
}

void APickupItem::ItemInterp(float DeltaTime)
{
	if (!bInterp) { return; }

	if (Character && ItemZCurve)
	{
		// Elapsed time since we started ItemInterpTimer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// Get curve value corresponding to ElapsedTime
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		UE_LOG(LogTemp, Warning, TEXT("CurveValue: %f"), CurveValue);
		// Get the item's initial location when the curve started
		FVector ItemLocation = ItemInterpStartLocation;
		// Get location in front of the camera
		const FVector CameraInterpLocation{ GetInterpLocation() };

		// Vector from Item to Camera Interp Location, X and Y are zeroed out
		const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };
		// Scale factor to multiply with CurveValue
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		// Interpolated X value
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X,
			CameraInterpLocation.X,
			DeltaTime,
			30.0f);
		// Interpolated Y value
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.f);

		// Set X and Y of ItemLocation to Interped values
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		// Adding curve value to the Z component of the Initial Location (scaled by DeltaZ)
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// Camera rotation this frame
		const FRotator CameraRotation{ Character->GetCameraComponent()->GetComponentRotation() };
		// Camera rotation plus inital Yaw Offset
		FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

FVector APickupItem::GetInterpLocation()
{
	if (Character == nullptr)return FVector(0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		
		break;

	case EItemType::EIT_Weapon:

		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();

		break;
	}

	return FVector();
}

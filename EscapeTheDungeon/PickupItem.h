// This game is fully owned by KMG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

UENUM(BlueprintType)
enum class EItemRarity :uint8 {

	EIR_Damaged		UMETA(Display = "Damaged"),
	EIR_Common		UMETA(Display = "Common"),
	EIR_Uncommon	UMETA(Display = "Uncommon"),
	EIR_Rare		UMETA(Display = "Rare"),
	EIR_Legendary	UMETA(Display = "Damaged"),

	EIR_Max		UMETA(Display = "Default Max")
};
 
UENUM(BlueprintType)
enum class EItemState :uint8 {

	EIS_Pickup			UMETA(DisplayName ="Pickup"),
	EIS_EquipInterping	UMETA(DisplayName = "EquipInterping"),
	EIS_Pickedup		UMETA(DisplayName = "Pickedup"),
	EIS_Equipped		UMETA(DisplayName = "Equipped"),
	EIS_Falling			UMETA(DisplayName = "Falling"),

	EIS_Max		UMETA(Display = "Default Max")
};

UENUM(BlueprintType)
enum class EItemType : uint8 
{
	EIT_Ammo	UMETA(DisplayName = "Ammo"),
	EIT_Weapon	UMETA(DisplayName = "Weapon"),

	EIT_Max		UMETA(DisplayName "Default")
};

UCLASS()
class ESCAPETHEDUNGEON_API APickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

	//Variables
protected:

	/** Base shape collision  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Collision", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkeletalMesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SkeletalMesh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " StaticMesh", meta = (AllowPrivateAccess = "true"))
	//	class UStaticMeshComponent* Barrel;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMesh", meta = (AllowPrivateAccess = "true"))
	//	UStaticMeshComponent* Stock;

	/* enables item tracing when overlapped*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Mesh", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	/* picking up Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	/* the name that appears on the widget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/* Item Count; Ammo, etc*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	/* Item Rarity determins the number of stars in the Pickup WIdget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/* State of the Item*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		EItemState ItemState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
		TArray<bool> ActiveStars;

	/* Starting location when interp beging*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	/* Target interp location in front of the camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;

	/* true when interping*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	bool bInterp;

	/*Plays when we start interping*/
	FTimerHandle ItemInterpTimer;

	/* the curve asset to use for the item's Z location when interping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
		class UCurveFloat* ItemZCurve;

	/* Duration of the curve and timer*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime;

	/* Pointer to the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	class AMainCharacter* Character;

	/*X and Y for the item while interping in the EquipInterping*/
	float ItemInterpX;
	float ItemInterpY;

	/* initial yaw offset between the camera and the interping Item*/
	float InterpInitialYawOffset;

	/* Curve used to scale the item when interping*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	UCurveFloat* ItemScaleCurve;

	/* Sound played when Item is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* EquipSound;

	/* Sound played when item is picked up*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Sound", meta = (AllowPrivateAccess = "true"))
		USoundCue* PickupSound;

	/* Enum for the type of item this item is*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Enum", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	/* Index of the interp location this item is interping to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex;


	/* Background for this item in the inventory*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;

	/* Icon for this item in the inventory*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconItem;

	/* Icon for this item in the inventory*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconAmmo;

	/* Slot in the inventory array*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;

	/* true when the character's inventory is full*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	bool bCharacterInventoryFull;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionVolume; }

	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return SkeletalMesh; }

	//FORCEINLINE UStaticMeshComponent* GetBarrelMesh() const { return Barrel; }
	//FORCEINLINE void SetBarrelMesh(UStaticMeshComponent* BarrelMesh) { Barrel = BarrelMesh; }

	//FORCEINLINE UStaticMeshComponent* GetStockMesh() const { return Stock; }
	//FORCEINLINE void SetStockMesh(UStaticMeshComponent* StockMesh) { Stock = StockMesh; }

	FORCEINLINE USoundCue* GetEquippedSound() const { return EquipSound; }
	FORCEINLINE void SetEquippedSound(USoundCue* Sound) { EquipSound = Sound; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE void SetPickupSound(USoundCue* Sound) { PickupSound = Sound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void SetCharacter(AMainCharacter* Char) { Character = Char; }

	FORCEINLINE void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; }

	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; }
	/* Item Icon for the inventory*/
	FORCEINLINE void SetIconItem(UTexture2D* Icon) { IconItem = Icon; }
	/* Set Ammo Icon*/
	FORCEINLINE void SetAmmoItem(UTexture2D* Icon) { IconAmmo = Icon; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//functions
public:
	
	/* Called form the MainCharacter class*/
	void StartItemCurve(AMainCharacter* Char, bool bForcePlaySound = false);

	void PlayPickupSound(bool bForcePlaySound = false);

	/* Called in AMainCharacter::GetPickupItem*/
	void PlayEquipSound(bool bForcePlaySound = false);

protected:

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Sets the ActiveStars array of bools based on rarity*/
	void SetActiveStars();

	/* Set properties of the Item's based on State */
	virtual void SetItemProperties(EItemState State);

	/* Called when Item Interp timer is finished */
	void FinishInterping();

	/* Handles Item Interpolation when in the EquipeInterping State*/
	void ItemInterp(float DeltaTime);

	/* Get Interp Location based on the item Type*/
	FVector GetInterpLocation();
};

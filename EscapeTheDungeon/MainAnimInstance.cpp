// This game is fully owned by KMG


#include "MainAnimInstance.h"
#include "MainCharacter.h"
#include "Guns.h"
#include "WeaponType.h"
#include "HealthComponent.h"

#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>

UMainAnimInstance::UMainAnimInstance() 
{
	Speed = 0.f;

	bIsInAir = false;

	bIsAccelerating = false;

	MovementOffsetYaw = 0.f;

	LastMovementOffsetYaw = 0.0;

	bAiming = false;

	CharacterRotation = FRotator(0.f);

	CharacterRotationLastFrame = FRotator(0.f);

	TIPCharacterYaw = 0.f;

	TIPCharacterYawLastFrame = 0.f;

	YawDelta = 0.f;

	RootYawOffset = 0.f;

	Pitch = 0;

	bReloading = false;

	RecoilWeight = 1.f;

	bTurningInPlace = false;

	EquippedWeaponType = EWeaponType::EWT_MAX;

	bShouldUseFabrik = false;
}

void UMainAnimInstance::NativeInitializeAnimation()
{
	Main = Cast<AMainCharacter>(TryGetPawnOwner());
}

void UMainAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Main == nullptr) {
		Main = Cast<AMainCharacter>(TryGetPawnOwner());
	}

	// Get the lateral speed of the character for velocity
	if (Main) {

		bReloading = Main->GetCombatState() == ECombatState::ECS_Reloading;
		bIsEquipping = Main->GetCombatState() == ECombatState::ECS_Equipping;
		bShouldUseFabrik = Main->GetCombatState() == ECombatState::ECS_Unoccupied || Main->GetCombatState() == ECombatState::ECS_FireTimerInProgress;
	//	bIsDead = HealthComp->bIsDead;

		FVector Velocity{ Main->GetVelocity() };
		Velocity.Z = 0;

		Speed = Velocity.Size();

		// Is the character in the air? 
		bIsInAir = Main->GetCharacterMovement()->IsFalling();
		if (bIsInAir) {
			bShouldUseFabrik = false;
		}
		

		// Is Character Crouching
		bIsCrouching = Main->GetCrouching();

		//is the character Accelerating
		if (Main->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) {
			bIsAccelerating = true;
		}
		else {
			bIsAccelerating = false;
		}
		FRotator AimRotation = Main->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Main->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	
		if (Main->GetVelocity().Size() > 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = Main->GetAiming();
		
		//Check if the main character has a valid EquippedWeapon
		if (Main->GetEquippedGun()) {

			EquippedWeaponType = Main->GetEquippedGun()->GetWeaponType();
		}
	
	}



	TurnInPlace();
	Lean(DeltaTime);
}

void UMainAnimInstance::TurnInPlace()
{
	if (Main == nullptr) return;

	Pitch = Main->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		// Don't want to turn in place; Character is moving
		RootYawOffset = 0.f;
		TIPCharacterYaw = Main->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = Main->GetActorRotation().Yaw;
		const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

		// Root Yaw Offset, updated and clamped to [-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		// 1.0 if turning, 0.0 if not
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			bTurningInPlace = true;

			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// RootYawOffset > 0, -> Turning Left. RootYawOffset < 0, -> Turning Right.
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else 
		{
			bTurningInPlace = false;
		}
	}
	// Set the recoil wait
	if (bTurningInPlace) {
		if (bReloading || bIsEquipping)
		{
			RecoilWeight = 1.0f;
		}
		else {
			RecoilWeight = 0.f;
		}
	}
	else // not turning in place
	{
		if (bIsCrouching) {

			if (bReloading || bIsEquipping)
			{
				RecoilWeight = 1.0f;
			}
			else {
				RecoilWeight = 0.1f;
			}
		}
		else {
			if (bAiming || bReloading || bIsEquipping) {
				RecoilWeight = 1.f;
			}
			else {
				RecoilWeight = 0.5f;
			}
		}
	}
}

void UMainAnimInstance::Lean(float DeltaTime)
{
	if (Main == nullptr) return;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = Main->GetActorRotation();

	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	const float Target{ Delta.Yaw / DeltaTime };
	const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f) };
	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "XRMotionControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "VRTeleportVisualizer.h"
#include "InputActionValue.h"
#include "HandComponent.h"
#include "GrabComponent.h"
#include "Item.h"
#include "DrumStick.h"
#include "VRCharacter.generated.h"


UENUM(BlueprintType)
enum class EDominantEye : uint8
{
	DE_LeftEye		UMETA(DisplayName = "LeftEye"),
	DE_RightEye		UMETA(DisplayName = "RightEye")
};


UCLASS(config = Game)
class ANOTHERSTARDEMO_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DominantEye")
	EDominantEye DominantEye;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	class UInputAction* MovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* TurnAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* SnapTurnAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* TeleportAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* LeftGrabTriggerAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* RightGrabTriggerAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* ResetViewAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* OpenStatusUIAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* OpenMenuUIAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	class UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	UInputMappingContext* HandsMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	UInputMappingContext* OrbitMappingContext;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRateGamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	class USceneComponent* VRTrackingCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	class UCameraComponent* Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	class UMotionControllerComponent* LeftController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UMotionControllerComponent* RightController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	class UHandComponent* LeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UHandComponent* RightHand;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = __hide)
	class UGrabComponent* HeldComponentLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = __hide)
	UGrabComponent* HeldComponentRight;
	*/

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	//class UNiagaraComponent* TeleportPath;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	//class UWidgetInteractionComponent* MenuInteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitResult")
	class AActor* HitResultActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SunPosition")
	class AOrbitDiskParts* PartsOrbitDisk;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DominantEye")
	class ADominateEyeTestActor* DominantEyeTestActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	class AItem* ActiveOverlappingItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	class ADrumStick* GrabbedDrumStick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	AVRTeleportVisualizer* TeleportVisualizerReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	float AxisDeadzoneThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	FVector ProjectedTeleportLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	FVector TeleportProjectPointToNavigationQueryExtent;

	//DirectionalVector값을 담을 Array로 OrbitDisk를 돌릴 때 활용 된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = __hide)
	TArray<FVector> DV_Array;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Slot")
	//TArray<AItem*> ConsumableItemSlot;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Slot")
	//TArray<AItem*> UnConsumableItemSlot;

	// GrabComponent와 GripPosition과의 Collision활성 범위 반지름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	float GrabRadiusFromGripPosition;

	// 이 State가 활성화 되어 있는 경우에만 Tick 함수에서 FingerPointer가 활성화 되도록 함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bCanFingerPointer;

	// 이 State가 활성화 되어 있는 경우에만 OrbitPlatform 위에서 기도하는 애니메이션 기능이 활성화 되도록 함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bCanPray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bSnapRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bDoOnceTeleport;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bDoOnceSnapTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bRightTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsHeld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bValidTeleportLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bTeleportTraceActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bValidRayTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bGrabOrbitDisk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bCanReverse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bPlayReverse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bTestState;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Movement(const FInputActionValue& Value);
	
	// 회전 관련 함수는 Binding 하지 않고 현재 시점에선 Blueprint로 구현 및 Binding
	void Turn(const FInputActionValue& Value);

	// 회전 관련 함수는 Binding 하지 않고 현재 시점에선 Blueprint로 구현 및 Binding
	void SnapTurn(const FInputActionValue& Value);

	void RightTrigger(const FInputActionValue& Value);

	void RightRelease(const FInputActionValue& Value);

	void RightGrabDisk(const FInputActionValue& Value);

	void RightGrabRelease(const FInputActionValue& Value);

	void LeftGrabTrigger(const FInputActionValue& Value);

	void LeftGrabRelease(const FInputActionValue& Value);

	void ResetView(const FInputActionValue& Value);

	void Teleport(const FInputActionValue& Value);

	void OpenStatusUI(const FInputActionValue& Value);

	void CloseStatusUI(const FInputActionValue& Value);

	void OpenMenuUI(const FInputActionValue& Value);

	void CloseMenuUI(const FInputActionValue& Value);

	//UFUNCTION(BlueprintCallable)
	//UGrabComponent* GetGrabComponentNearMotionController(class UMotionControllerComponent* MotionController) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	TArray<FVector> GetRayPoint();

	UFUNCTION()
	FHitResult GetRayHitResult(FVector StartLoc, FVector EndLoc);

	UFUNCTION()
	void SetRayOrbitDisk(AActor* HitActor);

	UFUNCTION()
	bool IsCounterClockwise(const FVector2D& Vector1, const FVector2D& Vector2);

	UFUNCTION()
	float CalculateRotateDiskAngle(FHitResult HitResult, AActor* HitActor);

	UFUNCTION()
	void StartTeleportTrace();

	UFUNCTION()
	void TeleportTrace(FVector StartPosition, FVector ForwardVector);

	UFUNCTION()
	void EndTeleportTrace();

	UFUNCTION()
	void TryTeleport();

	UFUNCTION()
	void SetTeleportTraceActive(bool TraceActive);

	UFUNCTION()
	bool IsAxisGreaterThanDeadzone(float AxisValue, float DeadzoneThreshold);

	TTuple<FVector, bool> IsValidTeleportLocation(FHitResult Hit);
	
	FORCEINLINE void SetOrbitDisk(AOrbitDiskParts* OrbitDiskParts) { PartsOrbitDisk = OrbitDiskParts; }
	FORCEINLINE void SetDominantEye(EDominantEye DominateEye) { DominantEye = DominateEye; }
	FORCEINLINE void SetDominantEyeTestActor(ADominateEyeTestActor* DominateEyeTestActor) { DominantEyeTestActor = DominateEyeTestActor; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }
	FORCEINLINE void SetIsHeld(bool IsHeld) { bIsHeld = IsHeld; }
	FORCEINLINE void SetCanFingerPointer(bool CanFingerPointer) { bCanFingerPointer = CanFingerPointer; }
	FORCEINLINE void SetCanPray(bool CanPray) { bCanPray = CanPray; }

	FORCEINLINE bool GetIsHeld() { return bIsHeld; }
	FORCEINLINE bool GetCanFingerPointer() { return bCanFingerPointer; }
	FORCEINLINE bool GetCanPray() { return bCanPray; }

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "GrabComponent.generated.h"

DECLARE_DELEGATE(FOnGrabbedDelegate);
DECLARE_DELEGATE(FOnDroppedDelegate);

// Custom의 경우 OnGrabbed, OnDropped 이벤트를 사용해 GrabAction에 관한 커스텀 로직을 설정할 수 있다.
// 또한 현재 사용자가 오브젝트를 쥐고 있는지 알려주는 플래그인 bIsHeld 부울 변수처럼 공개된 다른 변수를 활용할 수도 있습니다.
// 생성할 수 있는 커스텀 그랩 액션 타입의 예로는 양손 잡기, 다이얼, 레버 및 기타 복잡한 행동(예: 잡고 있으면 지오메트리가 겹치지 않는 액터)이 있습니다.
UENUM(BlueprintType)
enum class EGrabType : uint8
{
	GT_Free		UMETA(DisplayName = "None"),
	GT_Snap		UMETA(DisplayName = "Free"),
	GT_Remote	UMETA(DisplayName = "Snap"),
	GT_Custom	UMETA(DisplayName = "Custom")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), meta = (IsBlueprintBase = "true"))
class ANOTHERSTARDEMO_API UGrabComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	
	UGrabComponent();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "For Designer")
	EGrabType GrabType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UMotionControllerComponent* MotionControllerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UGrabComponent* PrimaryGrabComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FRotator PrimaryGrabRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UHapticFeedbackEffect_Base* OnGrabHapticEffect;

	// Grab이 가능한 Actor가 이미 Grabbed 상태인지를 나타낸다.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	bool bIsHeld;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	bool bSimulateOnDrop;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGrabbedEvent();

	UFUNCTION()
	void OnDroppedEvent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnGrabbedDelegate OnGrabbed;
	FOnDroppedDelegate OnDropped;

	/*
	UFUNCTION(BlueprintCallable)
	bool TryGrab(UMotionControllerComponent* MotionController);

	UFUNCTION(BlueprintCallable)
	bool TryRelease();
	*/
	UFUNCTION()
	void SetShouldSimulateOnDrop();

	UFUNCTION()
	void SetPrimitiveCompPhysics(bool bSimulate);

	UFUNCTION()
	EControllerHand GetHeldByHand();

	FORCEINLINE void SetGrabType(EGrabType Type) { GrabType = Type; }
	FORCEINLINE EGrabType GetGrabType() { return GrabType; }

};

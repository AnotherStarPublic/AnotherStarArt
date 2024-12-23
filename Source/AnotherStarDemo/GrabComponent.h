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

// Custom�� ��� OnGrabbed, OnDropped �̺�Ʈ�� ����� GrabAction�� ���� Ŀ���� ������ ������ �� �ִ�.
// ���� ���� ����ڰ� ������Ʈ�� ��� �ִ��� �˷��ִ� �÷����� bIsHeld �ο� ����ó�� ������ �ٸ� ������ Ȱ���� ���� �ֽ��ϴ�.
// ������ �� �ִ� Ŀ���� �׷� �׼� Ÿ���� ���δ� ��� ���, ���̾�, ���� �� ��Ÿ ������ �ൿ(��: ��� ������ ������Ʈ���� ��ġ�� �ʴ� ����)�� �ֽ��ϴ�.
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

	// Grab�� ������ Actor�� �̹� Grabbed ���������� ��Ÿ����.
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

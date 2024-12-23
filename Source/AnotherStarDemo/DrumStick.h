// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "DrumStick.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EDrumStickState : uint8
{
	/**
	* DrumStick은 세가지 Status 존재한다.
	* 아이템을 획득 가능한 상태 : PickUp
	* 아이템을 획득하여 아이템 슬롯에서 꺼낼 수 있는 상태 : Slot
	* 아이템을 슬롯에서 꺼내 사용하고 있는 상태 : Grabbed
	*/
	EDS_PickUp			UMETA(DisplayName = "PickUp"),
	EDS_Slot			UMETA(DisplayName = "Slot"),
	EDS_Grabbed			UMETA(DisplayName = "Grabbed")
};

UENUM(BlueprintType)
enum class ESpeedState : uint8
{
	/**
	* DrumStick은 속력에 따라 Threshold값이 3단계로 지정되어야 한다.
	* 따라서 사용하지 않는 상태 None을 포함 4가지 Status가 존재한다.
	* 너무 속력이 약해 아무런 효과를 주지 못하는 단계 : Slow
	* 속력이 빠르긴 하지만 Trigger를 발동시킬 만큼은 아닌 중간 단계 : Medium
	* Trigger를 발동시킬 수 있을 만큼 속도가 빠른 단계 : Grabbed
	*/
	ESS_None			UMETA(DisplayName = "None"),
	ESS_Slow			UMETA(DisplayName = "Slow"),
	ESS_MediumSlow		UMETA(DisplayName = "MediumSlow"),
	ESS_MediumFast		UMETA(DisplayName = "MediumFast"),
	ESS_Fast			UMETA(DisplayName = "Fast"),
};

UCLASS()
class ANOTHERSTARDEMO_API ADrumStick : public AItem
{
	GENERATED_BODY()
	
public : 
	ADrumStick();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	EDrumStickState DrumStickState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	ESpeedState SpeedState;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = __hide)
	class UBoxComponent* PercussionCollision;
	*/

	// 퍼즐을 풀기위한 조건 달성 여부를 위한 변수
	bool bIsTriggeredEvent;




protected :
	virtual void BeginPlay() override;

public : 
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DoPickUp(class AVRCharacter* Character);

	UFUNCTION()
	void Grabbed(AVRCharacter* Character);

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void PercussionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void PercussionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE void SetDrumStickState(EDrumStickState State) { DrumStickState = State; }
	FORCEINLINE void SetSpeedState(ESpeedState State) { SpeedState = State; }
	FORCEINLINE void SetIsTriggeredEvent(bool TriggeredEvent) { bIsTriggeredEvent = TriggeredEvent; }

	FORCEINLINE EDrumStickState GetDrumStickState() { return DrumStickState; }
	FORCEINLINE ESpeedState GetSpeedState() { return SpeedState; }
	FORCEINLINE bool GetIsTriggeredEvent() { return bIsTriggeredEvent; }

};

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
	* DrumStick�� ������ Status �����Ѵ�.
	* �������� ȹ�� ������ ���� : PickUp
	* �������� ȹ���Ͽ� ������ ���Կ��� ���� �� �ִ� ���� : Slot
	* �������� ���Կ��� ���� ����ϰ� �ִ� ���� : Grabbed
	*/
	EDS_PickUp			UMETA(DisplayName = "PickUp"),
	EDS_Slot			UMETA(DisplayName = "Slot"),
	EDS_Grabbed			UMETA(DisplayName = "Grabbed")
};

UENUM(BlueprintType)
enum class ESpeedState : uint8
{
	/**
	* DrumStick�� �ӷ¿� ���� Threshold���� 3�ܰ�� �����Ǿ�� �Ѵ�.
	* ���� ������� �ʴ� ���� None�� ���� 4���� Status�� �����Ѵ�.
	* �ʹ� �ӷ��� ���� �ƹ��� ȿ���� ���� ���ϴ� �ܰ� : Slow
	* �ӷ��� ������ ������ Trigger�� �ߵ���ų ��ŭ�� �ƴ� �߰� �ܰ� : Medium
	* Trigger�� �ߵ���ų �� ���� ��ŭ �ӵ��� ���� �ܰ� : Grabbed
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

	// ������ Ǯ������ ���� �޼� ���θ� ���� ����
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

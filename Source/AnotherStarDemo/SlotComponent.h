// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "SlotComponent.generated.h"

/**
 * 
 */
UCLASS()
class ANOTHERSTARDEMO_API USlotComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public : 

	USlotComponent();

	// 슬롯이 저장하고 있는 고유의 시각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Of Day")
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bIsItHorizon;

	FORCEINLINE void SetTime(float Times) { Time = Times; }
	FORCEINLINE void SetbIsItHorizon(bool IsItHorizon) { bIsItHorizon = IsItHorizon; }

	FORCEINLINE float GetTime() { return Time; }
	FORCEINLINE bool GetbIsItHorizon() { return bIsItHorizon; }
};

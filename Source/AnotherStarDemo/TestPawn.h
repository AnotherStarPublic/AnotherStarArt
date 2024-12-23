// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "HandComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TestPawn.generated.h"

/**
 * 
 */
UCLASS()
class ANOTHERSTARDEMO_API ATestPawn : public ADefaultPawn
{
	GENERATED_BODY()

public : 
	ATestPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	USceneComponent* VRTrackingCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UMotionControllerComponent* LeftController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UMotionControllerComponent* RightController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UHandComponent* LeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UHandComponent* RightHand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	class UInputMappingContext* HandsMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	class UInputMappingContext* OrbitMappingContext;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

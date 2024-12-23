// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "VRTeleportVisualizer.generated.h"

UCLASS()
class ANOTHERSTARDEMO_API AVRTeleportVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRTeleportVisualizer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UNiagaraComponent* NS_TeleportRing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UNiagaraComponent* NS_PlayAreaBounds;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	FVector GetRelativeLocation();

	UFUNCTION()
	FRotator GetRelativeRotation();
};

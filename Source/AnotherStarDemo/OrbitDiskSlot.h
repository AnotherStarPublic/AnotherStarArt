// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlotComponent.h"
#include "Components/SphereComponent.h"
#include "OrbitDiskSlot.generated.h"

UCLASS()
class ANOTHERSTARDEMO_API AOrbitDiskSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitDiskSlot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* SlotRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* DiskComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USlotComponent* SlotComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionVolume")
	class USphereComponent* SphereCollisionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bIsItNight;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

}; 

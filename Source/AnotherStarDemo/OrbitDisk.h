// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "SlotComponent.h"
#include "OrbitDisk.generated.h"


UCLASS()
class ANOTHERSTARDEMO_API AOrbitDisk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitDisk();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* PartsRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* SlotRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* PartsCollisionRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* SlotCollisionRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* DiskComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* PartsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	class USlotComponent* SlotComponent1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	USlotComponent* SlotComponent2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	USlotComponent* SlotComponent3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	USlotComponent* SlotComponent4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	USlotComponent* SlotComponent5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	USlotComponent* SlotComponent6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	USlotComponent* HorizonComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionVolume")
	class USphereComponent* PartsCollisionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionVolume")
	class USphereComponent* SlotCollisionVolume;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	class AVRCharacter* ControlCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Of Day")
	float OverlappedSlotTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsOverlappedWithSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsOverlappedWithHorizon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsItNight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void RotateParts(float Degree);

	UFUNCTION(BlueprintCallable)
	void SetLookAtRoot(UStaticMeshComponent* TargetComponent);

	UFUNCTION()
	void OnPartsComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPartsComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	FORCEINLINE void SetOverlappedSlotTime(float SlotTime) { OverlappedSlotTime = SlotTime; }

};

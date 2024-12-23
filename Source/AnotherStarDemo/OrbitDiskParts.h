// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "OrbitDiskParts.generated.h"

UCLASS()
class ANOTHERSTARDEMO_API AOrbitDiskParts : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitDiskParts();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* PartsRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* CollisionRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* PartsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionVolume")
	class USphereComponent* SphereCollisionVolume;

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
	void RotateDisk(float Degree);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE void SetOverlappedSlotTime(float SlotTime) { OverlappedSlotTime = SlotTime; }
	
	FORCEINLINE float GetOverlapeedSlotTime() { return OverlappedSlotTime; }

};

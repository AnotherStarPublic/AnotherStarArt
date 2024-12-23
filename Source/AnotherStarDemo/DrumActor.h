// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrumStick.h"
#include "GameFramework/Actor.h"
#include "DrumActor.generated.h"

UCLASS()
class ANOTHERSTARDEMO_API ADrumActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrumActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StaticMesh")
	class UStaticMeshComponent* StaticMesh;

	// HitSound�� SoundCue������ Import�ؼ� ���� �־�����Ѵ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* HitSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DrumHit(ADrumStick* DrumStick);

};

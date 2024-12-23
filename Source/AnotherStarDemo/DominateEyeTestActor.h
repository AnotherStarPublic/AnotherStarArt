// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DominateEyeTestActor.generated.h"

UCLASS()
class ANOTHERSTARDEMO_API ADominateEyeTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADominateEyeTestActor();

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UWidgetComponent* DominateEyeTestWidget;
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	class AVRCharacter* VRCharacter;

	// StaticMesh는 Widget을 사용하지 않기 위해 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	class UStaticMeshComponent* AddScoreComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UStaticMeshComponent* SubScoreComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	uint8 Score;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AddScore();

	UFUNCTION(BlueprintCallable)
	void SubScore();
};

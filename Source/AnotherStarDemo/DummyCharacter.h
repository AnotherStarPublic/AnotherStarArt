// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "DominateEyeTestActor.h"
#include "VRCharacter.h"
#include "DummyCharacter.generated.h"

UCLASS()
class ANOTHERSTARDEMO_API ADummyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADummyCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DominantEye")
	EDominantEye DominantEye;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	class UInputAction* GrabTriggerAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Actions")
	UInputAction* OpenMenuUIAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	class UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	UInputMappingContext* HandsMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls | Input Mappings")
	UInputMappingContext* UIMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	class USceneComponent* VRTrackingCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	class UCameraComponent* Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UMotionControllerComponent* LeftController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UMotionControllerComponent* RightController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	class UHandComponent* LeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = __hide)
	UHandComponent* RightHand;

	// RayCast에 닿은 Actor를 분류하기 위한 공간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitResult")
	class AActor* HitResultActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitResult")
	FHitResult HitResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DominantEye")
	class ADominateEyeTestActor* DominantEyeTestActor;

	bool bValidRayTrace;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GrabTrigger(const FInputActionValue& Value);

	void OpenMenuUI(const FInputActionValue& Value);

	void CloseMenuUI(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetDominantEye(EDominantEye DominateEye) { DominantEye = DominateEye; }
	FORCEINLINE void SetDominantEyeTestActor(ADominateEyeTestActor* DominateEyeTestActor) { DominantEyeTestActor = DominateEyeTestActor; }

};
// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ATestPawn::ATestPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRTrackingCenter = CreateDefaultSubobject<USceneComponent>(TEXT("VRTrackingCenter"));
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	LeftHand = CreateDefaultSubobject<UHandComponent>(TEXT("LeftHand"));
	RightHand = CreateDefaultSubobject<UHandComponent>(TEXT("RightHand"));

	// �� Component���� SetupAttachment�� ���� ���� ������ ����
	LeftController->SetupAttachment(GetRootComponent());
	RightController->SetupAttachment(GetRootComponent());
	LeftHand->SetupAttachment(LeftController);
	RightHand->SetupAttachment(RightController);
	// ��,�� MotionController�� ���� ��,�� ��Ʈ�ѷ��� �����ϵ��� Default���� �ο�
	LeftController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	RightController->MotionSource = FXRMotionControllerBase::RightHandSourceId;

}

void ATestPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// HMD ���� ���� Ȯ��
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// HMD ī�޶� ��ġ ����
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::LocalFloor);

		// VR �ػ� ����
		UWorld* World = GetWorld();
		if (World)
		{
			UKismetSystemLibrary::ExecuteConsoleCommand(World, TEXT("vr.PixelDensity 1.0"));
		}
	}

	// Input Mapping Context �߰�
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Priority�� ���� mapping�� ���� ����ǰ� �Է��� ����ϴ� ��� �켱 ������ ���� mapping�� ����
			Subsystem->AddMappingContext(HandsMappingContext, 0);
			Subsystem->AddMappingContext(OrbitMappingContext, 0);
		}
	}
}
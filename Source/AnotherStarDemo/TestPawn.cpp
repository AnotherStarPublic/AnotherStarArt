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

	// 각 Component들을 SetupAttachment를 통해 계층 구조를 생성
	LeftController->SetupAttachment(GetRootComponent());
	RightController->SetupAttachment(GetRootComponent());
	LeftHand->SetupAttachment(LeftController);
	RightHand->SetupAttachment(RightController);
	// 좌,우 MotionController를 실제 좌,우 컨트롤러가 수행하도록 Default값을 부여
	LeftController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	RightController->MotionSource = FXRMotionControllerBase::RightHandSourceId;

}

void ATestPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// HMD 연결 여부 확인
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// HMD 카메라 위치 설정
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::LocalFloor);

		// VR 해상도 설정
		UWorld* World = GetWorld();
		if (World)
		{
			UKismetSystemLibrary::ExecuteConsoleCommand(World, TEXT("vr.PixelDensity 1.0"));
		}
	}

	// Input Mapping Context 추가
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Priority가 높은 mapping이 먼저 적용되고 입력을 사용하는 경우 우선 순위가 낮은 mapping이 차단
			Subsystem->AddMappingContext(HandsMappingContext, 0);
			Subsystem->AddMappingContext(OrbitMappingContext, 0);
		}
	}
}
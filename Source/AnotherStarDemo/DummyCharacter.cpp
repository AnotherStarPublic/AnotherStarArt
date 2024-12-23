// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyCharacter.h"

// Sets default values
ADummyCharacter::ADummyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DominantEye = EDominantEye::DE_RightEye;

	VRTrackingCenter = CreateDefaultSubobject<USceneComponent>(TEXT("VRTrackingCenter"));
	Head = CreateDefaultSubobject<UCameraComponent>(TEXT("Head"));
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	LeftHand = CreateDefaultSubobject<UHandComponent>(TEXT("LeftHand"));
	RightHand = CreateDefaultSubobject<UHandComponent>(TEXT("RightHand"));

	VRTrackingCenter->SetupAttachment(GetRootComponent());
	Head->SetupAttachment(GetRootComponent());
	LeftController->SetupAttachment(GetRootComponent());
	RightController->SetupAttachment(GetRootComponent());
	LeftHand->SetupAttachment(LeftController);
	RightHand->SetupAttachment(RightController);

	HitResultActor = nullptr;

	bValidRayTrace = false;
	
}

// Called when the game starts or when spawned
void ADummyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// HMD 연결 여부 확인
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// HMD 카메라 위치 설정
		// 앉아서 플레이 (Eye), 서서 플레이 (Floor)B
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
			Subsystem->AddMappingContext(BaseMappingContext, 0);
			Subsystem->AddMappingContext(HandsMappingContext, 0);
			Subsystem->AddMappingContext(UIMappingContext, 0);
		}
	}
}

// Called every frame
void ADummyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 눈에서 손이 가리키는 방향으로 RayCast 
	FVector CameraLoc = Head->GetComponentLocation();

	// 오른손 검지손가락 끝 위치를 불러와서 눈에서 해당 방향으로 RayCast
	const USkeletalMeshSocket* RightHandSocket = RightHand->GetSocketByName("TracePoint");
	FVector SocketLocation = RightHandSocket->GetSocketLocation(RightHand);

	FVector FingerLoc = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(CameraLoc, SocketLocation));


	if (RightHandSocket)
	{
		// 레이저 시작 지점
		FVector StartLoc = CameraLoc;
		if (DominantEye == EDominantEye::DE_LeftEye)
		{
			StartLoc.Y -= 3.15;
		}
		else if (DominantEye == EDominantEye::DE_RightEye)
		{
			StartLoc.Y += 3.15;
		}
		// 레이저 끝나는 지점
		FVector EndLoc = FingerLoc * 500;

		// 히트 가능한 오브젝트 유형
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
		TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
		ObjectTypes.Add(WorldStatic);
		ObjectTypes.Add(WorldDynamic);

		// 무시할 액터들
		TArray<AActor*> IgnoreActors;

		bValidRayTrace = UKismetSystemLibrary::LineTraceSingleForObjects
		(
			GetWorld(),
			StartLoc,
			EndLoc,
			ObjectTypes,
			false,
			IgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
			EDrawDebugTrace::None, // 선을 표시 하지 않으려면 EDrawDebugTrace::None , 선을 표시 하려면 EDrawDebugTrace::ForDuration
			HitResult,
			true
			//  아래 3개는 기본 값으로 제공 수정 하고 싶으면 값을 지정
			//, FLinearColor::Red
			//, FLinearColor::Green
			//, 5.0f
		);

		if (bValidRayTrace)
		{
			// Hit한 지점과 HitActor의 Location을 빼면 Actor의 원점에서 Hit한 지점으로 향하는 Location이 생성
			AActor* HitActor = HitResult.GetActor();
			HitResultActor = HitActor;
			if (HitActor)
			{
				// 주시안 측정 장치와 상호작용
				// Component단위로 구별 가능하도록 추가해야함
				ADominateEyeTestActor* DominateEyeTestActor = Cast<ADominateEyeTestActor>(HitActor);
				if (DominateEyeTestActor)
				{
					// DominantEyeTestActor를 Ray로 상호작용한 대상으로 Set
					SetDominantEyeTestActor(DominateEyeTestActor);
				}
				else
				{
					SetDominantEyeTestActor(nullptr);
				}
			}
		}
	}
}

// Called to bind functionality to input
void ADummyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (GrabTriggerAction)
		{
			EnhancedInputComponent->BindAction(GrabTriggerAction, ETriggerEvent::Triggered, this, &ADummyCharacter::GrabTrigger);
			EnhancedInputComponent->BindAction(GrabTriggerAction, ETriggerEvent::Completed, this, &ADummyCharacter::GrabTrigger);
		}
		if (OpenMenuUIAction)
		{
			EnhancedInputComponent->BindAction(OpenMenuUIAction, ETriggerEvent::Triggered, this, &ADummyCharacter::OpenMenuUI);
			EnhancedInputComponent->BindAction(OpenMenuUIAction, ETriggerEvent::Triggered, this, &ADummyCharacter::CloseMenuUI);
		}
	}
}

void ADummyCharacter::GrabTrigger(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();
	
	if (bValidRayTrace)
	{
		// 핑거포인터로 주시안 측정기를 가리키고 있을 때만 트리거가 발동하도록 한다.
		if (DominantEyeTestActor)
		{
			// DominantEyeTestActor의 Component 중에서
			// AddScore와 상호작용 한다면 점수를 +
			// SubScore와 상호작용 한다면 점수를 -
			if (HitResult.GetComponent() == DominantEyeTestActor->AddScoreComponent)
			{
				DominantEyeTestActor->AddScore();
			}
			else if (HitResult.GetComponent() == DominantEyeTestActor->SubScoreComponent)
			{
				DominantEyeTestActor->SubScore();
			}
		}
	}
}

void ADummyCharacter::OpenMenuUI(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

}

void ADummyCharacter::CloseMenuUI(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

}
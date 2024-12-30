// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Components/WidgetInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OrbitDisk.h"
#include "DominateEyeTestActor.h"
#include "NiagaraSystem.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DominantEye = EDominantEye::DE_RightEye;

	TurnRateGamepad = 50.f;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	AxisDeadzoneThreshold = 0.7f;
	GrabRadiusFromGripPosition = 6.0f;

	ProjectedTeleportLocation = FVector(0.f, 0.f, 0.f);
	TeleportProjectPointToNavigationQueryExtent = FVector(0.f, 0.f, 0.f);
	
	// 캐릭터에게 필요한 Component 생성
	VRTrackingCenter = CreateDefaultSubobject<USceneComponent>(TEXT("VRTrackingCenter"));
	Head = CreateDefaultSubobject<UCameraComponent>(TEXT("Head"));
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	LeftHand = CreateDefaultSubobject<UHandComponent>(TEXT("LeftHand"));
	RightHand = CreateDefaultSubobject<UHandComponent>(TEXT("RightHand"));
	//TeleportPath = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportPath"));
	//MenuInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("DominateEyeTestWidget"));

	// 각 Component들을 SetupAttachment를 통해 계층 구조를 생성
	//SetRootComponent(VRTrackingCenter);
	VRTrackingCenter->SetupAttachment(GetRootComponent());
	Head->SetupAttachment(VRTrackingCenter);
	LeftController->SetupAttachment(VRTrackingCenter);
	RightController->SetupAttachment(VRTrackingCenter);
	LeftHand->SetupAttachment(LeftController);
	RightHand->SetupAttachment(RightController);
	// 좌,우 MotionController를 실제 좌,우 컨트롤러가 수행하도록 Default값을 부여
	LeftController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	RightController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	//MenuInteractionComponent->SetupAttachment(GetRootComponent());

	// Teleport Trace가 시작하기 전까지 Hidden 상태를 유지하기 위해 초기화
	//TeleportPath->SetVisibility(false, false);

	//static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TeleportTrace(TEXT("/Game/VFX/NS_TeleportTrace"));
	//TeleportPath->SetAsset(TeleportTrace.Object);

	// 이  Character를 Player0가 조종하도록 설정
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// 초기화
	//HeldComponentLeft = nullptr;
	//HeldComponentRight = nullptr;
	HitResultActor = nullptr;
	OrbitDisk = nullptr;
	TeleportVisualizerReference = nullptr;
	ActiveOverlappingItem = nullptr;
	
	// bool type변수 초기화
	bCanFingerPointer = false;
	bCanPray = false;
	bSnapRotation = true;
	bDoOnceTeleport = false;
	bDoOnceSnapTurn = false;
	bRightTurn = false;
	bIsHeld = false;
	bValidTeleportLocation = false;
	bTeleportTraceActive = false;
	bValidRayTrace = false;
	bGrabOrbitDisk = false;	
	bCanReverse = false;
	bPlayReverse = false;
	bTestState = false;
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	// HMD 연결 여부 확인
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// HMD 카메라 위치 설정
		// 앉아서 플레이 (Eye), 서서 플레이 (Floor)B
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Local);

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
			Subsystem->AddMappingContext(OrbitMappingContext, 0);
		}
	}
}

// 핑거 포인터의 시작지점과 끝 지점을 Set하여 Return 해주는 함수
// 눈의 위치와 손가락의 위치를 가져온다.
TArray<FVector> AVRCharacter::GetRayPoint()
{
	TArray<FVector> RayLoc;

	FVector CameraLoc = Head->GetComponentLocation();

	// 아나 ㅡㅡ 도대체 오른손 레이포인트 소켓 언제 초기화해서 지운거냐 하.. 소켓이 없으니 계속 crash 나지 아 ㅡㅡ
	const USkeletalMeshSocket* RightHandSocket = RightHand->GetSocketByName("TracePoint");
	FVector SocketLocation = RightHandSocket->GetSocketLocation(RightHand);

	CameraLoc.Y -= 5.5;

	/*

	if (DominantEye == EDominantEye::DE_LeftEye)
	{
		CameraLoc.Y -= 3.15;
	}
	else if (DominantEye == EDominantEye::DE_RightEye)
	{
		CameraLoc.Y += 3.15;
	}

	*/

	FVector FingerLoc = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(CameraLoc, SocketLocation));

	RayLoc.Add(CameraLoc);
	RayLoc.Add(FingerLoc * 500000000);

	return RayLoc;

}

FHitResult AVRCharacter::GetRayHitResult(FVector StartLoc, FVector EndLoc)
{
	// 히트 가능한 오브젝트 유형
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
	ObjectTypes.Add(WorldStatic);
	ObjectTypes.Add(WorldDynamic);

	// 무시할 액터들
	TArray<AActor*> IgnoreActors;

	// 히트 결과 값 받을 변수
	FHitResult HitResult;

	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bValidRayTrace = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLoc,
		EndLoc,
		ECollisionChannel::ECC_GameTraceChannel2,
		QueryParams
	);
	
	FColor LineColor = bValidRayTrace ? FColor::Green : FColor::Red;

	DrawDebugLine(GetWorld(), StartLoc, EndLoc, LineColor, false);

	/*
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLoc,
		EndLoc,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration, // 선을 표시 하지 않으려면 EDrawDebugTrace::None , 선을 표시 하려면 EDrawDebugTrace::ForDuration
		HitResult,
		true
	);
	

	// RayCast 수행
	bValidRayTrace = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		StartLoc,
		EndLoc,
		ObjectTypes,
		false,
		IgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::ForDuration, // 선을 표시 하지 않으려면 EDrawDebugTrace::None , 선을 표시 하려면 EDrawDebugTrace::ForDuration
		HitResult,
		true
		//  아래 3개는 기본 값으로 제공 수정 하고 싶으면 값을 지정
		//, FLinearColor::Red
		//, FLinearColor::Green
		//, 5.0f
	);
	*/

	return HitResult;

}

void AVRCharacter::SetRayOrbitDisk(AActor* HitActor)
{
	if (HitActor)
	{
		// 발사된 RayCast가 OrbitDisk면 Cast할 수 있도록 제어하고 만약 OrbitDisk가 Cast에 성공하면 Character의 TimeOrbitDisk를 Set
		AOrbitDisk* OrbitDiskActor = Cast<AOrbitDisk>(HitActor);
		if (OrbitDiskActor)
		{
			SetOrbitDisk(OrbitDiskActor);
		}
		else
		{
			SetOrbitDisk(nullptr);
		}
	}
}

bool AVRCharacter::IsCounterClockwise(const FVector2D& Vector1, const FVector2D& Vector2)
{
	return UKismetMathLibrary::CrossProduct2D(Vector1, Vector2) > 0;
}

float AVRCharacter::CalculateRotateDiskAngle(FHitResult HitResult, AActor* HitActor)
{
	float Angle = 0;

	FVector DirectionalVector = (HitResult.Location) - (HitActor->GetActorLocation());

	// Add와 같은 기능이지만 Emplace의 경우 임시 변수를 생성하지 않음
	DV_Array.Add(DirectionalVector);
	// 벡터 2개를 이용해 Degree를 구해야 하기 때문에 Array에 Element가 둘 이상일 때 Pop
	// 마찬가지로 2개일 때 부터 연산이 가능하기 때문에 이때 부터 연산하여 각도 계산
	if (DV_Array.Num() == 2)
	{
		FVector DegreeVector1 = DV_Array[0];
		FVector DegreeVector2 = DV_Array[1];
		// 벡터 외적에서는 벡터의 방향이 중요하기 때문에 두번째 값에 대해서는 역벡터를 취해야 맞는 구조를 갖음
		FVector InverseVector = DegreeVector2 * (-1);

		// CCW(Counter ClockWise)를 활용하기 위해 2차원 벡터로 치환
		// 정확한 값은 필요 없고 양수, 음수만 판별하면 되기 때문에 Z축을 그냥 날려버리는 방식으로 단순 치환
		FVector2D CrossProductVector1 = FVector2D(DegreeVector1.X, DegreeVector1.Y);
		FVector2D CrossProductVector2 = FVector2D(InverseVector.X, InverseVector.Y);
		// 두 벡터의 사이각 계산
		// θ = cos^(-1) { (v1 dot v2) / (|| v1 || * || v2 ||) }
		float AbsAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::Normal(DegreeVector1, 0.0001), UKismetMathLibrary::Normal(DegreeVector2, 0.0001)));

		if (IsCounterClockwise(CrossProductVector1, CrossProductVector2))
		{
			Angle = AbsAngle;
		}
		else
		{
			Angle = -AbsAngle;
		}

		// 배열의 첫번 째 Element를 제거해서 다음 계산을 준비
		// TArray는 공백이 있을 수 없기 때문에 0번을 제거해도 1번 Element가 0번으로 옮겨짐
		DV_Array.RemoveAt(0);
	}

	return Angle;
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCanFingerPointer)
	{
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Tick Function running")));

	TArray<FVector> RayLoc = GetRayPoint();
	FHitResult HitResult = GetRayHitResult(RayLoc[0], RayLoc[1]);
	AActor* HitActor = HitResult.GetActor();
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Hit Actor Name: %s"), *HitActor->GetName()));
	
	if (bValidRayTrace)
	{
		SetRayOrbitDisk(HitActor);

		float RotateAngle = CalculateRotateDiskAngle(HitResult, HitActor);

		if (bGrabOrbitDisk && OrbitDisk)
		{
			OrbitDisk->RotateParts(RotateAngle);
		}


	}
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MovementAction)
		{
			//EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AVRCharacter::Movement);
		}
		/* 회전 관련 기능 UnBinding
		* Blueprint를 활용해 기능 구현 및 Binding 할 예정
		if (RightGrabTriggerAction)
		{
			EnhancedInputComponent->BindAction(RightGrabTriggerAction, ETriggerEvent::Started, this, &AVRCharacter::RightTrigger);
			EnhancedInputComponent->BindAction(RightGrabTriggerAction, ETriggerEvent::Completed, this, &AVRCharacter::RightRelease);
		}
		
		if (TurnAction && !bSnapRotation)
		{
			EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AVRCharacter::Turn);
		}
		if (SnapTurnAction && bSnapRotation)
		{
			EnhancedInputComponent->BindAction(SnapTurnAction, ETriggerEvent::Started, this, &AVRCharacter::SnapTurn);
		}
		*/
		/* 그랩 관련 기능 UnBinding
		*  Blueprint를 활용해 기능 구현 및 Binding 할 예정
		if (LeftGrabTriggerAction)
		{
			EnhancedInputComponent->BindAction(LeftGrabTriggerAction, ETriggerEvent::Started, this, &AVRCharacter::LeftGrabTrigger);
			EnhancedInputComponent->BindAction(LeftGrabTriggerAction, ETriggerEvent::Completed, this, &AVRCharacter::LeftGrabRelease);
		}
		if (RightGrabTriggerAction)
		{
			EnhancedInputComponent->BindAction(RightGrabTriggerAction, ETriggerEvent::Started, this, &AVRCharacter::RightGrabTrigger);
			EnhancedInputComponent->BindAction(RightGrabTriggerAction, ETriggerEvent::Completed, this, &AVRCharacter::RightGrabRelease);
		}
		*/
		if (TeleportAction)
		{
			EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Triggered, this, &AVRCharacter::Teleport);
			EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Completed, this, &AVRCharacter::Teleport);
		}
		if (ResetViewAction)
		{
			EnhancedInputComponent->BindAction(ResetViewAction, ETriggerEvent::Triggered, this, &AVRCharacter::ResetView);
		}
		if (OpenStatusUIAction)
		{
			EnhancedInputComponent->BindAction(OpenStatusUIAction, ETriggerEvent::Triggered, this, &AVRCharacter::OpenStatusUI);
			EnhancedInputComponent->BindAction(OpenStatusUIAction, ETriggerEvent::Triggered, this, &AVRCharacter::CloseStatusUI);
		}
		if (OpenMenuUIAction)
		{
			EnhancedInputComponent->BindAction(OpenMenuUIAction, ETriggerEvent::Triggered, this, &AVRCharacter::OpenMenuUI);
			EnhancedInputComponent->BindAction(OpenMenuUIAction, ETriggerEvent::Triggered, this, &AVRCharacter::CloseMenuUI);
		}
	}
}

void AVRCharacter::Movement(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);		
	}
}

// 회전 관련 함수는 Binding 하지 않고 현재 시점에선 Blueprint로 구현 및 Binding
void AVRCharacter::Turn(const FInputActionValue& Value)
{
	float LookAxisValue = Value.Get<float>();

	if (Controller != nullptr)
	{
		if (IsAxisGreaterThanDeadzone(LookAxisValue, AxisDeadzoneThreshold))
		{
			AddControllerYawInput(LookAxisValue);
		}
	}
}

// 회전 관련 함수는 Binding 하지 않고 현재 시점에선 Blueprint로 구현 및 Binding
void AVRCharacter::SnapTurn(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();

	if (bSnapRotation)
	{
		if (IsAxisGreaterThanDeadzone(AxisValue, AxisDeadzoneThreshold))
		{
			if (!bDoOnceSnapTurn)
			{
				float SnapTurnDegree = -18.f;
				
				float LookAxisValue = UKismetMathLibrary::SelectFloat(UKismetMathLibrary::Abs(SnapTurnDegree), SnapTurnDegree, bRightTurn);
				AddControllerYawInput(LookAxisValue);
				bDoOnceSnapTurn = true;
			}
		}
		else
		{
			bDoOnceSnapTurn = false;
		}
	}
}

void AVRCharacter::RightTrigger(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

	UE_LOG(LogTemp, Warning, TEXT("PressPressPressPressPressPress"));

	if (bValidRayTrace)
	{
		if (OrbitDisk)
		{
			bGrabOrbitDisk = bIsTriggered;
		}
	}
	else
	{
		bGrabOrbitDisk = false;
	}

}

void AVRCharacter::RightRelease(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

	UE_LOG(LogTemp, Warning, TEXT("ReleaseReleaseReleaseReleaseRelease"));

	if (OrbitDisk)
	{
		if (OrbitDisk->bIsOverlappedWithSlot)
		{
			
		}
	}
}


/*
void AVRCharacter::LeftGrabTrigger(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

	UGrabComponent* NearestComponent = this->GetGrabComponentNearMotionController(LeftController);

	if (NearestComponent != nullptr)
	{
		bIsHeld = NearestComponent->TryGrab(LeftController);

		if (bIsHeld)
		{
			HeldComponentLeft = NearestComponent;

			// 반대 손에서 해당 GrabComponent를 들고있으면 반대 손의 참조를 제거한다.
			if (HeldComponentLeft == HeldComponentRight)
			{
				HeldComponentRight = nullptr;
			}
		}
	}
}

void AVRCharacter::LeftGrabRelease(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

	if (HeldComponentLeft != nullptr)
	{
		bool bReleased = HeldComponentLeft->TryRelease();

		if (bReleased)
		{
			HeldComponentLeft = nullptr;
			bIsHeld = false;
		}
	}
}

void AVRCharacter::RightGrabTrigger(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

	//OrbitDisk를 돌리는 시스템
	if (bValidRayTrace)
	{
		if (PartsOrbitDisk)
		{
			bGrabOrbitDisk = bIsTriggered;
		}
	}
	else
	{
		bGrabOrbitDisk = false;
	}


	// Item 획득
	if (ActiveOverlappingItem)
	{
		ADrumStick* DrumStick = Cast<ADrumStick>(ActiveOverlappingItem);
		if (DrumStick)
		{
			DrumStick->DoPickUp(this);
		}
	}

	UGrabComponent* NearestComponent = this->GetGrabComponentNearMotionController(RightController);

	if (NearestComponent != nullptr)
	{
		bIsHeld = NearestComponent->TryGrab(RightController);

		if (bIsHeld)
		{
			HeldComponentRight = NearestComponent;

			if (HeldComponentRight == HeldComponentLeft)
			{
				HeldComponentLeft = nullptr;
			}
		}
	}


}


void AVRCharacter::RightGrabRelease(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

	if (HeldComponentRight != nullptr)
	{
		bool bReleased = HeldComponentRight->TryRelease();

		if (bReleased)
		{
			HeldComponentRight = nullptr;
			bIsHeld = false;
		}
	}
}
*/

void AVRCharacter::ResetView(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();
	
	if (bIsTriggered)
	{
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
	}
}

/*
UGrabComponent* AVRCharacter::GetGrabComponentNearMotionController(class UMotionControllerComponent* MotionController) const
{
	// LocalVariable 선언
	UGrabComponent* LocalNearestGrabComponent = nullptr;
	TArray<UGrabComponent*> LocalGrabComponents;
	FVector LocalGripPosition = FVector(0, 0, 0);
	float LocalNearestComponentDistance = 9999999.f;
	
	LocalGripPosition = MotionController->GetComponentLocation();

	// "Grabbable" 채널을 사용하면 좋지만
	// Default값 중에 잡을 수 없는 objects에 WorldDynamic이 사용 될 수 있으므로
	// ECC_PhysicsBody를 사용한다
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
	ObjectTypes.Add(PhysicsBody);

	TArray<AActor*> ActorsToNotTargeting;
	FHitResult HitResult;

	// 타게팅 시스템으로 Ray를 Sphere형태로 쏘는 것.
	bool bIsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		LocalGripPosition,
		LocalGripPosition,
		GrabRadiusFromGripPosition,
		ObjectTypes,
		false,
		ActorsToNotTargeting,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Blue,
		FLinearColor::Green,
		1.0f
	);

	// 이 class에 GrabComponent가 존재하는지 검사
	AActor* HitActor = HitResult.GetActor();
	// LocalGrabComponents에 HitActor를 불러와 Set
	HitActor->GetComponents(LocalGrabComponents);
	
	if (bIsHit)
	{
		float ArrayLength = LocalGrabComponents.Num();

		if (ArrayLength > 0)
		{
			// 어떤 GrabComponent가 가장 가까이 있는지 결정한다.
			for (int32 ComponentIdx = 0; ComponentIdx < ArrayLength; ++ComponentIdx)
			{
				float VectorLengthSquared = UKismetMathLibrary::VSizeSquared(LocalGrabComponents[ComponentIdx]->GetComponentLocation() - LocalGripPosition);

				if (VectorLengthSquared <= LocalNearestComponentDistance)
				{
					LocalNearestComponentDistance = VectorLengthSquared;
					LocalNearestGrabComponent = LocalGrabComponents[ComponentIdx];
				}
			}
		}
	}

	return LocalNearestGrabComponent;
}

void AVRCharacter::TakeItem(AItem* Item)
{
	if(Item->GetItemType() == EItemType::IT_UnConsumable)
	{
		ADrumStick* DrumStick = Cast<ADrumStick>(Item);
		if (DrumStick)
		{
			ConsumableItemSlot.Emplace(DrumStick);
			DrumStick->SetDrumStickState(EDrumStickState::EDS_Slot);
			DrumStick->Destroy();
		}
	}
}
*/
void AVRCharacter::Teleport(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();

	// ThumbStick이 일정 수준 이상으로 확실하게 입력되어야 작동하도록 함
	// 양수 값의 입력에만 작동하도록 함 (ThumbStick Y 의 경우 위로 밀었을 경우에만 작동)
	if (IsAxisGreaterThanDeadzone(AxisValue, AxisDeadzoneThreshold) && (AxisValue > 0.f))
	{
		// 입력 값이 주어졌을 때 한번만 작동하도록 제어
		if (!bDoOnceTeleport)
		{
			StartTeleportTrace();
			bDoOnceTeleport = true;
		}
		TeleportTrace(RightController->GetComponentLocation(), RightController->GetForwardVector());
	}
	else
	{
		// Teleport Trace가 활성화된 상태에서만 수행되도록 제어
		if (bTeleportTraceActive)
		{
			EndTeleportTrace();
			TryTeleport();

			bDoOnceTeleport = false;
		}
	}
}

void AVRCharacter::StartTeleportTrace()
{
	// TeleportTrace 스위치 활성화 및 시각화
	SetTeleportTraceActive(true);
	//TeleportPath->SetVisibility(true, false);
	 
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		FRotator Rotator;
		FVector SpawnLocation = FVector::ZeroVector;

		// TeleportVisualizer를 Spawn
		TeleportVisualizerReference = World->SpawnActor<AVRTeleportVisualizer>(AVRTeleportVisualizer::StaticClass(), SpawnLocation, Rotator, SpawnParams);
	}
}

void AVRCharacter::TeleportTrace(FVector StartPosition, FVector ForwardVector)
{
	float TeleportLaunchSpeed = 650.f;
	FVector LaunchVelocity = UKismetMathLibrary::Multiply_VectorVector(UKismetMathLibrary::Conv_DoubleToVector(TeleportLaunchSpeed), ForwardVector);

	float TeleportProjectileRadius = 3.6f;

	FHitResult HitResult;
	TArray<FVector> OutPathPositions;
	FVector OutLastTraceDestination;

	// 플레이어가 이동할 수 있는 객체 유형
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> IgnoreActors;

	// 맨 아래 파라미터 값으로 주어진 0.0f 는 DebugDrawTime으로 숫자를 늘려 Debugging에 활용
	// Predict ProjectilePath 함수를 사용해서 포물선을 생성
	UGameplayStatics::Blueprint_PredictProjectilePath_ByObjectType
	(
		GetWorld(),
		HitResult,
		OutPathPositions,
		OutLastTraceDestination,
		StartPosition,
		LaunchVelocity,
		true,
		TeleportProjectileRadius,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		0.0f
	);

	// Teleport Trace Niagara System을 위해 사용할 Trace Path Position을 저장
	TArray<FVector> TeleportTracePathPositions = OutPathPositions;
	TeleportTracePathPositions.Insert(StartPosition, 0);

	// Teleport할 위치가 유효한지 검사해서 유효하다면 해당 Location과 bool타입의 true를 return
	// 유효하지 않다면 Zero Vector와 false를 return
	TTuple<FVector, bool> HitValues = IsValidTeleportLocation(HitResult);
	float LocalNavMeshCellHeight = 8.f;

	// 투영된 텔레포트 위치를 설정
	// Navigation Mesh의 높이에 따라 이를 상쇄하고 cell 높이를 변경할 경우에도 일치하도록 이 값을 조정
	
	ProjectedTeleportLocation = UKismetMathLibrary::MakeVector(HitValues.Get<0>().X, HitValues.Get<0>().Y, (HitValues.Get<0>().Z - LocalNavMeshCellHeight));

	if (bValidTeleportLocation != HitValues.Get<1>())
	{
		bValidTeleportLocation = HitValues.Get<1>();
	}
	
	// 유효한 텔레포트의 위치 및 텔레포트를 시각화
	TeleportVisualizerReference->GetRootComponent()->SetVisibility(bValidTeleportLocation, true);

	// TeleportVisualizer의 위치를 Update
	// 5.4 Migration
	//TeleportVisualizerReference->SetActorLocation(ProjectedTeleportLocation, false, false);

	// VFX - Teleport Trace VFX를 위한 Niagara System에서 Vector Array를 설정
	//UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportPath, "User.PointArray", TeleportTracePathPositions);
}

void AVRCharacter::EndTeleportTrace()
{
	SetTeleportTraceActive(false);

	// Teleport가 끝난 시점에 TeleportVisualizerRefernce Actor가 유효하다면 Destory
	if (IsValid(TeleportVisualizerReference))
	{
		TeleportVisualizerReference->Destroy();
	}

	// TeleportPath를 Hidden 상태로 변경
	//TeleportPath->SetVisibility(false, false);
}

void AVRCharacter::TryTeleport()
{
	if (bValidTeleportLocation)
	{
		bValidTeleportLocation = false;

		// 올바른 Pawn Destination에 대해 HMD(Camera)를 sub
		FVector DestinationLocation = UKismetMathLibrary::Subtract_VectorVector(
			ProjectedTeleportLocation,
			UKismetMathLibrary::GreaterGreater_VectorRotator(
				UKismetMathLibrary::MakeVector(Head->GetRelativeLocation().X, Head->GetRelativeLocation().Y, 0.f),
				FRotator(0.f, GetActorRotation().Yaw, 0.f)
			)
		);
		// Input값에 넣은 값으로 Teleport
		this->TeleportTo(DestinationLocation, FRotator(0.f, GetActorRotation().Yaw, 0.f), false, true);
	}
}

void AVRCharacter::SetTeleportTraceActive(bool TraceActive)
{
	if (!bTeleportTraceActive)
	{
		bTeleportTraceActive = TraceActive;
	}
}

void AVRCharacter::OpenStatusUI(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

}

void AVRCharacter::CloseStatusUI(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

}

void AVRCharacter::OpenMenuUI(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

}

void AVRCharacter::CloseMenuUI(const FInputActionValue& Value)
{
	bool bIsTriggered = Value.Get<bool>();

}

bool AVRCharacter::IsAxisGreaterThanDeadzone(float AxisValue, float DeadzoneThreshold)
{
	return UKismetMathLibrary::Abs(AxisValue) > DeadzoneThreshold;
}

TTuple<FVector, bool> AVRCharacter::IsValidTeleportLocation(FHitResult Hit)
{
	UWorld* World = GetWorld();

	FVector Location = FVector(0.f);
	bool bNavResult = false;

	if (World)
	{
		World->GetNavigationSystem();
		const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(this);

		if (NavSystem)
		{
			FNavLocation ProjectedLocation;

			bNavResult = UNavigationSystemV1::GetNavigationSystem(World)->ProjectPointToNavigation(Hit.Location, ProjectedLocation, TeleportProjectPointToNavigationQueryExtent);
			Location = ProjectedLocation.Location;
		}
	}
	
	return MakeTuple(Location, bNavResult);
}
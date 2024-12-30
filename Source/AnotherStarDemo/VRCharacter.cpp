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
	
	// ĳ���Ϳ��� �ʿ��� Component ����
	VRTrackingCenter = CreateDefaultSubobject<USceneComponent>(TEXT("VRTrackingCenter"));
	Head = CreateDefaultSubobject<UCameraComponent>(TEXT("Head"));
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	LeftHand = CreateDefaultSubobject<UHandComponent>(TEXT("LeftHand"));
	RightHand = CreateDefaultSubobject<UHandComponent>(TEXT("RightHand"));
	//TeleportPath = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportPath"));
	//MenuInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("DominateEyeTestWidget"));

	// �� Component���� SetupAttachment�� ���� ���� ������ ����
	//SetRootComponent(VRTrackingCenter);
	VRTrackingCenter->SetupAttachment(GetRootComponent());
	Head->SetupAttachment(VRTrackingCenter);
	LeftController->SetupAttachment(VRTrackingCenter);
	RightController->SetupAttachment(VRTrackingCenter);
	LeftHand->SetupAttachment(LeftController);
	RightHand->SetupAttachment(RightController);
	// ��,�� MotionController�� ���� ��,�� ��Ʈ�ѷ��� �����ϵ��� Default���� �ο�
	LeftController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	RightController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	//MenuInteractionComponent->SetupAttachment(GetRootComponent());

	// Teleport Trace�� �����ϱ� ������ Hidden ���¸� �����ϱ� ���� �ʱ�ȭ
	//TeleportPath->SetVisibility(false, false);

	//static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TeleportTrace(TEXT("/Game/VFX/NS_TeleportTrace"));
	//TeleportPath->SetAsset(TeleportTrace.Object);

	// ��  Character�� Player0�� �����ϵ��� ����
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// �ʱ�ȭ
	//HeldComponentLeft = nullptr;
	//HeldComponentRight = nullptr;
	HitResultActor = nullptr;
	OrbitDisk = nullptr;
	TeleportVisualizerReference = nullptr;
	ActiveOverlappingItem = nullptr;
	
	// bool type���� �ʱ�ȭ
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

	// HMD ���� ���� Ȯ��
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// HMD ī�޶� ��ġ ����
		// �ɾƼ� �÷��� (Eye), ���� �÷��� (Floor)B
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Local);

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
			Subsystem->AddMappingContext(BaseMappingContext, 0);
			Subsystem->AddMappingContext(HandsMappingContext, 0);
			Subsystem->AddMappingContext(OrbitMappingContext, 0);
		}
	}
}

// �ΰ� �������� ���������� �� ������ Set�Ͽ� Return ���ִ� �Լ�
// ���� ��ġ�� �հ����� ��ġ�� �����´�.
TArray<FVector> AVRCharacter::GetRayPoint()
{
	TArray<FVector> RayLoc;

	FVector CameraLoc = Head->GetComponentLocation();

	// �Ƴ� �Ѥ� ����ü ������ ��������Ʈ ���� ���� �ʱ�ȭ�ؼ� ����ų� ��.. ������ ������ ��� crash ���� �� �Ѥ�
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
	// ��Ʈ ������ ������Ʈ ����
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
	ObjectTypes.Add(WorldStatic);
	ObjectTypes.Add(WorldDynamic);

	// ������ ���͵�
	TArray<AActor*> IgnoreActors;

	// ��Ʈ ��� �� ���� ����
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
		EDrawDebugTrace::ForDuration, // ���� ǥ�� ���� �������� EDrawDebugTrace::None , ���� ǥ�� �Ϸ��� EDrawDebugTrace::ForDuration
		HitResult,
		true
	);
	

	// RayCast ����
	bValidRayTrace = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		StartLoc,
		EndLoc,
		ObjectTypes,
		false,
		IgnoreActors, // ������ ���� ���ٰ��ص� null�� ���� �� ����.
		EDrawDebugTrace::ForDuration, // ���� ǥ�� ���� �������� EDrawDebugTrace::None , ���� ǥ�� �Ϸ��� EDrawDebugTrace::ForDuration
		HitResult,
		true
		//  �Ʒ� 3���� �⺻ ������ ���� ���� �ϰ� ������ ���� ����
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
		// �߻�� RayCast�� OrbitDisk�� Cast�� �� �ֵ��� �����ϰ� ���� OrbitDisk�� Cast�� �����ϸ� Character�� TimeOrbitDisk�� Set
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

	// Add�� ���� ��������� Emplace�� ��� �ӽ� ������ �������� ����
	DV_Array.Add(DirectionalVector);
	// ���� 2���� �̿��� Degree�� ���ؾ� �ϱ� ������ Array�� Element�� �� �̻��� �� Pop
	// ���������� 2���� �� ���� ������ �����ϱ� ������ �̶� ���� �����Ͽ� ���� ���
	if (DV_Array.Num() == 2)
	{
		FVector DegreeVector1 = DV_Array[0];
		FVector DegreeVector2 = DV_Array[1];
		// ���� ���������� ������ ������ �߿��ϱ� ������ �ι�° ���� ���ؼ��� �����͸� ���ؾ� �´� ������ ����
		FVector InverseVector = DegreeVector2 * (-1);

		// CCW(Counter ClockWise)�� Ȱ���ϱ� ���� 2���� ���ͷ� ġȯ
		// ��Ȯ�� ���� �ʿ� ���� ���, ������ �Ǻ��ϸ� �Ǳ� ������ Z���� �׳� ���������� ������� �ܼ� ġȯ
		FVector2D CrossProductVector1 = FVector2D(DegreeVector1.X, DegreeVector1.Y);
		FVector2D CrossProductVector2 = FVector2D(InverseVector.X, InverseVector.Y);
		// �� ������ ���̰� ���
		// �� = cos^(-1) { (v1 dot v2) / (|| v1 || * || v2 ||) }
		float AbsAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::Normal(DegreeVector1, 0.0001), UKismetMathLibrary::Normal(DegreeVector2, 0.0001)));

		if (IsCounterClockwise(CrossProductVector1, CrossProductVector2))
		{
			Angle = AbsAngle;
		}
		else
		{
			Angle = -AbsAngle;
		}

		// �迭�� ù�� ° Element�� �����ؼ� ���� ����� �غ�
		// TArray�� ������ ���� �� ���� ������ 0���� �����ص� 1�� Element�� 0������ �Ű���
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
		/* ȸ�� ���� ��� UnBinding
		* Blueprint�� Ȱ���� ��� ���� �� Binding �� ����
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
		/* �׷� ���� ��� UnBinding
		*  Blueprint�� Ȱ���� ��� ���� �� Binding �� ����
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

// ȸ�� ���� �Լ��� Binding ���� �ʰ� ���� �������� Blueprint�� ���� �� Binding
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

// ȸ�� ���� �Լ��� Binding ���� �ʰ� ���� �������� Blueprint�� ���� �� Binding
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

			// �ݴ� �տ��� �ش� GrabComponent�� ��������� �ݴ� ���� ������ �����Ѵ�.
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

	//OrbitDisk�� ������ �ý���
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


	// Item ȹ��
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
	// LocalVariable ����
	UGrabComponent* LocalNearestGrabComponent = nullptr;
	TArray<UGrabComponent*> LocalGrabComponents;
	FVector LocalGripPosition = FVector(0, 0, 0);
	float LocalNearestComponentDistance = 9999999.f;
	
	LocalGripPosition = MotionController->GetComponentLocation();

	// "Grabbable" ä���� ����ϸ� ������
	// Default�� �߿� ���� �� ���� objects�� WorldDynamic�� ��� �� �� �����Ƿ�
	// ECC_PhysicsBody�� ����Ѵ�
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
	ObjectTypes.Add(PhysicsBody);

	TArray<AActor*> ActorsToNotTargeting;
	FHitResult HitResult;

	// Ÿ���� �ý������� Ray�� Sphere���·� ��� ��.
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

	// �� class�� GrabComponent�� �����ϴ��� �˻�
	AActor* HitActor = HitResult.GetActor();
	// LocalGrabComponents�� HitActor�� �ҷ��� Set
	HitActor->GetComponents(LocalGrabComponents);
	
	if (bIsHit)
	{
		float ArrayLength = LocalGrabComponents.Num();

		if (ArrayLength > 0)
		{
			// � GrabComponent�� ���� ������ �ִ��� �����Ѵ�.
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

	// ThumbStick�� ���� ���� �̻����� Ȯ���ϰ� �ԷµǾ�� �۵��ϵ��� ��
	// ��� ���� �Է¿��� �۵��ϵ��� �� (ThumbStick Y �� ��� ���� �о��� ��쿡�� �۵�)
	if (IsAxisGreaterThanDeadzone(AxisValue, AxisDeadzoneThreshold) && (AxisValue > 0.f))
	{
		// �Է� ���� �־����� �� �ѹ��� �۵��ϵ��� ����
		if (!bDoOnceTeleport)
		{
			StartTeleportTrace();
			bDoOnceTeleport = true;
		}
		TeleportTrace(RightController->GetComponentLocation(), RightController->GetForwardVector());
	}
	else
	{
		// Teleport Trace�� Ȱ��ȭ�� ���¿����� ����ǵ��� ����
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
	// TeleportTrace ����ġ Ȱ��ȭ �� �ð�ȭ
	SetTeleportTraceActive(true);
	//TeleportPath->SetVisibility(true, false);
	 
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		FRotator Rotator;
		FVector SpawnLocation = FVector::ZeroVector;

		// TeleportVisualizer�� Spawn
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

	// �÷��̾ �̵��� �� �ִ� ��ü ����
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> IgnoreActors;

	// �� �Ʒ� �Ķ���� ������ �־��� 0.0f �� DebugDrawTime���� ���ڸ� �÷� Debugging�� Ȱ��
	// Predict ProjectilePath �Լ��� ����ؼ� �������� ����
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

	// Teleport Trace Niagara System�� ���� ����� Trace Path Position�� ����
	TArray<FVector> TeleportTracePathPositions = OutPathPositions;
	TeleportTracePathPositions.Insert(StartPosition, 0);

	// Teleport�� ��ġ�� ��ȿ���� �˻��ؼ� ��ȿ�ϴٸ� �ش� Location�� boolŸ���� true�� return
	// ��ȿ���� �ʴٸ� Zero Vector�� false�� return
	TTuple<FVector, bool> HitValues = IsValidTeleportLocation(HitResult);
	float LocalNavMeshCellHeight = 8.f;

	// ������ �ڷ���Ʈ ��ġ�� ����
	// Navigation Mesh�� ���̿� ���� �̸� ����ϰ� cell ���̸� ������ ��쿡�� ��ġ�ϵ��� �� ���� ����
	
	ProjectedTeleportLocation = UKismetMathLibrary::MakeVector(HitValues.Get<0>().X, HitValues.Get<0>().Y, (HitValues.Get<0>().Z - LocalNavMeshCellHeight));

	if (bValidTeleportLocation != HitValues.Get<1>())
	{
		bValidTeleportLocation = HitValues.Get<1>();
	}
	
	// ��ȿ�� �ڷ���Ʈ�� ��ġ �� �ڷ���Ʈ�� �ð�ȭ
	TeleportVisualizerReference->GetRootComponent()->SetVisibility(bValidTeleportLocation, true);

	// TeleportVisualizer�� ��ġ�� Update
	// 5.4 Migration
	//TeleportVisualizerReference->SetActorLocation(ProjectedTeleportLocation, false, false);

	// VFX - Teleport Trace VFX�� ���� Niagara System���� Vector Array�� ����
	//UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportPath, "User.PointArray", TeleportTracePathPositions);
}

void AVRCharacter::EndTeleportTrace()
{
	SetTeleportTraceActive(false);

	// Teleport�� ���� ������ TeleportVisualizerRefernce Actor�� ��ȿ�ϴٸ� Destory
	if (IsValid(TeleportVisualizerReference))
	{
		TeleportVisualizerReference->Destroy();
	}

	// TeleportPath�� Hidden ���·� ����
	//TeleportPath->SetVisibility(false, false);
}

void AVRCharacter::TryTeleport()
{
	if (bValidTeleportLocation)
	{
		bValidTeleportLocation = false;

		// �ùٸ� Pawn Destination�� ���� HMD(Camera)�� sub
		FVector DestinationLocation = UKismetMathLibrary::Subtract_VectorVector(
			ProjectedTeleportLocation,
			UKismetMathLibrary::GreaterGreater_VectorRotator(
				UKismetMathLibrary::MakeVector(Head->GetRelativeLocation().X, Head->GetRelativeLocation().Y, 0.f),
				FRotator(0.f, GetActorRotation().Yaw, 0.f)
			)
		);
		// Input���� ���� ������ Teleport
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
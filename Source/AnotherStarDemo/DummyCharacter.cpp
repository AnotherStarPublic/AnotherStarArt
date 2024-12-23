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
	
	// HMD ���� ���� Ȯ��
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// HMD ī�޶� ��ġ ����
		// �ɾƼ� �÷��� (Eye), ���� �÷��� (Floor)B
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

	// ������ ���� ����Ű�� �������� RayCast 
	FVector CameraLoc = Head->GetComponentLocation();

	// ������ �����հ��� �� ��ġ�� �ҷ��ͼ� ������ �ش� �������� RayCast
	const USkeletalMeshSocket* RightHandSocket = RightHand->GetSocketByName("TracePoint");
	FVector SocketLocation = RightHandSocket->GetSocketLocation(RightHand);

	FVector FingerLoc = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(CameraLoc, SocketLocation));


	if (RightHandSocket)
	{
		// ������ ���� ����
		FVector StartLoc = CameraLoc;
		if (DominantEye == EDominantEye::DE_LeftEye)
		{
			StartLoc.Y -= 3.15;
		}
		else if (DominantEye == EDominantEye::DE_RightEye)
		{
			StartLoc.Y += 3.15;
		}
		// ������ ������ ����
		FVector EndLoc = FingerLoc * 500;

		// ��Ʈ ������ ������Ʈ ����
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
		TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
		ObjectTypes.Add(WorldStatic);
		ObjectTypes.Add(WorldDynamic);

		// ������ ���͵�
		TArray<AActor*> IgnoreActors;

		bValidRayTrace = UKismetSystemLibrary::LineTraceSingleForObjects
		(
			GetWorld(),
			StartLoc,
			EndLoc,
			ObjectTypes,
			false,
			IgnoreActors, // ������ ���� ���ٰ��ص� null�� ���� �� ����.
			EDrawDebugTrace::None, // ���� ǥ�� ���� �������� EDrawDebugTrace::None , ���� ǥ�� �Ϸ��� EDrawDebugTrace::ForDuration
			HitResult,
			true
			//  �Ʒ� 3���� �⺻ ������ ���� ���� �ϰ� ������ ���� ����
			//, FLinearColor::Red
			//, FLinearColor::Green
			//, 5.0f
		);

		if (bValidRayTrace)
		{
			// Hit�� ������ HitActor�� Location�� ���� Actor�� �������� Hit�� �������� ���ϴ� Location�� ����
			AActor* HitActor = HitResult.GetActor();
			HitResultActor = HitActor;
			if (HitActor)
			{
				// �ֽþ� ���� ��ġ�� ��ȣ�ۿ�
				// Component������ ���� �����ϵ��� �߰��ؾ���
				ADominateEyeTestActor* DominateEyeTestActor = Cast<ADominateEyeTestActor>(HitActor);
				if (DominateEyeTestActor)
				{
					// DominantEyeTestActor�� Ray�� ��ȣ�ۿ��� ������� Set
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
		// �ΰ������ͷ� �ֽþ� �����⸦ ����Ű�� ���� ���� Ʈ���Ű� �ߵ��ϵ��� �Ѵ�.
		if (DominantEyeTestActor)
		{
			// DominantEyeTestActor�� Component �߿���
			// AddScore�� ��ȣ�ۿ� �Ѵٸ� ������ +
			// SubScore�� ��ȣ�ۿ� �Ѵٸ� ������ -
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
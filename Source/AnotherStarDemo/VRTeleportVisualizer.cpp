// Fill out your copyright notice in the Description page of Project Settings.


#include "VRTeleportVisualizer.h"
#include "NiagaraSystem.h"

// Sets default values
AVRTeleportVisualizer::AVRTeleportVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	NS_TeleportRing = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_TeleportRing"));
	NS_PlayAreaBounds = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_PlayAreaBounds"));

	NS_TeleportRing->SetupAttachment(Root);
	NS_PlayAreaBounds->SetupAttachment(Root);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TPRing(TEXT("/Game/VFX/NS_TeleportRing"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TPPlayAreaBounds(TEXT("/Game/VFX/NS_PlayAreaBounds"));
	NS_TeleportRing->SetAsset(TPRing.Object);
	NS_PlayAreaBounds->SetAsset(TPPlayAreaBounds.Object);

	this->RootComponent = Root;
}

// Called when the game starts or when spawned
void AVRTeleportVisualizer::BeginPlay()
{
	Super::BeginPlay();
	
	// Vec2 return 값을 받아서 각각 x, y 에 대입하고 In Value Z 값을 0으로 초기화
	FVector2D PlayArea2D;
	PlayArea2D = UHeadMountedDisplayFunctionLibrary::GetPlayAreaBounds(EHMDTrackingOrigin::Stage);

	FVector PlayArea3D(PlayArea2D.X, PlayArea2D.Y, 0.f);

	// 5.4 Migration
	//NS_PlayAreaBounds->SetNiagaraVariableVec3("User.PlayAreaBounds", PlayArea3D);
}

// Called every frame
void AVRTeleportVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 5.4 Migration
	//NS_PlayAreaBounds->SetRelativeLocation(GetRelativeLocation(), false, false);
	this->SetActorRotation(GetRelativeRotation(), ETeleportType::None);
}

FVector AVRTeleportVisualizer::GetRelativeLocation()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector NewArea = UKismetMathLibrary::NegateVector(UKismetMathLibrary::InverseTransformLocation(PlayerPawn->GetActorTransform(), PlayerCameraManager->GetCameraLocation()));
	NewArea.Z = 0.f;

	return NewArea;
}

FRotator AVRTeleportVisualizer::GetRelativeRotation()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return PlayerPawn->GetActorRotation();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDisk.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AOrbitDisk::AOrbitDisk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	PartsRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PartsRootSceneComponent"));
	SlotRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SlotRootSceneComponent"));
	PartsCollisionRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PartsCollisionRootSceneComponent"));
	SlotCollisionRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SlotCollisionRootSceneComponent"));
	DiskComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiskComponent"));
	PartsComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PartsComponent"));
	SlotComponent1 = CreateDefaultSubobject<USlotComponent>(TEXT("SlotComponent1"));
	SlotComponent2 = CreateDefaultSubobject<USlotComponent>(TEXT("SlotComponent2"));
	SlotComponent3 = CreateDefaultSubobject<USlotComponent>(TEXT("SlotComponent3"));
	SlotComponent4 = CreateDefaultSubobject<USlotComponent>(TEXT("SlotComponent4"));
	SlotComponent5 = CreateDefaultSubobject<USlotComponent>(TEXT("SlotComponent5"));
	SlotComponent6 = CreateDefaultSubobject<USlotComponent>(TEXT("SlotComponent6"));
	HorizonComponent = CreateDefaultSubobject<USlotComponent>(TEXT("HorizonComponent"));
	PartsCollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("PartsCollisionVolume"));
	SlotCollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("SlotCollisionVolume"));

	RootComponent = RootSceneComponent;
	PartsRootSceneComponent->SetupAttachment(GetRootComponent());
	SlotRootSceneComponent->SetupAttachment(GetRootComponent());
	PartsCollisionRootSceneComponent->SetupAttachment(GetRootComponent());
	SlotCollisionRootSceneComponent->SetupAttachment(GetRootComponent());
	DiskComponent->SetupAttachment(GetRootComponent());
	PartsComponent->SetupAttachment(PartsRootSceneComponent);
	SlotComponent1->SetupAttachment(SlotRootSceneComponent);
	SlotComponent2->SetupAttachment(SlotRootSceneComponent);
	SlotComponent3->SetupAttachment(SlotRootSceneComponent);
	SlotComponent4->SetupAttachment(SlotRootSceneComponent);
	SlotComponent5->SetupAttachment(SlotRootSceneComponent);
	SlotComponent6->SetupAttachment(SlotRootSceneComponent);
	HorizonComponent->SetupAttachment(SlotRootSceneComponent);
	PartsCollisionVolume->SetupAttachment(PartsCollisionRootSceneComponent);
	SlotCollisionVolume->SetupAttachment(SlotCollisionRootSceneComponent);
	
	// 궤적을 나타내는 고리와는 OverlapEvent를 발생시키면 안된다.
	DiskComponent->SetGenerateOverlapEvents(false);
	HorizonComponent->SetbIsItHorizon(true);
	
	bIsOverlappedWithSlot = false;
	bIsOverlappedWithHorizon = false;
	bIsItNight = false;
}

// Called when the game starts or when spawned
void AOrbitDisk::BeginPlay()
{
	Super::BeginPlay();

	PartsCollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AOrbitDisk::OnPartsComponentOverlapBegin);
	PartsCollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AOrbitDisk::OnPartsComponentOverlapEnd);


	
}

// Called every frame
void AOrbitDisk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrbitDisk::RotateParts(float Degree)
{
	FRotator AddRot(0.f, -Degree, 0.f);

	// Slot과 Overlap되어있을 경우에는 Parts가 Snap되어 있어야 한다.
	if (!bIsOverlappedWithSlot)
	{
		PartsRootSceneComponent->AddRelativeRotation(AddRot);
	}

	PartsCollisionRootSceneComponent->AddRelativeRotation(AddRot);
}

// TargetComponent가 Root를 바라보도록 Set
void AOrbitDisk::SetLookAtRoot(UStaticMeshComponent* TargetComponent)
{
	FVector StartLoc = TargetComponent->GetComponentLocation(); // Root를 바라보게 하려는 Component의 위치
	FVector EndLoc = RootSceneComponent->GetComponentLocation(); // Root의 위치
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLoc, EndLoc);

	LookAtRotation.Add(0, -90.f, 0); // 궤도와 수직으로 방향으로 서서 바라보도록 해야함
	
	TargetComponent->SetWorldRotation(LookAtRotation);
}

void AOrbitDisk::OnPartsComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("PartsComponent Overlap Begin... Other Actor Name: %s"), *OtherActor->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("PartsComponent Overlap Begin... Other Comp Class Name: %s"), *OtherComp->GetClass()->GetName()));

	if (OtherComp)
	{
		USlotComponent* OverlappedSlotComponent = Cast<USlotComponent>(OtherComp);

		if (OverlappedSlotComponent)
		{
			bIsOverlappedWithSlot = true;

			FVector SlotLocation = OverlappedSlotComponent->GetComponentLocation();
			FRotator SlotRotation = OverlappedSlotComponent->GetComponentRotation();
			if (OverlappedSlotComponent->bIsItHorizon)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("HorizonHorizonHorizonHorizon")));
				bIsOverlappedWithHorizon = true;
			}

			SetOverlappedSlotTime(OverlappedSlotComponent->Time);

			PartsComponent->SetWorldLocationAndRotation(SlotLocation, SlotRotation);

			float Time = OverlappedSlotComponent->GetTime();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Failed to cast to USlotComponent"));
		}

	}

}

void AOrbitDisk::OnPartsComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("PartsComponent Overlap End... Other Actor Name: %s"), *OtherActor->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("PartsComponent Overlap End... Other Comp Name: %s"), *OtherComp->GetName()));

	if (OtherComp)
	{
		USlotComponent* OverlappedSlotComponent = Cast<USlotComponent>(OtherComp);
		if (OverlappedSlotComponent)
		{
			bIsOverlappedWithSlot = false;
			bIsOverlappedWithHorizon = false;

			// 파츠의 위치 Snap 해제
			FVector CollisionLocation = PartsCollisionVolume->GetComponentLocation();
			FRotator CollisionRotation = PartsCollisionVolume->GetComponentRotation();

			PartsComponent->SetWorldLocationAndRotation(CollisionLocation, CollisionRotation);
		}
	}

}
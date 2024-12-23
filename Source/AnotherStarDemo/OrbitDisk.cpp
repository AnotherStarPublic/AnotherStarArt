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
	SlotComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlotComponent"));
	PartsCollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("PartsCollisionVolume"));
	SlotCollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("SlotCollisionVolume"));

	RootComponent = RootSceneComponent;
	PartsRootSceneComponent->SetupAttachment(GetRootComponent());
	SlotRootSceneComponent->SetupAttachment(GetRootComponent());
	PartsCollisionRootSceneComponent->SetupAttachment(GetRootComponent());
	SlotCollisionRootSceneComponent->SetupAttachment(GetRootComponent());
	DiskComponent->SetupAttachment(GetRootComponent());
	PartsComponent->SetupAttachment(PartsRootSceneComponent);
	SlotComponent->SetupAttachment(SlotRootSceneComponent);
	PartsCollisionVolume->SetupAttachment(PartsCollisionRootSceneComponent);
	SlotCollisionVolume->SetupAttachment(SlotCollisionRootSceneComponent);
	
	// ������ ��Ÿ���� ���ʹ� OverlapEvent�� �߻���Ű�� �ȵȴ�.
	DiskComponent->SetGenerateOverlapEvents(false);

	// �Ϲ� StaticMesh�� PartsComponent�� �����ϱ� ����  Tag
	PartsComponent->ComponentTags.Add(FName("Parts"));
	
	bIsOverlappedWithSlot = false;

}

// Called when the game starts or when spawned
void AOrbitDisk::BeginPlay()
{
	Super::BeginPlay();

	PartsCollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AOrbitDisk::OnPartsComponentOverlapBegin);
	SlotCollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AOrbitDisk::OnSlotComponentOverlapBegin);
	PartsCollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AOrbitDisk::OnPartsComponentOverlapEnd);
	SlotCollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AOrbitDisk::OnSlotComponentOverlapEnd);


	
}

// Called every frame
void AOrbitDisk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrbitDisk::RotateParts(float Degree)
{
	FRotator AddRot(0.f, -Degree, 0.f);

	// Slot�� Overlap�Ǿ����� ��쿡�� Parts�� Snap�Ǿ� �־�� �Ѵ�.
	if (!bIsOverlappedWithSlot)
	{
		PartsRootSceneComponent->AddRelativeRotation(AddRot);
	}

	PartsCollisionRootSceneComponent->AddRelativeRotation(AddRot);
}

// TargetComponent�� Root�� �ٶ󺸵��� Set
void AOrbitDisk::SetLookAtRoot(UStaticMeshComponent* TargetComponent)
{
	FVector StartLoc = TargetComponent->GetComponentLocation(); // Root�� �ٶ󺸰� �Ϸ��� Component�� ��ġ
	FVector EndLoc = RootSceneComponent->GetComponentLocation(); // Root�� ��ġ
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLoc, EndLoc);

	LookAtRotation.Add(0, -90.f, 0); // �˵��� �������� �������� ���� �ٶ󺸵��� �ؾ���
	
	TargetComponent->SetWorldRotation(LookAtRotation);
}

void AOrbitDisk::OnPartsComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("PartsComponent Overlap Begin... Other Actor Name: %s"), *OtherActor->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("PartsComponent Overlap Begin... Other Comp Name: %s"), *OtherComp->GetName()));
}

void AOrbitDisk::OnSlotComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("SlotComponent Overlap Begin... Other Actor Name: %s"), *OtherActor->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("SlotComponent Overlap Begin... Other Comp Name: %s"), *OtherComp->GetName()));
}

void AOrbitDisk::OnPartsComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("PartsComponent Overlap End... Other Actor Name: %s"), *OtherActor->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("PartsComponent Overlap End... Other Comp Name: %s"), *OtherComp->GetName()));
}

void AOrbitDisk::OnSlotComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("SlotComponent Overlap End... Other Actor Name: %s"), *OtherActor->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("SlotComponent Overlap End... Other Comp Name: %s"), *OtherComp->GetName()));
}
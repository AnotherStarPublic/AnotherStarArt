// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDiskSlot.h"

// Sets default values
AOrbitDiskSlot::AOrbitDiskSlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SlotRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SlotRootSceneComponent"));
	DiskComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiskComponent"));
	SlotComponent = CreateDefaultSubobject<USlotComponent>(TEXT("SlotComponent"));
	SphereCollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionVolume"));

	RootComponent = RootSceneComponent;
	SlotRootSceneComponent->SetupAttachment(GetRootComponent());
	DiskComponent->SetupAttachment(GetRootComponent());
	SlotComponent->SetupAttachment(SlotRootSceneComponent);
	SphereCollisionVolume->SetupAttachment(SlotRootSceneComponent);

	bIsItNight = false;

}

// Called when the game starts or when spawned
void AOrbitDiskSlot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrbitDiskSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
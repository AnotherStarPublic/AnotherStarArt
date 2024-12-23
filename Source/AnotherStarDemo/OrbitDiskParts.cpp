// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDiskParts.h"
#include "SlotComponent.h"
#include "OrbitDiskSlot.h"

// Sets default values
AOrbitDiskParts::AOrbitDiskParts()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	PartsRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PartsRootSceneComponent"));
	CollisionRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionRootSceneComponent"));
	PartsComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PartsComponent"));
	SphereCollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionVolume"));

	RootComponent = RootSceneComponent;
	PartsRootSceneComponent->SetupAttachment(GetRootComponent());
	CollisionRootSceneComponent->SetupAttachment(GetRootComponent());
	PartsComponent->SetupAttachment(PartsRootSceneComponent);
	SphereCollisionVolume->SetupAttachment(CollisionRootSceneComponent);

	OverlappedSlotTime = 0.f;
	bIsOverlappedWithSlot = false;
	bIsOverlappedWithHorizon = false;
	bIsItNight = false;

}

// Called when the game starts or when spawned
void AOrbitDiskParts::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AOrbitDiskParts::OnOverlapBegin);
	SphereCollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AOrbitDiskParts::OnOverlapEnd);

}

// Called every frame
void AOrbitDiskParts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrbitDiskParts::RotateDisk(float Degree)
{
	FRotator AddRot(0.f, -Degree, 0.f);
	
	if (!bIsOverlappedWithSlot)
	{
		PartsRootSceneComponent->AddRelativeRotation(AddRot);
	}

	CollisionRootSceneComponent->AddRelativeRotation(AddRot);

}

void AOrbitDiskParts::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Notify Actor Begin Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Notify Component Begin Overlap... OverlappedComponent Name: %s"), *OverlappedComponent->GetName()));

	if (OtherActor)
	{
		AOrbitDiskSlot* OrbitDiskSlot = Cast<AOrbitDiskSlot>(OtherActor);
		if (OrbitDiskSlot)
		{
			if (OverlappedComponent)
			{
				USlotComponent* SlotComponent = Cast<USlotComponent>(OverlappedComponent);
				bIsOverlappedWithSlot = true;
				
				FVector SlotLocation = OrbitDiskSlot->SlotComponent->GetComponentLocation();
				FRotator SlotRotation = OrbitDiskSlot->SlotComponent->GetComponentRotation();
				if (OrbitDiskSlot->SlotComponent->bIsItHorizon)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("HorizonHorizonHorizonHorizon")));
					bIsOverlappedWithHorizon = true;
				}
				
				SetOverlappedSlotTime(OrbitDiskSlot->SlotComponent->Time);

				PartsComponent->SetWorldLocationAndRotation(SlotLocation, SlotRotation);

				float Time = SlotComponent->GetTime();

			}
		}
	}
}

void AOrbitDiskParts::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AOrbitDiskSlot* OrbitDiskSlot = Cast<AOrbitDiskSlot>(OtherActor);
		if (OrbitDiskSlot)
		{
			if (OverlappedComponent)
			{
				USlotComponent* SlotComponent = Cast<USlotComponent>(OverlappedComponent);
				bIsOverlappedWithSlot = false;
				bIsOverlappedWithHorizon = false;

				FVector CollisionLocation = SphereCollisionVolume->GetComponentLocation();
				FRotator CollisionRotation = SphereCollisionVolume->GetComponentRotation();

				PartsComponent->SetWorldLocationAndRotation(CollisionLocation, CollisionRotation);

			}
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitPlatform.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "VRCharacter.h"

// Sets default values
AOrbitPlatform::AOrbitPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));

	CollisionVolume->SetupAttachment(StaticMesh);

}

// Called when the game starts or when spawned
void AOrbitPlatform::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AOrbitPlatform::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AOrbitPlatform::OnOverlapEnd);
}

// Called every frame
void AOrbitPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrbitPlatform::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Notify Actor Begin Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
	// 1. ĳ���Ͱ� �ö���� ��� ���� ������ �����İ� ������ �� �ֵ��� �ϴ� ������ Ȱ��ȭ ���Ѿ� �Ѵ�.
	// 2. �̾ ���� ������ ������ N��(�� �ʰ� ������ �׽�Ʈ�ϸ鼭 �����غ�����) ������ ��� OrbitPlatform�� Ȱ��ȭ�ǰ�
	// 3. �ش� Platform�� ���������� �ڷ���Ʈ�� �����ϵ��� �������Ѵ�.
	// 4. ����, OrbitDisk�� ���� �� �ֵ��� UI�� Ȱ��ȭ �Ǹ�
	// 5. FingerPointer�� Ȱ��ȭ�ȴ�.
	if (OtherActor)
	{
		AVRCharacter* VRCharacter = Cast<AVRCharacter>(OtherActor);
		if (VRCharacter)
		{
			// 1. �����İ� ����������.
			VRCharacter->SetCanFingerPointer(true);
		}
	}
}

void AOrbitPlatform::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Notify Actor End Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
	// 1. ĳ���Ͱ� �������� ��� ���� ������ �����İ� �Ұ����ϵ��� ������ ��Ȱ��ȭ ���Ѿ��Ѵ�.
	// 2. ���� OrbitDisk�� ������ ���ϵ��� ������ ��Ȱ��ȭ�ǰ� FingerPointer ���õ� ��Ȱ��ȭ �ȴ�.
	// 3. ������ �ڷ���Ʈ ����� Ȱ��ȭ�� �������ΰ����� ��Ȱ��ȭ ���� �ʴ´�.
	if (OtherActor)
	{
		AVRCharacter* VRCharacter = Cast<AVRCharacter>(OtherActor);
		if (VRCharacter)
		{
			// 1. �ΰ������� �Ұ���
			VRCharacter->SetCanFingerPointer(false);
		}
	}
}
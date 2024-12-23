// Fill out your copyright notice in the Description page of Project Settings.


#include "DrumActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADrumActor::ADrumActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());


}

// Called when the game starts or when spawned
void ADrumActor::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void ADrumActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADrumActor::DrumHit(ADrumStick* DrumStick)
{
	ESpeedState SpeedState = DrumStick->GetSpeedState();
	
	if (SpeedState == ESpeedState::ESS_Slow)
	{
		return;
	}
	else if (SpeedState == ESpeedState::ESS_MediumSlow)
	{
		// DrumStick�� �ӷ°� ����
		FVector DrumStickVelocity = DrumStick->GetVelocity();
		float DrumStickSpeed = DrumStickVelocity.Size();
		// DrumStick�� �ӵ����� ���� �Ǽ������� Ư�� ������ ���� �� �� �ֵ��� �Ѵ�.
		// ����, �ӵ� ���� ���� Collider�� �ִ� �������� Collision�� Spawn�ϰ� �߾� �����κ��� õõ�� ũ�⸦ ������Ų��.
	}
	else if (SpeedState == ESpeedState::ESS_MediumFast)
	{
		// DrumStick�� �ӷ°� ����
		FVector DrumStickVelocity = DrumStick->GetVelocity();
		float DrumStickSpeed = DrumStickVelocity.Size();
		// DrumStick�� �ӵ����� ���� �Ǽ������� Ư�� ������ ���� �� �� �ֵ��� �Ѵ�.
		// ����, �ӵ� ���� ���� Collider�� �ִ� �������� Collision�� Spawn�ϰ� �߾� �����κ��� õõ�� ũ�⸦ ������Ų��.
	}
	else if (SpeedState == ESpeedState::ESS_Fast)
	{
		// ���� �ĵ� ������ ū ��(float)�� ������ �� �ֵ��� �Ѵ�.
		// ����, Collider�� �ִ� �������� Collision�� Spawn�ϰ� �߾� �����κ��� õõ�� ũ�⸦ ������Ű�µ� �ִ� ũ��� �����Ѵ�.
		// Ư�� Trigger�� �ߵ� ��ų �� �ֵ��� �Ѵ�.
	}
}
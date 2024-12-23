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
		// DrumStick의 속력값 추출
		FVector DrumStickVelocity = DrumStick->GetVelocity();
		float DrumStickSpeed = DrumStickVelocity.Size();
		// DrumStick의 속도값에 따라 실수범위의 특정 값들을 전달 할 수 있도록 한다.
		// 또한, 속도 값에 따라 Collider가 있는 돔형태의 Collision을 Spawn하고 중앙 값으로부터 천천히 크기를 증가시킨다.
	}
	else if (SpeedState == ESpeedState::ESS_MediumFast)
	{
		// DrumStick의 속력값 추출
		FVector DrumStickVelocity = DrumStick->GetVelocity();
		float DrumStickSpeed = DrumStickVelocity.Size();
		// DrumStick의 속도값에 따라 실수범위의 특정 값들을 전달 할 수 있도록 한다.
		// 또한, 속도 값에 따라 Collider가 있는 돔형태의 Collision을 Spawn하고 중앙 값으로부터 천천히 크기를 증가시킨다.
	}
	else if (SpeedState == ESpeedState::ESS_Fast)
	{
		// 가장 파동 범위가 큰 값(float)을 전달할 수 있도록 한다.
		// 또한, Collider가 있는 돔형태의 Collision을 Spawn하고 중앙 값으로부터 천천히 크기를 증가시키는데 최대 크기로 증가한다.
		// 특정 Trigger를 발동 시킬 수 있도록 한다.
	}
}
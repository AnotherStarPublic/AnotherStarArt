// Fill out your copyright notice in the Description page of Project Settings.


#include "DominateEyeTestActor.h"
#include "Components/WidgetComponent.h"
#include "VRCharacter.h"

// Sets default values
ADominateEyeTestActor::ADominateEyeTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DominateEyeTestWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DominateEyeTestWidget"));
	AddScoreComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AddScoreComponent"));
	SubScoreComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubScoreComponent"));

	AddScoreComponent->SetupAttachment(GetRootComponent());
	SubScoreComponent->SetupAttachment(GetRootComponent());

	Score = 0;
}

// Called when the game starts or when spawned
void ADominateEyeTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADominateEyeTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADominateEyeTestActor::AddScore()
{
	Score++;

	// 점수가 3이 되면 오른눈을 주시안으로 설정
	if (Score == 3)
	{
		VRCharacter->SetDominantEye(EDominantEye::DE_RightEye);
	}

}

void ADominateEyeTestActor::SubScore()
{
	Score--;

	// 점수가 -3이 되면 오른눈을 주시안으로 설정
	if (Score == -3)
	{
		VRCharacter->SetDominantEye(EDominantEye::DE_LeftEye);
	}
}
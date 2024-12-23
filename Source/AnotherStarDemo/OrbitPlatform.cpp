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
	// 1. 캐릭터가 올라왔을 경우 손을 모으는 제스쳐가 가능할 수 있도록 하는 조건을 활성화 시켜야 한다.
	// 2. 이어서 손을 모으는 행위를 N초(몇 초가 좋을지 테스트하면서 결정해봐야함) 유지할 경우 OrbitPlatform이 활성화되고
	// 3. 해당 Platform은 영구적으로 텔레포트가 가능하도록 만들어야한다.
	// 4. 또한, OrbitDisk를 돌릴 수 있도록 UI가 활성화 되며
	// 5. FingerPointer가 활성화된다.
	if (OtherActor)
	{
		AVRCharacter* VRCharacter = Cast<AVRCharacter>(OtherActor);
		if (VRCharacter)
		{
			// 1. 제스쳐가 가능해진다.
			VRCharacter->SetCanFingerPointer(true);
		}
	}
}

void AOrbitPlatform::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Notify Actor End Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
	// 1. 캐릭터가 내려왔을 경우 손을 모으는 제스쳐가 불가능하도록 조건을 비활성화 시켜야한다.
	// 2. 또한 OrbitDisk를 돌리지 못하도록 조건이 비활성화되고 FingerPointer 역시도 비활성화 된다.
	// 3. 하지만 텔레포트 기능의 활성화는 영구적인것으로 비활성화 되지 않는다.
	if (OtherActor)
	{
		AVRCharacter* VRCharacter = Cast<AVRCharacter>(OtherActor);
		if (VRCharacter)
		{
			// 1. 핑거포인터 불가능
			VRCharacter->SetCanFingerPointer(false);
		}
	}
}
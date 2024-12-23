// Fill out your copyright notice in the Description page of Project Settings.


#include "DrumStick.h"
#include "VRCharacter.h"
#include "DrumActor.h"
#include "GrabComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


ADrumStick::ADrumStick()
{
	/*
	PercussionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PercussionCollision"));
	PercussionCollision->SetupAttachment(GetRootComponent());
	*/

	DrumStickState = EDrumStickState::EDS_PickUp;
	SpeedState = ESpeedState::ESS_None;

	this->SetItemType(EItemType::IT_UnConsumable);

	bIsTriggeredEvent = false;

}

void ADrumStick::BeginPlay()
{
	Super::BeginPlay();

	/*
	// DrumStick의 충돌반응 Binding
	PercussionCollision->OnComponentBeginOverlap.AddDynamic(this, &ADrumStick::PercussionOnOverlapBegin);
	PercussionCollision->OnComponentEndOverlap.AddDynamic(this, &ADrumStick::PercussionOnOverlapEnd);

	PercussionCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PercussionCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PercussionCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PercussionCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	*/
}

// Called every frame
void ADrumStick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// DrumStick이 손에 들려있을 때 매 프레임 속도에 따라 SpeedState를 결정한다.
	// Tick은 데이터 낭비가 많으므로 Grabbed상태가 아니라면 바로 return
	if (DrumStickState != EDrumStickState::EDS_Grabbed) return;
	

}

void ADrumStick::DoPickUp(AVRCharacter* Character)
{
	// 캐릭터가 아이템을 들고 있을 때 아이템을 획득 할 수 없도록 한다.
	if (Character->GetIsHeld())
	{
		return;
	}
	else
	{
		//Character->TakeItem(this);
	}
}

void ADrumStick::Grabbed(AVRCharacter* Character)
{
	if (Character)
	{

	}
}

// Item 형태로 되어있는 DrumStick을 습득할 때 VRCharacter와 Collision발생 시 해당 아이템을 인지하도록 한다.
void ADrumStick::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if ((DrumStickState == EDrumStickState::EDS_PickUp) && OtherActor)
	{
		AVRCharacter* VRCharacter = Cast<AVRCharacter>(OtherActor);
		if (VRCharacter)
		{
			VRCharacter->SetActiveOverlappingItem(this);
		}
	}

}

// Item 형태로 되어있는 DrumStick을 습득할 때 VRCharacter와 Collision이 떨어 질 때 VRCharacter의 OverllapingItem을 nullptr로 설정한다.
void ADrumStick::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if ((DrumStickState == EDrumStickState::EDS_PickUp) && OtherActor)
	{
		AVRCharacter* VRCharacter = Cast<AVRCharacter>(OtherActor);
		if (VRCharacter)
		{
			VRCharacter->SetActiveOverlappingItem(nullptr);
		}
	}
}

void ADrumStick::PercussionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((DrumStickState == EDrumStickState::EDS_Grabbed) && (SpeedState != ESpeedState::ESS_None) && OtherActor)
	{
		ADrumActor* DrumActor = Cast<ADrumActor>(OtherActor);
		if (DrumActor)
		{
			DrumActor->DrumHit(this);

			if (DrumActor->HitSound)
			{
				// SpeedState가 Slow라면 소리가 발생하지 않도록 한다.
				if (SpeedState != ESpeedState::ESS_Slow)
				{
					// 세기에 따라 소리가 달라야한다.
					UGameplayStatics::PlaySound2D(this, DrumActor->HitSound);
				}
			}
		}
	}
}

void ADrumStick::PercussionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
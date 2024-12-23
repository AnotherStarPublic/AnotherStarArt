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
	// DrumStick�� �浹���� Binding
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

	// DrumStick�� �տ� ������� �� �� ������ �ӵ��� ���� SpeedState�� �����Ѵ�.
	// Tick�� ������ ���� �����Ƿ� Grabbed���°� �ƴ϶�� �ٷ� return
	if (DrumStickState != EDrumStickState::EDS_Grabbed) return;
	

}

void ADrumStick::DoPickUp(AVRCharacter* Character)
{
	// ĳ���Ͱ� �������� ��� ���� �� �������� ȹ�� �� �� ������ �Ѵ�.
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

// Item ���·� �Ǿ��ִ� DrumStick�� ������ �� VRCharacter�� Collision�߻� �� �ش� �������� �����ϵ��� �Ѵ�.
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

// Item ���·� �Ǿ��ִ� DrumStick�� ������ �� VRCharacter�� Collision�� ���� �� �� VRCharacter�� OverllapingItem�� nullptr�� �����Ѵ�.
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
				// SpeedState�� Slow��� �Ҹ��� �߻����� �ʵ��� �Ѵ�.
				if (SpeedState != ESpeedState::ESS_Slow)
				{
					// ���⿡ ���� �Ҹ��� �޶���Ѵ�.
					UGameplayStatics::PlaySound2D(this, DrumActor->HitSound);
				}
			}
		}
	}
}

void ADrumStick::PercussionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
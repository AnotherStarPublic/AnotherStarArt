// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabComponent.h"

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	GrabType = EGrabType::GT_Free;
	
	bIsHeld = false;
	bSimulateOnDrop = false;
}


// Called when the game starts
void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();

	SetShouldSimulateOnDrop();

	// Parent�� CollisionProfile�� ����
	Cast<UPrimitiveComponent>(GetAttachParent())->SetCollisionProfileName("PhysicsActor", true);

}

void UGrabComponent::OnGrabbedEvent()
{
	OnGrabbed.ExecuteIfBound();
}

void UGrabComponent::OnDroppedEvent()
{
	OnDropped.ExecuteIfBound();
}

// Called every frame
void UGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/*
bool UGrabComponent::TryGrab(UMotionControllerComponent* MotionController)
{
	if (GrabType == EGrabType::GT_Free)
	{
		// �׷��� Ȱ��ȭ ���� ��� Physics�� ��Ȱ��ȭ ��Ų ��, Parent Actor�� MotionController�� Attach �Ѵ� 
		this->SetPrimitiveCompPhysics(false);
		GetAttachParent()->AttachToComponent(MotionController, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));

		bIsHeld = true;
	}
	else if (GrabType == EGrabType::GT_Snap)
	{
		// �׷��� Ȱ��ȭ ���� ��� Physics�� ��Ȱ��ȭ ��Ų ��, Parent Actor�� MotionController�� Attach �Ѵ�
		this->SetPrimitiveCompPhysics(false);
		GetAttachParent()->AttachToComponent(MotionController, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));

		bIsHeld = true;

		// Held�� Actor�� GrabComponent�� RelativeLocation�� ���� ���� (Snap)
		GetAttachParent()->SetRelativeRotation(UKismetMathLibrary::NegateRotator(this->GetRelativeRotation()), false, nullptr, TeleportFlagToEnum(true));
		GetAttachParent()->SetWorldLocation((MotionController->GetComponentLocation()) + ((this->GetComponentLocation()) - (GetAttachParent()->GetComponentLocation())) * (-1), false, nullptr, TeleportFlagToEnum(true));
	}
	else if (GrabType == EGrabType::GT_Custom)
	{
		bIsHeld = true;
	}
	
	if (bIsHeld)	// Grab�� �������� ��
	{
		// MotionController�� ���� ���� ����
		MotionControllerRef = MotionController;
		OnGrabbedEvent();

		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayHapticEffect(OnGrabHapticEffect, GetHeldByHand());

		return true;
	}
	else			// Grab�� �������� ��
	{
		return false;
	}
}

bool UGrabComponent::TryRelease()
{
	if (GrabType == EGrabType::GT_Free || GrabType == EGrabType::GT_Snap)
	{
		if (bSimulateOnDrop == true)
		{
			SetPrimitiveCompPhysics(true);
			bIsHeld = false;
		}
		else
		{
			GetAttachParent()->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
			bIsHeld = false;
		}
	}
	else if (GrabType == EGrabType::GT_Custom)
	{
		bIsHeld = false;
	}

	if (bIsHeld)
	{
		return false;
	}
	else
	{
		OnDroppedEvent();
		return true;
	}
}
*/

void UGrabComponent::SetShouldSimulateOnDrop()
{
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetAttachParent());

	if (PrimitiveComponent->IsAnySimulatingPhysics())
	{
		bSimulateOnDrop = true;
	}
}

void UGrabComponent::SetPrimitiveCompPhysics(bool bSimulate)
{
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetAttachParent());

	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetSimulatePhysics(bSimulate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GrabComponent->SetSimulatingParent->Cast To PrimitiveComponent FAILED"));
	}
}

EControllerHand UGrabComponent::GetHeldByHand()
{
	if (MotionControllerRef->GetTrackingMotionSource() == "Left")
	{
		return EControllerHand::Left;
	}
	else
	{
		return EControllerHand::Right;
	}
	
}
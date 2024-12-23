// Fill out your copyright notice in the Description page of Project Settings.


#include "HandComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

UHandComponent::UHandComponent()
{
	bMirror = false;
}

void UHandComponent::BeginPlay()
{
	Super::BeginPlay();

}

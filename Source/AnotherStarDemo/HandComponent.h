// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Item.h"
#include "HandComponent.generated.h"

/**
 *  SkeletalMeshComponent인데 bool Type Variable을 추가
 *  왼손 오른손 Mirror 기능을 적용하기 위함
 *  VR Hand를 만들기 위한 Component
 */

UCLASS()
class ANOTHERSTARDEMO_API UHandComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public : 
	UHandComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool bMirror;

protected : 
	virtual void BeginPlay() override;

};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Item.h"
#include "HandComponent.generated.h"

/**
 *  SkeletalMeshComponent�ε� bool Type Variable�� �߰�
 *  �޼� ������ Mirror ����� �����ϱ� ����
 *  VR Hand�� ����� ���� Component
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
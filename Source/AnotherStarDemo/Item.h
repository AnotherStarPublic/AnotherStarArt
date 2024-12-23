// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"



/**
 *  Consumable 아이템은 소모성아이템
 *  UnConsumable 아이템은 중요아이템으로 게임을 진행하면서 계속 사용하게 될 아이템
 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Consumable		UMETA(DisplayName = "ConsumableItem"),
	IT_UnConsumable		UMETA(DisplayName = "UnConsumableItem"),

	IT_MAX				UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class ANOTHERSTARDEMO_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enum")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
	class USphereComponent* CollisionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	class UStaticMeshComponent* Mesh;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE void SetItemType(EItemType Type) { ItemType = Type; }
	FORCEINLINE EItemType GetItemType() { return ItemType; }

};

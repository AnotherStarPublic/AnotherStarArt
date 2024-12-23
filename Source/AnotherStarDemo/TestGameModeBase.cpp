// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

ATestGameModeBase::ATestGameModeBase()
	:Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/TestPawn_BP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

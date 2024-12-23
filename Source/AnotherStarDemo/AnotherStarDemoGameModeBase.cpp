// Copyright Epic Games, Inc. All Rights Reserved.


#include "AnotherStarDemoGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

AAnotherStarDemoGameModeBase::AAnotherStarDemoGameModeBase()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/VRCharacter_BP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Sparta_TProject_02GameMode.h"
#include "Sparta_TProject_02Character.h"
#include "UObject/ConstructorHelpers.h"

ASparta_TProject_02GameMode::ASparta_TProject_02GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

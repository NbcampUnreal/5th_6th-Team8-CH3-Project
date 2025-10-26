#include "PlayerCharacterGameMode.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

APlayerCharacterGameMode::APlayerCharacterGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = APlayerCharacterController::StaticClass();
}
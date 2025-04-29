#include "UE_Grapple/Public/PlayerAndGM/GrapplePC.h"

#include "Debug.h"

void AGrapplePC::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	
	EMouseLockMode::LockAlways;
}

void AGrapplePC::OnAnyKeyPressed(const FKey& Key)
{
	this->bIsUsingGamepad = Key.IsGamepadKey();
}

void AGrapplePC::SetupInputComponent()
{
	Super::SetupInputComponent();

	FInputKeyBinding keysIKB(FInputChord(EKeys::AnyKey, false, false, false, false), EInputEvent::IE_Pressed);

	keysIKB.bConsumeInput = true;
	keysIKB.bExecuteWhenPaused = false;

	keysIKB.KeyDelegate.GetDelegateWithKeyForManualSet().BindLambda([this](const FKey& Key) {
		OnAnyKeyPressed(Key);
	});

	InputComponent->KeyBindings.Add(keysIKB);

	FInputKeyBinding mouseIKB(FInputChord(EKeys::Mouse2D, false, false, false, false), EInputEvent::IE_Pressed);

	mouseIKB.bConsumeInput = true;
	mouseIKB.bExecuteWhenPaused = false;

	mouseIKB.KeyDelegate.GetDelegateWithKeyForManualSet().BindLambda([this](const FKey& Key) {
		OnAnyKeyPressed(Key);
	});

	InputComponent->KeyBindings.Add(mouseIKB);
}

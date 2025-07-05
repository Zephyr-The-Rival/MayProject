// Fill out your copyright notice in the Description page of Project Settings.


#include "UE_Grapple2/Public/Player/GrapplePlayerController.h"

void AGrapplePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	
	EMouseLockMode::LockAlways;
}

void AGrapplePlayerController::OnAnyKeyPressed(const FKey& Key)
{
	if (this->bIsUsingGamepad != Key.IsGamepadKey())
	{
		this->bIsUsingGamepad = Key.IsGamepadKey();
		this->OnInputDeviceSwitched.Broadcast(this->bIsUsingGamepad);
	}
	
}
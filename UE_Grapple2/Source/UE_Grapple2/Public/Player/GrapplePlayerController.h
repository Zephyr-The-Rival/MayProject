// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GrapplePlayerController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputDeviceSwitched, bool, bIsUsingGamepad);

UCLASS()
class UE_GRAPPLE2_API AGrapplePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
public:
	bool bIsUsingGamepad=false;

private:
	void OnAnyKeyPressed(const FKey& Key);

public:
	UPROPERTY(BlueprintAssignable)
	FInputDeviceSwitched OnInputDeviceSwitched;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GrapplePlayerController.generated.h"

/**
 * 
 */
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
};

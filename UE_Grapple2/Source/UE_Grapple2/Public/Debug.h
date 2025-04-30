// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Debug.generated.h"

/**
 * 
 */
UCLASS()
class UE_GRAPPLE2_API UDebug : public UObject
{
	GENERATED_BODY()

public:
	
	static void Print(FString Text, float TimeToDisplay= 5, FColor Color = FColor::Cyan);
};

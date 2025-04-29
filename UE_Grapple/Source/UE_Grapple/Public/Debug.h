// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UE_GRAPPLE_API Debug
{
public:
	Debug();
	~Debug();

	static void Print(FString text, float TimeToDisplay= 5, FColor Color = FColor::Cyan);
};

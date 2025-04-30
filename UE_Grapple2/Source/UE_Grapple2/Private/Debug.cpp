// Fill out your copyright notice in the Description page of Project Settings.


#include "UE_Grapple2/Public/Debug.h"


void UDebug::Print(FString Text, float TimeToDisplay, FColor Color)
{
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, Color, Text);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Text);
}

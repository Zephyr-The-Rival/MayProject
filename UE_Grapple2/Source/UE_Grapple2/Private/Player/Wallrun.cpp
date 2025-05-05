// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Wallrun.h"


// Sets default values for this component's properties
UWallrun::UWallrun()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWallrun::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWallrun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWallrun::StartWallrun_Implementation()
{
	
}

void UWallrun::EndWallrun_Implementation()
{
}

void UWallrun::JumpOff_Implementation()
{
	
}


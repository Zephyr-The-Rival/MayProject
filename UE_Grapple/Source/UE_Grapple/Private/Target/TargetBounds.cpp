// Fill out your copyright notice in the Description page of Project Settings.


#include "Target/TargetBounds.h"

#include "Debug.h"
#include "Components/BoxComponent.h"
#include "DynamicMesh/DynamicMesh3.h"


// Sets default values
ATargetBounds::ATargetBounds()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root= CreateDefaultSubobject<USceneComponent>("Root component");
	SetRootComponent(Root);

	Volume=CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	Volume->SetupAttachment(Root);
	
}

// Called when the game starts or when spawned
void ATargetBounds::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetBounds::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATargetBounds::IsLocationInBounds(FVector Location)
{
	return
		Location.X < (Volume->GetComponentLocation() + Volume->GetScaledBoxExtent()).X &&
		Location.X > (Volume->GetComponentLocation() - Volume->GetScaledBoxExtent()).X &&
			
		Location.Y < (Volume->GetComponentLocation() + Volume->GetScaledBoxExtent()).Y &&
		Location.Y > (Volume->GetComponentLocation() - Volume->GetScaledBoxExtent()).Y &&
			
		Location.Z < (Volume->GetComponentLocation() + Volume->GetScaledBoxExtent()).Z &&
		Location.Z > (Volume->GetComponentLocation() - Volume->GetScaledBoxExtent()).Z;
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GrappleShooter/GrappleShooter.h"
#include "CableComponent.h"


// Sets default values
AGrappleShooter::AGrappleShooter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->GrappleShooterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	this->CableComponent= CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	this->CableComponent->SetupAttachment(GrappleShooterMesh,TEXT("ProjectileSpawnPoint"));
	this->CableComponent->SetHiddenInGame(true);
	
}

// Called when the game starts or when spawned
void AGrappleShooter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrappleShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrappleShooter::Pressed_Implementation()
{
}

void AGrappleShooter::PullPressed_Implementation()
{
}

void AGrappleShooter::Released_Implementation()
{
}

void AGrappleShooter::PullReleased_Implementation()
{
}







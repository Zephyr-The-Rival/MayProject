// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GrappleShooter/GrappleProjectile.h"

#include "Components/SphereComponent.h"


// Sets default values
AGrappleProjectile::AGrappleProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->ProjectileMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	this->ProjectileMesh->SetupAttachment(this->GetRootComponent());

	this->SpinAnchor=CreateDefaultSubobject<USceneComponent>("SpinAnchor");
	this->SpinAnchor->SetupAttachment(ProjectileMesh);

	this->SpinComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Spin Component"));
	this->SpinComponent->SetupAttachment(this->SpinAnchor);
}

// Called when the game starts or when spawned
void AGrappleProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrappleProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


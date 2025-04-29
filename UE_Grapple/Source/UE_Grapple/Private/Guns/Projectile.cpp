// Fill out your copyright notice in the Description page of Project Settings.


#include "Guns/Projectile.h"

#include "Debug.h"
#include "Target/FlyingTarget.h"
#include "Components/SphereComponent.h"


// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionSphere= CreateDefaultSubobject<USphereComponent>("Collision Sphere");
	SetRootComponent(CollisionSphere);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector deltaLocation= this->GetActorForwardVector()*this->ProjectileSpeed*DeltaTime;
	FHitResult hit;
	AddActorWorldOffset(deltaLocation,true,&hit);
	
	if(hit.bBlockingHit)
	{
		//Debug::Print("Bullet hit something"+hit.GetActor()->GetName());
		if(Cast<AFlyingTarget>(hit.GetActor()))
		{
			OnHitTargetEvent.Broadcast();
			hit.GetActor()->Destroy();
		}
		
		
		Destroy();
	}
		
	
}


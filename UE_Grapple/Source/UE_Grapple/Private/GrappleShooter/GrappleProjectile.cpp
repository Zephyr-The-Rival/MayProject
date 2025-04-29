// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleShooter/GrappleProjectile.h"
#include "Debug.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
AGrappleProjectile::AGrappleProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->Mesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	this->RootComponent=this->Mesh;
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

	if(this->Move)
	{
		FHitResult ProjectileHitResult;
		this->AddActorWorldOffset(GetActorForwardVector()*this->ProjectileSpeed*DeltaTime, true,&ProjectileHitResult);

		if(ProjectileHitResult.bBlockingHit)
		{
			this->Move=false;
			this->SetActorLocation(ProjectileHitResult.Location);
			FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld,EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld,false); 
			this->AttachToComponent(ProjectileHitResult.GetComponent(),rules);

			if(SparksOnHit)
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),SparksOnHit,ProjectileHitResult.Location);
			
			OnHit.Broadcast();
		}
	}
	
}

void AGrappleProjectile::StopMove()
{
	this->Move=false;
}


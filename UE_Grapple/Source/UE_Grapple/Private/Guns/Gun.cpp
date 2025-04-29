// Fill out your copyright notice in the Description page of Project Settings.


#include "Guns/Gun.h"

#include "Debug.h"
#include "Guns/Projectile.h"
#include "PlayerAndGM/GrapplePlayerCharacter.h"


// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GunMesh=CreateDefaultSubobject<USkeletalMeshComponent>("GunMesh");
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	if(!GunMesh->DoesSocketExist("BulletSocket"))
		Debug::Print("Gun doesnt have a 'BulletSocket'");
		
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void AGun::PullTrigger()
{
	if(!bFirstShot && !bAutomatic)
		return;
	
	this->Shoot();
}

void AGun::Shoot()
{
	bFirstShot=false;
	SetGunState(EGunState::Shooting);
	Ammo--;

	if(!this->ProjectileToSpawn)
		return;

	
	FHitResult hit = Cast<AGrapplePlayerCharacter>(GetAttachParentActor())->PerformShootLineTrace();

	FTransform BulletSocket=this->GunMesh->GetSocketTransform("BulletSocket");
	FRotator BulletRotation;
	if(hit.bBlockingHit)
	{
		FVector ShootVector= hit.Location-BulletSocket.GetLocation();
		BulletRotation=ShootVector.ToOrientationRotator();
	}
	else
	{
		BulletRotation=(hit.TraceEnd-hit.TraceStart).ToOrientationRotator();
	}

	
	AProjectile* bullet=GetWorld()->SpawnActor<AProjectile>(ProjectileToSpawn, BulletSocket.GetLocation(),BulletRotation);
	AGrapplePlayerCharacter* myCharacter=Cast<AGrapplePlayerCharacter>(this->GunMesh->GetAttachParent()->GetOwner());
	bullet->OnHitTargetEvent.AddUFunction(myCharacter, "IncreaseScore");
	OnShoot();
}

void AGun::OnShoot_Implementation()
{
}


void AGun::ReleaseTrigger()
{
	bFirstShot=true;
}

void AGun::RefillAmmo()
{
	this->Ammo=this->MaxAmmo;
}


void AGun::SetGunState(EGunState NewState)
{
	State=NewState;
	OnGunStateChanged.Broadcast(NewState);
}



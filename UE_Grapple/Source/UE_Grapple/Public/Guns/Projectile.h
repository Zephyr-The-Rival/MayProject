// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UArrowComponent;
DECLARE_MULTICAST_DELEGATE(FHitDelegate);
UCLASS()
class UE_GRAPPLE_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* CollisionSphere;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ProjectileSpeed=100000;
public:
	FHitDelegate OnHitTargetEvent;
	
};

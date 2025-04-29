// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleProjectile.generated.h"

DECLARE_MULTICAST_DELEGATE(Event);

class UNiagaraSystem;
UCLASS()
class UE_GRAPPLE_API AGrappleProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrappleProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ProjectileSpeed=3000;

private:
	bool Move=true;

public:
	void StopMove();
	Event OnHit;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UNiagaraSystem* SparksOnHit;

};

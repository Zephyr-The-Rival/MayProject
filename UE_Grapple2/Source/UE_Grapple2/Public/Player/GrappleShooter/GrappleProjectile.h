﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleProjectile.generated.h"

class USphereComponent;
class UCapsuleComponent;

UCLASS()
class UE_GRAPPLE2_API AGrappleProjectile : public AActor
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ProjectileMesh;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SpinAnchor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SpinComponent;
};

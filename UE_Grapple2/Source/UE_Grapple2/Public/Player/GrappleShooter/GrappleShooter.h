// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleShooter.generated.h"

class AGrapplePlayerCharacter;
class AGrappleProjectile;
class UCableComponent;
class UNiagaraSystem;


UCLASS()
class UE_GRAPPLE2_API AGrappleShooter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrappleShooter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Component
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* GrappleShooterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCableComponent* CableComponent;

	//Functionality
public:
	UFUNCTION(BlueprintNativeEvent)
	void Pressed();
	void Pressed_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void PullPressed();
	void PullPressed_Implementation();


	UFUNCTION(BlueprintNativeEvent)
	void Released();
	void Released_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void PullReleased();
	void PullReleased_Implementation();
	
};

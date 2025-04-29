// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrappleMovementComponent.generated.h"

class AGrapplePlayerCharacter;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None UMETA(Hidden),
	CMOVE_WallRun UMETA(DisplayName="Wall Run"),
	CMOVE_MAX UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWallrunStartEvent, bool, bRight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWallrunEndEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLandedEvent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable)
class UE_GRAPPLE_API UGrappleMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrappleMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;


public:
	FOnLandedEvent OnLanded;
private:
	bool bWasInAirLastFrame=false; 
protected:
	UPROPERTY(Transient)
	AGrapplePlayerCharacter* MyGrapplePlayerCharacter;

	virtual void InitializeComponent() override;

public:
	UFUNCTION(BlueprintPure)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode)const;

public:
	
	void StartWallrun(bool bRightSide, FVector SurfaceNormal);
	void EndWallrun();
	void SetWallrunInput(FVector2d Input);
	void JumpOffWall();
	FWallrunStartEvent OnWallrunStart;
	FWallrunEndEvent OnWallrunEnd;
private:
	bool bWallrunAllowed=true;
	void AllowWallrun();
	bool bWallrunRight;
	
	void PhysWallrun(float deltaTime, int32 Iterations);

	FHitResult WallrunLineTrace(bool bRight);

	float WallrunHorizontalVelocity;
	float WallrunZValue;

	float Wallrun_DecreaseZValue(float deltaTime);
	float Wallrun_CalculateInitalHorizonatlVelocity(FVector SurfaceNormal);
	FVector GetWallrunDirection(bool bRight); 

	FVector2d WallrunInput;
	FVector currentWallrunNormal;
public:
	UFUNCTION(BlueprintCallable)
	FVector GetCurrentWallrunNormal() const {return this->currentWallrunNormal;}
public:
	//CustomParameters

	//how much the character accellerates based on input
	UPROPERTY(EditDefaultsOnly) float WallrunHorizontalAcceleration=800;
	UPROPERTY(EditDefaultsOnly) float WallrunHorizontalFriction=200;

	UPROPERTY(EditDefaultsOnly) float WallrunMaxSpeed=2000;

	UPROPERTY(EditDefaultsOnly) float WallrunMinSpeed=300;

	//Speed at which the vertical velocity value changes towards 0 while wallrunning (gets multiplied with deltatime)
	UPROPERTY(EditDefaultsOnly) float WallrunVerticalBreakFactor=1000;

	UPROPERTY(EditDefaultsOnly) float WallrunJumpOffFoce=100000;

	
};



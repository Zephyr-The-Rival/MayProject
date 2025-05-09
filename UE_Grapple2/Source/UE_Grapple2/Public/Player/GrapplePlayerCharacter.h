// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GrapplePlayerCharacter.generated.h"

class USlide;
class UWallBounce;
class UWallrun;
class AGrappleShooter;
class UCameraComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSingleEvent);

UCLASS()
class UE_GRAPPLE2_API AGrapplePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGrapplePlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//Components_____
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* LowerBodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UChildActorComponent* GrappleShooterCA;

	//mechanic components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWallrun* Wallrunner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWallBounce* WallBouncer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USlide* Slider;
	
	//Input_____
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* ShootGrapplingHookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* SlideAction;

protected:
	//movement
	//walking
	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Walking")
	float WalkingSpeed = 300;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Walking")
	float SprintingSpeed = 600;


private:
	UPROPERTY(EditAnywhere,BlueprintGetter=GetIsSprinting)
	bool bIsSprinting=false;
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsSprinting() const{return this->bIsSprinting;}
private:
	void SprintButtonDown();
	void SprintButtonUp();
	void StartSprinting();
	void StopSprinting();
	//Jumping
	void JumpButtonDown();
	void EndJump();

public:
	UPROPERTY(EditAnywhere,BlueprintAssignable)
	FSingleEvent OnStartJump;
	UPROPERTY(EditAnywhere,BlueprintAssignable)
	FSingleEvent OnEndJump;

	//looking
protected:
	
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Looking")
	float TurningSpeed=1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Looking")
	float PitchLimit = 70;

	//Grapple
protected:
	void ShootGrapplePressed();
	void ShootGrappleEnd();

private:
	AGrappleShooter* MyGrappleShooter;
};

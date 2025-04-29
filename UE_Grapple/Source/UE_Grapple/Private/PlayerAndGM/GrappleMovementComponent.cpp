// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAndGM/GrappleMovementComponent.h"

#include "Debug.h"
#include "GrappleShooter/GrappleShooter.h"
#include "Physics/ImmediatePhysics/ImmediatePhysicsShared/ImmediatePhysicsCore.h"
#include "PlayerAndGM/GrapplePlayerCharacter.h"


// Sets default values for this component's properties
UGrappleMovementComponent::UGrappleMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrappleMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGrappleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(bWasInAirLastFrame&& !IsFalling())
	{
		OnLanded.Broadcast();
	}
		
	
	bWasInAirLastFrame=IsFalling();
}

void UGrappleMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case ECustomMovementMode::CMOVE_WallRun:
		PhysWallrun(deltaTime, Iterations);
		break;
		
	default:
		break;
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UGrappleMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	this->MyGrapplePlayerCharacter= Cast<AGrapplePlayerCharacter>(GetOwner());
}

bool UGrappleMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UGrappleMovementComponent::StartWallrun(bool bRightSide, FVector SurfaceNormal)
{
	if(!bWallrunAllowed)
		return;
	
	MyGrapplePlayerCharacter->MyGrappleShooter->LetGo();

	
	GravityScale=0;
	this->bWallrunRight=bRightSide;
	
	this->WallrunZValue=this->Velocity.Z;
	this->WallrunHorizontalVelocity=Wallrun_CalculateInitalHorizonatlVelocity(SurfaceNormal);
	
	SetMovementMode(MOVE_Custom, CMOVE_WallRun);
	this->OnWallrunStart.Broadcast(bRightSide);
}

void UGrappleMovementComponent::PhysWallrun(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
		return;
	
	RestorePreAdditiveRootMotionVelocity();


	//Velocity+=FVector::DownVector*deltaTime*WallrunGravity;//applyGravity
	
	
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, 0.1, false, GetMaxBrakingDeceleration());//apparently friction only in fluid??
	}
	ApplyRootMotionToVelocity(deltaTime);

	
	FQuat AbsoluteRotation =FQuat::MakeFromRotator(MyGrapplePlayerCharacter->GetActorRotation());
	Iterations++;
	bJustTeleported=false;
	FHitResult SweepHit= FHitResult();
	FVector OldLocation=UpdatedComponent->GetComponentLocation();

//____________________________________________________________
	
	FHitResult HitResult =WallrunLineTrace(bWallrunRight);
	this->currentWallrunNormal=HitResult.Normal;
	if(!HitResult.bBlockingHit || Velocity.Length()<this->WallrunMinSpeed)
	{
		EndWallrun();
		StartNewPhysics(deltaTime,Iterations);
		return;
	}
	
	float vectorRotation;
	if(bWallrunRight)
		vectorRotation=90;
	else
		vectorRotation=-90;
	
	//sets the direction to walk in. value is normalized
	FVector WallrunDirection = HitResult.Normal.RotateAngleAxis(vectorRotation,FVector(0,0,1));
	
	
	
	if(WallrunInput==FVector2d::Zero()||WallrunHorizontalVelocity>WallrunMaxSpeed)
	{
		WallrunHorizontalVelocity-=WallrunHorizontalFriction*deltaTime;
	}
	else
	{
		if(WallrunHorizontalVelocity<WallrunMaxSpeed)
			WallrunHorizontalVelocity+= WallrunInput.X*WallrunHorizontalAcceleration*deltaTime;
		else
			WallrunHorizontalVelocity=WallrunMaxSpeed;
	}

	WallrunInput=FVector2d::Zero();//reset wallrun input to zero because move input event isnt called on 0 value

	Velocity=WallrunDirection * this->WallrunHorizontalVelocity;
	
	WallrunZValue= Wallrun_DecreaseZValue(deltaTime);
	Velocity.Z=WallrunZValue;
	

	
	FVector DeltaPosition = Velocity*deltaTime;
	

	
	SafeMoveUpdatedComponent(DeltaPosition, AbsoluteRotation, true,SweepHit);
	
	
	 if(SweepHit.bBlockingHit)
	 {
		 HandleImpact(SweepHit,1-SweepHit.Time,DeltaPosition);
		 EndWallrun();
	 }

	if(!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity= (UpdatedComponent->GetComponentLocation()-OldLocation)/deltaTime;
	}
}

FHitResult UGrappleMovementComponent::WallrunLineTrace(bool bRight)
{
	FVector directionVector=MyGrapplePlayerCharacter->GetActorRightVector();

	if(!bRight)
		directionVector*=-1;
	
	
	
	FHitResult HitResult;

	FVector Start= this->GetActorLocation();
	FVector End =Start+ directionVector * MyGrapplePlayerCharacter->WallrunCheckDistance;

	
	GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_GameTraceChannel1);
	return HitResult;
}

float UGrappleMovementComponent::Wallrun_DecreaseZValue(float deltaTime)
{
	if(abs(WallrunZValue)<10)
		return 0;

	if(WallrunZValue>0)
		return WallrunZValue - WallrunVerticalBreakFactor*deltaTime;
	else
		return WallrunZValue + WallrunVerticalBreakFactor*deltaTime;
}

float UGrappleMovementComponent::Wallrun_CalculateInitalHorizonatlVelocity(FVector SurfaceNormal)
{
	FVector tmpVelocity= this->Velocity;
	tmpVelocity.Z=0;
	tmpVelocity.Normalize();

	float dot = tmpVelocity.Dot(SurfaceNormal); //dot is negative
	
	// the closer the impact angle is to 90 degrees, the slower the inital speed is
	float horizontalVelocity=(this->Velocity*(1+dot)*FVector(1,1,0)).Length();
	if(horizontalVelocity>300)
		return horizontalVelocity;
	else
		return 300;
}

void UGrappleMovementComponent::EndWallrun()
{
	GravityScale=GetDefault<UGrappleMovementComponent>()->GravityScale;
	SetMovementMode(MOVE_Walking);

	this->bWallrunAllowed=false;
	FTimerHandle WallrunCooldownTimer;
	GetWorld()->GetTimerManager().SetTimer(WallrunCooldownTimer, this, &UGrappleMovementComponent::AllowWallrun, 0.5,false);
	this->OnWallrunEnd.Broadcast();
}

void UGrappleMovementComponent::SetWallrunInput(FVector2d Input)
{
	WallrunInput=Input;
}

void UGrappleMovementComponent::JumpOffWall()
{
	EndWallrun();

	FVector JumpVector = FVector();
	

	JumpVector= FVector::UpVector*2 + currentWallrunNormal;

	JumpVector*=WallrunJumpOffFoce;

	AddImpulse(JumpVector);

}

void UGrappleMovementComponent::AllowWallrun()
{
	this->bWallrunAllowed=true;
}


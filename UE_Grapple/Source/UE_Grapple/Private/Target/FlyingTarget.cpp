// Fill out your copyright notice in the Description page of Project Settings.


#include "Target/FlyingTarget.h"

#include "Debug.h"
#include "EngineUtils.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerAndGM/GrapplePlayerCharacter.h"
#include "Target/TargetBounds.h"
#include "Windows/AllowWindowsPlatformTypes.h"


// Sets default values
AFlyingTarget::AFlyingTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh=CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
}

// Called when the game starts or when spawned
void AFlyingTarget::BeginPlay()
{
	Super::BeginPlay();
	State=ETargetState::Idle;

	
	MyBounds = Cast<ATargetBounds>(UGameplayStatics::GetActorOfClass(GetWorld(),ATargetBounds::StaticClass()));
	if(MyBounds)
		LookForNewLocation();
	else
		Debug::Print("Target Bounds not found!");
	
}

// Called every frame
void AFlyingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(State==ETargetState::MovingToNewPosition)
	{
		float currentDistance= FVector::Distance(GetActorLocation(), NewLoaction);
		if(distanceLastFrame<currentDistance)
		{
			ArrivedAtNewLocation();
		}
		distanceLastFrame=currentDistance;
			

		AddActorWorldOffset(NormalizedMovementDirection*DeltaTime*MovementSpeed);
	}
}

void AFlyingTarget::LookForNewLocation()
{

	TArray<FVector>SearchPoints= GenerateSearchPoints();
	TArray<FVector>ViablePoints;

	for (FVector SearchPoint : SearchPoints)
	{
		FHitResult hit=PerformCapsuleTrace(SearchPoint);

		//condition of what points are viable
		if (IsLocationValid(&hit))
			ViablePoints.Add(SearchPoint);
	}

	if(ViablePoints.IsEmpty())
	{
		NewLoaction=OldLocation;
	}
	else
	{
		NewLoaction=ViablePoints[FMath::FRandRange(0.0f, ViablePoints.Num()-1)];
	}
	OldLocation=GetActorLocation();
	NormalizedMovementDirection=NewLoaction-GetActorLocation();
	NormalizedMovementDirection.Normalize();

	distanceLastFrame=FLT_MAX;
	
	State = ETargetState::MovingToNewPosition;
}

void AFlyingTarget::ArrivedAtNewLocation()
{
	SetActorLocation(NewLoaction);
	State=ETargetState::Idle;

	FTimerHandle IdleTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(IdleTimerHandle,this, &AFlyingTarget::LookForNewLocation, FMath::FRandRange(1.0,2.0),false);
}

FHitResult AFlyingTarget::PerformCapsuleTrace(FVector EndPoint)
{
	FHitResult HitResult;

	
	ECollisionChannel TraceChannel = ECC_Visibility;

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true; 
	QueryParams.AddIgnoredActor(this);

	// Perform the trace
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		EndPoint,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeCapsule(this->TraceCapsuleSize, this->TraceCapsuleSize),
		QueryParams
	);

	if(this->bShowDebug)
	{
		FColor DebugColor = FColor::Red;
		if(IsLocationValid(&HitResult))
			DebugColor=FColor::Green;
		
		DrawDebugCapsule(GetWorld(), GetActorLocation(), this->TraceCapsuleSize, this->TraceCapsuleSize, FQuat::Identity, FColor::Cyan, false, 2.0f);
		DrawDebugCapsule(GetWorld(), EndPoint, this->TraceCapsuleSize, this->TraceCapsuleSize, FQuat::Identity, DebugColor, false, 2.0f);
		DrawDebugLine(GetWorld(), GetActorLocation(), EndPoint, DebugColor, false, 2.0f, 0, 2.0f);	
	}
	

	return HitResult;
}

TArray<FVector> AFlyingTarget::GenerateSearchPoints()
{
	TArray<FVector> Points;
	
	float ZValue= GetActorLocation().Z-(this->HeightLevels/2)*SpaceBetweenLevels; //starting height for generating circles

	if(this->HeightLevels%2==0)
		ZValue+=SpaceBetweenLevels/2.0f;
	
	float AngleStepSize=360/PointsPerCircle;

	
	for (int l = 0; l < this->HeightLevels; l++)
	{
		for(int i=1; i<=NumOfCirclesPerLevel; i++)
		{
			for (int c = 0; c < PointsPerCircle; c++)
			{
				FVector RotationCenter= GetActorLocation();
				RotationCenter.Z=ZValue;
				FVector point= RotationCenter+ FVector(this->CircleRadius*i,0,0).RotateAngleAxis(AngleStepSize*c, FVector::UpVector);
				//DrawDebugCapsule(GetWorld(), point, 50, 50, FQuat::Identity, FColor::Cyan, false, 10.0f);
				Points.Add(point);
			}
		}
		
		ZValue += SpaceBetweenLevels;
	}
	
	return Points;
}

bool AFlyingTarget::IsLocationValid(FHitResult* HitResult)
{
	return (!HitResult->bBlockingHit && MyBounds->IsLocationInBounds(HitResult->TraceEnd));
}


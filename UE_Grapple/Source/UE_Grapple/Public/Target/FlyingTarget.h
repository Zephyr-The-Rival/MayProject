// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlyingTarget.generated.h"

class ATargetBounds;

UENUM(BlueprintType)
enum class ETargetState: uint8
{
	Idle UMETA(DisplayName="Idle"),
	LookingForPosition UMETA(DisplayName="Looking for Position"),
	MovingToNewPosition UMETA(DisplayName="Moving to new Position")

};
UCLASS()
class UE_GRAPPLE_API AFlyingTarget : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyingTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;


	
private:
	ETargetState State;
	FVector NewLoaction;
	FVector NormalizedMovementDirection;
	FVector OldLocation;
	float distanceLastFrame;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MovementSpeed=700;

private:

	void LookForNewLocation();
	void ArrivedAtNewLocation();
	FHitResult PerformCapsuleTrace(FVector EndPoint);
	TArray<FVector> GenerateSearchPoints();
	bool IsLocationValid(FHitResult* HitResult);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation")
	bool bShowDebug=false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation")
	float TraceCapsuleSize=50;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation")
	int32 HeightLevels=4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation")
	float SpaceBetweenLevels=200;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation")
	int32 PointsPerCircle=5;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation")
	int32 CircleRadius=300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation")
	int32 NumOfCirclesPerLevel=3;

private:
	ATargetBounds* MyBounds;
};

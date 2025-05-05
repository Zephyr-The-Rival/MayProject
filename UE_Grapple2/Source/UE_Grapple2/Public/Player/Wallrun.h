// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Wallrun.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class UE_GRAPPLE2_API UWallrun : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWallrun();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartWallrun();
	void StartWallrun_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndWallrun();
	void EndWallrun_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void JumpOff();
	void JumpOff_Implementation();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWallrunning;
};


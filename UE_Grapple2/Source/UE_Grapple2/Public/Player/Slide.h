// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Slide.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class UE_GRAPPLE2_API USlide : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USlide();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartSlide();
	void StartSlide_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndSlide();
	void EndSlide_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSlidingButtonDown=false;
};

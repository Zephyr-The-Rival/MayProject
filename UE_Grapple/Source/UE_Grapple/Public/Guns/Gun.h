// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class AProjectile;

UENUM(BlueprintType)
enum class EGunState: uint8
{
	Idle UMETA(DisplayName="Idle"),
	Shooting UMETA(DisplayName="Shooting"),
	SoftCoolDown UMETA(DisplayName="SoftCoolDown"),
	Reloading UMETA(DisplayName="Reloading")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeStateDelegate, EGunState, newState);

UCLASS()
class UE_GRAPPLE_API AGun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAutomatic=false;


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int MaxAmmo=6;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<AProjectile> ProjectileToSpawn;

private: int Ammo=6;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetAmmo() const{return this->Ammo;}

public:
	void PullTrigger();
	void ReleaseTrigger();

	UFUNCTION(BlueprintCallable)
	void RefillAmmo();
	//The shoot animation takes longer then the shoot cycle so it enters a soft cool down first
	//this function is called at the end of a shooting animatoin so it can ender idle again

	FChangeStateDelegate OnGunStateChanged;
	UFUNCTION(BlueprintCallable)
	void SetGunState(EGunState NewState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGunState State=EGunState::Idle;
private:
	void Shoot();
	bool bFirstShot=true;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnShoot();
	void OnShoot_Implementation();
	
};

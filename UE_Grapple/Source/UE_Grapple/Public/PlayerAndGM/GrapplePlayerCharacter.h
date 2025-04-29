#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GrapplePlayerCharacter.generated.h"

//UInputAction;

enum class EGrappleState : uint8;
enum class EGunState : uint8;
class AGun;
class UGrappleMovementComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class AGrappleShooter;

struct FMovementDirecions
{
	bool bForward = false;
	bool bBackward = false;
	bool bRight = false;
	bool bLeft = false;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSingleEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSystemChangedEvent, bool, bUsingEnhanced);


UCLASS()
class UE_GRAPPLE_API AGrapplePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGrapplePlayerCharacter(const FObjectInitializer& ObjectInitializer);

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

	//ToggleSystems
public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsingEnhancedSystem=true;
private:
	void ToggleSystem();
public:
	UPROPERTY(BlueprintAssignable)
	FSystemChangedEvent OnSystemChanged;
	
	
//Input_____
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputMappingContext* DefaultMappingContext_MaK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputMappingContext* DefaultMappingContext_GamePad;
	
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
	UInputAction* ShootGunAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* ReloadAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* ToggleSystemAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* PullBoostAction;


//Movement_____
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	class UGrappleMovementComponent* CharacterMovementComponent;
	FMovementDirecions CurrentMovementDirection;

public:
	UGrappleMovementComponent* GetCustomMovementComponent() const { return this->CharacterMovementComponent;}
protected:
	
	//looking
	void Look(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Looking")
	float TurningSpeed=1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Looking")
	float PitchLimit = 70;

	//Jumping
	void JumpButtonDown();
	void EndJump();
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSingleEvent OnStartJump;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSingleEvent OnEndJump;

	//for animation blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bJumping=false;

protected:
	//walking
	void Move(const FInputActionValue& Value);
	void EndMove();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Walking")
	float WalkingSpeed = 300;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Walking")
	float SprintingSpeed = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement/Walking")
	float EnhancedSprintingSpeed = 800;

private:
	bool bIsSprinting=false;
	void SprintButtonDown();
	void SprintButtonUp();
	void StartSprinting();
	void StopSprinting();

	//Boosting

protected:
	//Amount of seconds where boosting is possible
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement/Boosting")
	float MaxBoostingFuel=1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement/Boosting")
	float BoostingStrenght=15000;

	//How fast boosting refuels per second
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement/Boosting")
	float BoostingRefuelSpeed=0.3;

	//How long until boost starts refueling
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement/Boosting")
	float BoostingRefuelDelay=2;
	
private:
	bool bIsBoosting=false;
	float BoostingFuel=0;
	bool bRefuelBoostAllowed=true;
	

	void StartBoosting();
	void StopBoosting();
	void Tick_ApplyBoost();
	void Tick_RefuelBoost();

	void AllowBoostRefill();
	
	FTimerHandle BoostRefuelTimer;

	//Double Jump

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Enhanced_DoubleJumpStrength=100000;
private:	
	void DoubleJump();
	bool bCanDoubleJump=true;
	UFUNCTION()
	void AllowDoubleJump();
	
	//WallRunning
private:
	void Tick_WallrunCheck();

protected:

	UFUNCTION(BlueprintNativeEvent)
	void Wallrun_TiltMeshToSide(bool bRight);
	void Wallrun_TiltMeshToSide_Implementation(bool bRight);

	UFUNCTION(BlueprintNativeEvent)
	void Wallrun_TiltMeshBack();
	void Wallrun_TiltMeshBack_Implementation();
	
public:
	//How close to the wall the player must be to start the wallrun
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WallrunCheckDistance= 120;
	
	

//Grappling_____
protected:
	void ShootGrapplePressed();
	void ShootGrappleEnd();
	
	UPROPERTY(EditDefaultsOnly)
	UChildActorComponent* GrappleShooterChildActor;

	UFUNCTION(BlueprintNativeEvent)
	void OnGrappleStateChanged(EGrappleState NewState);
	void OnGrappleStateChanged_Implementation(EGrappleState NewState);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AGrappleShooter* MyGrappleShooter;
	
public:
	void Enhanced_StartPullBoost();
	void Enhanced_EndPullBoost();

	
//Debug
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug");
	TSubclassOf<UUserWidget> DebugStats;

	//Other
	void SetDefaultValues();

//UserInterface/UI

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD");
	TSubclassOf<UUserWidget> MainHud;

	//speed field of view feedback
private:
	void Tick_AdjustFieldOfView();

	//Gun 

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AGun* CurrentGun;
private:
	void PickUpGun(AGun* NewGun);
	void PullGunTrigger();
	void ReleaseGunTrigger();
	void ReloadGun();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AGun> StarterGun;

	UFUNCTION(BlueprintNativeEvent)
	void OnGunStateChanged(EGunState newState);
	void OnGunStateChanged_Implementation(EGunState newState);
public:
	FHitResult PerformShootLineTrace();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ScoreCounter=0;

	UFUNCTION()
	void IncreaseScore();
};

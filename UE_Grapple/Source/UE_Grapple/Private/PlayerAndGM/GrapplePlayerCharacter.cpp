#include "UE_Grapple/Public/PlayerAndGM/GrapplePlayerCharacter.h"
#include "Debug.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GrappleShooter/GrappleShooter.h"
#include "Guns/Gun.h"
#include "PlayerAndGM/GrappleMovementComponent.h"
#include "PlayerAndGM/GrapplePC.h"
#include "Target/FlyingTarget.h"


// Sets default values
AGrapplePlayerCharacter::AGrapplePlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UGrappleMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	this->CharacterMovementComponent= Cast<UGrappleMovementComponent>(GetMovementComponent());
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(this->GetMesh(), TEXT("CameraSocket"));

	this->GrappleShooterChildActor = CreateDefaultSubobject<UChildActorComponent>("GrappleShooterChildActor");
	this->GrappleShooterChildActor->SetupAttachment(this->GetMesh(),TEXT("lowerarm_l"));

	
}

// Called when the game starts or when spawned
void AGrapplePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetDefaultValues();

	if (MainHud)
		CreateWidget<UUserWidget>(Cast<AGrapplePC>(GetController()), MainHud)->AddToViewport(0);
	if (DebugStats)
		CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), DebugStats)->AddToViewport(1);

	if(this->MyGrappleShooter)
		MyGrappleShooter->OnGrappleStateStateChanged.AddDynamic(this, &AGrapplePlayerCharacter::OnGrappleStateChanged);
	
	CharacterMovementComponent->OnWallrunStart.AddDynamic(this, &AGrapplePlayerCharacter::Wallrun_TiltMeshToSide);
	CharacterMovementComponent->OnWallrunEnd.AddDynamic(this, &AGrapplePlayerCharacter::Wallrun_TiltMeshBack);

	if(StarterGun)
		PickUpGun(GetWorld()->SpawnActor<AGun>(StarterGun, FVector(), FRotator()));
	else
		Debug::Print("Starter Gun isn't Valid!!!");

	GetCustomMovementComponent()->OnLanded.AddDynamic(this, &AGrapplePlayerCharacter::AllowDoubleJump);
			
}

// Called every frame
void AGrapplePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->bIsBoosting)
		Tick_ApplyBoost();

	if (this->bRefuelBoostAllowed)
		Tick_RefuelBoost();

	if(CharacterMovementComponent->MovementMode!=ECustomMovementMode::CMOVE_WallRun)
		Tick_WallrunCheck();

	Tick_AdjustFieldOfView();
}

// Called to bind functionality to input
void AGrapplePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetController()->GetNetOwningPlayer());


	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	InputSystem->AddMappingContext(DefaultMappingContext_MaK, InputPriority);
	InputSystem->AddMappingContext(DefaultMappingContext_GamePad, InputPriority);


	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGrapplePlayerCharacter::Look);
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGrapplePlayerCharacter::Move);
	Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::EndMove);

	Input->BindAction(JumpAction, ETriggerEvent::Started, this, &AGrapplePlayerCharacter::JumpButtonDown);
	Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::EndJump);

	Input->BindAction(SprintAction, ETriggerEvent::Started, this, &AGrapplePlayerCharacter::SprintButtonDown);
	Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::SprintButtonUp);

	Input->BindAction(ShootGrapplingHookAction, ETriggerEvent::Started, this,&AGrapplePlayerCharacter::ShootGrapplePressed);
	Input->BindAction(ShootGrapplingHookAction, ETriggerEvent::Completed, this,&AGrapplePlayerCharacter::ShootGrappleEnd);

	Input->BindAction(ShootGunAction, ETriggerEvent::Triggered, this, &AGrapplePlayerCharacter::PullGunTrigger);
	Input->BindAction(ShootGunAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::ReleaseGunTrigger);

	Input->BindAction(ReloadAction, ETriggerEvent::Started, this, &AGrapplePlayerCharacter::ReloadGun);
	Input->BindAction(ToggleSystemAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::ToggleSystem);

	//disabled becuase it wasnt apparent enough without feedback
	// Input->BindAction(PullBoostAction, ETriggerEvent::Started, this, &AGrapplePlayerCharacter::Enhanced_StartPullBoost);
	// Input->BindAction(PullBoostAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::Enhanced_EndPullBoost);
}

void AGrapplePlayerCharacter::ToggleSystem()
{
	if(!(this->MyGrappleShooter->GetGrappleState() == EGrappleState::Standby||this->MyGrappleShooter->GetGrappleState() == EGrappleState::SoftCoolDown))
		return;
	
	this->bUsingEnhancedSystem=!this->bUsingEnhancedSystem;
	OnSystemChanged.Broadcast(bUsingEnhancedSystem);
	if(bIsSprinting)
		StartSprinting();//will set the appropriate sprinting speed.

}


void AGrapplePlayerCharacter::Look(const FInputActionValue& Value)
{
	float DeltaYaw = Value.Get<FVector2d>().X * this->TurningSpeed;
	AddControllerYawInput(DeltaYaw);

	float DeltaPitch = Value.Get<FVector2d>().Y * this->TurningSpeed;
	FRotator Rotator = FRotator(DeltaPitch, 0, 0);
	if (abs(GetMesh()->GetRelativeRotation().Pitch + DeltaPitch) < this->PitchLimit)
		GetMesh()->AddLocalRotation(Rotator);
}

void AGrapplePlayerCharacter::JumpButtonDown()
{
	this->bJumping = true;
	
	if (CharacterMovementComponent->MovementMode == MOVE_Custom && CharacterMovementComponent->CustomMovementMode == ECustomMovementMode::CMOVE_WallRun)
	{
		CharacterMovementComponent->JumpOffWall();
		return;
	}
	if (GetMovementComponent()->IsFalling())
	{
		if (bUsingEnhancedSystem)
			DoubleJump();
		else
			StartBoosting();
		
	}
	else
	{
		Jump(); //part of character;
		OnStartJump.Broadcast();
	}
}

void AGrapplePlayerCharacter::EndJump()
{
	this->bJumping = false;
	if (bIsBoosting)
	{
		StopBoosting();
	}
	else
	{
		StopJumping(); //part of character;
		OnEndJump.Broadcast();
	}
}

void AGrapplePlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2d Vector2d = Value.Get<FVector2d>();

	if(this->CharacterMovementComponent->MovementMode==MOVE_Walking)
	{
		FVector DeltaMovement = FVector(Vector2d.X, Vector2d.Y, 0);
		FVector Forward = GetCapsuleComponent()->GetForwardVector() * Vector2d.X;
		FVector Right = GetCapsuleComponent()->GetRightVector() * Vector2d.Y;

		GetMovementComponent()->AddInputVector(Forward + Right);

		if (Vector2d.X<0.5)
		{
			StopSprinting();
		}
	}
	if (CharacterMovementComponent->MovementMode == MOVE_Custom && CharacterMovementComponent->CustomMovementMode == ECustomMovementMode::CMOVE_WallRun)
	{
		CharacterMovementComponent->SetWallrunInput(Vector2d);
	}

	 this->CurrentMovementDirection.bForward=(Vector2d.X>0.5);
	 this->CurrentMovementDirection.bBackward=(Vector2d.X<-0.5);
	 this->CurrentMovementDirection.bRight=(Vector2d.Y>0.5);
	 this->CurrentMovementDirection.bLeft=(Vector2d.Y<-0.5);
}

void AGrapplePlayerCharacter::EndMove()
{
	this->CurrentMovementDirection.bForward=false;
	this->CurrentMovementDirection.bBackward=false;
	this->CurrentMovementDirection.bRight=false;
	this->CurrentMovementDirection.bLeft=false;
}

void AGrapplePlayerCharacter::SprintButtonDown()
{
	AGrapplePC* PlayerController = Cast<AGrapplePC>(GetController());
	if (PlayerController->bIsUsingGamepad)
	{
		if (this->bIsSprinting)
			StopSprinting();
		else
			StartSprinting();
	}
	else
	{
		StartSprinting();
	}
}

void AGrapplePlayerCharacter::SprintButtonUp()
{
	AGrapplePC* PlayerController = Cast<AGrapplePC>(GetController());
	if (!PlayerController->bIsUsingGamepad)
		StopSprinting();
}

void AGrapplePlayerCharacter::StartSprinting()
{
	this->bIsSprinting = true;

	if (bUsingEnhancedSystem)
		CharacterMovementComponent->MaxWalkSpeed = this->EnhancedSprintingSpeed;
	else
		CharacterMovementComponent->MaxWalkSpeed = this->SprintingSpeed;
}

void AGrapplePlayerCharacter::StopSprinting()
{
	this->bIsSprinting = false;
	CharacterMovementComponent->MaxWalkSpeed = this->WalkingSpeed;
}

void AGrapplePlayerCharacter::StartBoosting()
{
	this->bIsBoosting = true;
	this->bRefuelBoostAllowed = false;
	GetWorld()->GetTimerManager().PauseTimer(BoostRefuelTimer);
}

void AGrapplePlayerCharacter::StopBoosting()
{
	this->bIsBoosting = false;
	GetWorld()->GetTimerManager().SetTimer(BoostRefuelTimer, this, &AGrapplePlayerCharacter::AllowBoostRefill,BoostingRefuelDelay, false);
}

void AGrapplePlayerCharacter::Tick_ApplyBoost()
{
	if (BoostingFuel > 0)
	{
		BoostingFuel -= GetWorld()->DeltaTimeSeconds;
		CharacterMovementComponent->AddForce(FVector(0, 0, this->BoostingStrenght)); //deltatime is adjusted for already
	}
	else
	{
		StopBoosting();
	}
}

void AGrapplePlayerCharacter::Tick_RefuelBoost()
{
	if (BoostingFuel == MaxBoostingFuel)
		return;

	if (BoostingFuel < MaxBoostingFuel)
		this->BoostingFuel += GetWorld()->DeltaTimeSeconds * this->BoostingRefuelSpeed;
	else
		BoostingFuel = MaxBoostingFuel;
}

void AGrapplePlayerCharacter::AllowBoostRefill()
{
	this->bRefuelBoostAllowed = true;
}

void AGrapplePlayerCharacter::DoubleJump()
{
	if(!bCanDoubleJump)
		return;
	
	this->bCanDoubleJump=false;
	//GetCustomMovementComponent()->Velocity*=FVector(1,1,0);
	float speed= GetCustomMovementComponent()->Velocity.Length()*2+100000;
	GetCustomMovementComponent()->AddImpulse(FVector(0,0,1)*speed);
}

void AGrapplePlayerCharacter::AllowDoubleJump()
{
	this->bCanDoubleJump=true;
}

void AGrapplePlayerCharacter::Tick_WallrunCheck()
{
	
	if (!GetMovementComponent()->IsFalling())
		return;
	
	// if((CharacterMovementComponent->Velocity*FVector(1,1,0)).Length()<CharacterMovementComponent->WallrunMinSpeed)
	// 	return;

	FHitResult HitResultRight;

	FVector Start= this->GetActorLocation();
	FVector End =Start+ this->GetActorRightVector() * this->WallrunCheckDistance;

	
	GetWorld()->LineTraceSingleByChannel(HitResultRight,Start,End,ECC_GameTraceChannel1);


	float dot=GetActorForwardVector().Dot(HitResultRight.Normal);
	
	if (HitResultRight.bBlockingHit && dot < 0.3f && dot > -0.8f && CurrentMovementDirection.bForward)
	{
		CharacterMovementComponent->StartWallrun(true, HitResultRight.Normal);
	}

	FHitResult HitResultLeft;
	End =Start+ this->GetActorRightVector() * this->WallrunCheckDistance* -1;

	
	GetWorld()->LineTraceSingleByChannel(HitResultLeft,Start,End,ECC_GameTraceChannel1);

	dot=GetActorForwardVector().Dot(HitResultLeft.Normal);
	if(HitResultLeft.bBlockingHit && dot< 0.3f && dot >-0.8f && CurrentMovementDirection.bForward)
	{
		CharacterMovementComponent->StartWallrun(false, HitResultLeft.Normal);
	}
}

void AGrapplePlayerCharacter::Wallrun_TiltMeshToSide_Implementation(bool bRight)
{
}

void AGrapplePlayerCharacter::Wallrun_TiltMeshBack_Implementation()
{
}

void AGrapplePlayerCharacter::ShootGrapplePressed()
{
	if (!IsValid(MyGrappleShooter))
		return;

	if (!(MyGrappleShooter->GetGrappleState() == EGrappleState::Standby || MyGrappleShooter->GetGrappleState() == EGrappleState::SoftCoolDown))
		return;

	if(CurrentGun->State==EGunState::Reloading)
		return;
	
	MyGrappleShooter->SetGrappleState(EGrappleState::ShootingOut); //will trigger the animatoin that will shoot the projectile
}

void AGrapplePlayerCharacter::ShootGrappleEnd()
{
	if (!IsValid(MyGrappleShooter))
		return;

	MyGrappleShooter->LetGo();
}

void AGrapplePlayerCharacter::OnGrappleStateChanged_Implementation(EGrappleState NewState)
{
}

void AGrapplePlayerCharacter::Enhanced_StartPullBoost()
{
	MyGrappleShooter->bPullBoostEnabled=true;
}

void AGrapplePlayerCharacter::Enhanced_EndPullBoost()
{
	MyGrappleShooter->bPullBoostEnabled=false;
}

void AGrapplePlayerCharacter::SetDefaultValues()
{
	CharacterMovementComponent->MaxWalkSpeed = this->WalkingSpeed;

	this->MyGrappleShooter = Cast<AGrappleShooter>(GrappleShooterChildActor->GetChildActor());
	this->BoostingFuel = this->MaxBoostingFuel;
}

void AGrapplePlayerCharacter::Tick_AdjustFieldOfView()
{
	float alpha=CharacterMovementComponent->Velocity.Length()/7000;
	float TargetnewFieldOfView=FMath::Lerp(90,120,alpha);
	
	TargetnewFieldOfView=FMath::Clamp(TargetnewFieldOfView,90,120);
	

	float newFieldOfView=Camera->FieldOfView;
	newFieldOfView=FMath::FInterpTo(newFieldOfView,TargetnewFieldOfView,GetWorld()->DeltaTimeSeconds,10);
	this->Camera->FieldOfView=newFieldOfView;
}

void AGrapplePlayerCharacter::PickUpGun(AGun* NewGun)
{
	FAttachmentTransformRules rules= FAttachmentTransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,false);
	NewGun->AttachToComponent(this->GetMesh(),rules, "weapon_rSocket");
	this->CurrentGun=NewGun;
	CurrentGun->OnGunStateChanged.AddDynamic(this,&AGrapplePlayerCharacter::OnGunStateChanged);
}

void AGrapplePlayerCharacter::PullGunTrigger()
{
	if(!CurrentGun)
		return;
	
	if(!(CurrentGun->State==EGunState::Idle||CurrentGun->State==EGunState::SoftCoolDown))
		return;

	
	if(CurrentGun->GetAmmo()==0)
		ReloadGun();
	else
		CurrentGun->PullTrigger();
}

void AGrapplePlayerCharacter::ReleaseGunTrigger()
{
	if(!CurrentGun)
		return;
	
	CurrentGun->ReleaseTrigger();
}

void AGrapplePlayerCharacter::ReloadGun()
{
	if(CurrentGun->GetAmmo()==CurrentGun->MaxAmmo)
		return;
	
	ShootGrappleEnd();
	//MyGrappleShooter->SetGrappleState(EGrappleState::Standby);

	CurrentGun->SetGunState(EGunState::Reloading);
}

void AGrapplePlayerCharacter::OnGunStateChanged_Implementation(EGunState newState)
{
	
}

FHitResult AGrapplePlayerCharacter::PerformShootLineTrace()
{
	FHitResult hit;
	FVector Start= Camera->GetComponentLocation();
	FVector End= Start+Camera->GetForwardVector()*30000; 
	
	GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility);

	return hit;
}

void AGrapplePlayerCharacter::IncreaseScore()
{
		this->ScoreCounter+=100;	
}

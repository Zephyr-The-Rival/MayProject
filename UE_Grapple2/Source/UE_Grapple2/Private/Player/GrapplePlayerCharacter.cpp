// Fill out your copyright notice in the Description page of Project Settings.


#include "UE_Grapple2/Public/Player/GrapplePlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/GrappleShooter/GrappleShooter.h"
#include "UE_Grapple2/Public/Player/GrapplePlayerController.h"


// Sets default values
AGrapplePlayerCharacter::AGrapplePlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(this->GetMesh(), TEXT("CameraSocket"));

	this->ThirdPersonMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Third Person Mesh"));
	ThirdPersonMesh->SetupAttachment(GetCapsuleComponent());

	this->GrappleShooterCA=CreateDefaultSubobject<UChildActorComponent>(TEXT("GrappleShooterChildActor"));
	this->GrappleShooterCA->SetupAttachment(this->GetMesh());//attachment to bone happens in GrappleShooter.BeginPlay() bc it didnt work here
}

// Called when the game starts or when spawned
void AGrapplePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	this->GetCharacterMovement()->MaxWalkSpeed=this->WalkingSpeed;
	this->MyGrappleShooter=Cast<AGrappleShooter>(this->GrappleShooterCA->GetChildActor());
}

// Called every frame
void AGrapplePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGrapplePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetController()->GetNetOwningPlayer());


	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	InputSystem->AddMappingContext(DefaultMappingContext, InputPriority);
	//InputSystem->AddMappingContext(DefaultMappingContext_GamePad, InputPriority);


	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGrapplePlayerCharacter::Look);
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGrapplePlayerCharacter::Move);

	Input->BindAction(JumpAction, ETriggerEvent::Started, this, &AGrapplePlayerCharacter::JumpButtonDown);
	Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::EndJump);

	Input->BindAction(SprintAction, ETriggerEvent::Started, this, &AGrapplePlayerCharacter::SprintButtonDown);
	Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::SprintButtonUp);

	Input->BindAction(ShootGrapplingHookAction, ETriggerEvent::Started, this,&AGrapplePlayerCharacter::ShootGrapplePressed);
	Input->BindAction(ShootGrapplingHookAction, ETriggerEvent::Completed, this,&AGrapplePlayerCharacter::ShootGrappleEnd);

	// Input->BindAction(ShootGunAction, ETriggerEvent::Triggered, this, &AGrapplePlayerCharacter::PullGunTrigger);
	// Input->BindAction(ShootGunAction, ETriggerEvent::Completed, this, &AGrapplePlayerCharacter::ReleaseGunTrigger);
	//
	// Input->BindAction(ReloadAction, ETriggerEvent::Started, this, &AGrapplePlayerCharacter::ReloadGun);
}

void AGrapplePlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2d Vector2d = Value.Get<FVector2d>();
	
		FVector DeltaMovement = FVector(Vector2d.X, Vector2d.Y, 0);
		FVector Forward = GetCapsuleComponent()->GetForwardVector() * Vector2d.X;
		FVector Right = GetCapsuleComponent()->GetRightVector() * Vector2d.Y;

		this->AddMovementInput(Forward + Right);

		if (Vector2d.X<0.5)
		{
			StopSprinting();
		}
	
}

void AGrapplePlayerCharacter::SprintButtonDown()
{
	AGrapplePlayerController* PlayerController = Cast<AGrapplePlayerController>(GetController());
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
	AGrapplePlayerController* PlayerController = Cast<AGrapplePlayerController>(GetController());
	if (!PlayerController->bIsUsingGamepad)
		StopSprinting();
}

void AGrapplePlayerCharacter::StartSprinting()
{
	this->bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = this->SprintingSpeed;
}

void AGrapplePlayerCharacter::StopSprinting()
{
	this->bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = this->WalkingSpeed;
}

void AGrapplePlayerCharacter::JumpButtonDown()
{
	Jump(); //part of character;
	OnStartJump.Broadcast();
}

void AGrapplePlayerCharacter::EndJump()
{
	StopJumping(); //part of character;
	OnEndJump.Broadcast();
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

void AGrapplePlayerCharacter::ShootGrapplePressed()
{
	if(MyGrappleShooter)
		this->MyGrappleShooter->Pressed();
}

void AGrapplePlayerCharacter::ShootGrappleEnd()
{
	if(MyGrappleShooter)
		this->MyGrappleShooter->Released();
}


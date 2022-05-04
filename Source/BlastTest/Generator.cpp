// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator.h"
#include "Net/UnrealNetwork.h"
#include "ZoneToolCon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGenerator::AGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	GenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GenMesh"));
	PointSpawn= CreateDefaultSubobject<USceneComponent>(TEXT("PointObjectSpawn"));
	RootComponent = Scene;
	if(GenMesh)
	{
		GenMesh->SetupAttachment(Scene);

	}
	TimeToGenerate = 30.0;
	CurrentTime = 0.0;	
	IsActive = false;
	IsInPlay = false;
	SpawnObject = nullptr;
	AllWaysInPlay = false;
	ClientSound = false;
	
	if(!AudioComponent)
	{
		AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
		AudioComponent->SetIsReplicated(true);
	}
	if(!ActiveAudioComponent)
	{
		ActiveAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ActiveAudioComponent"));
		ActiveAudioComponent->SetIsReplicated(true);
	}
	
}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->SetWorldLocation(GetActorLocation());
	ActiveAudioComponent->SetWorldLocation(GetActorLocation());

}

void AGenerator::SetActive_Implementation(bool ActiveParma)
{
	//if InPlay is true then it can be interacted with
	if(IsInPlay == true)
	{
		IsActive = ActiveParma;
		if( ActiveParma )
		{
			if(!ActiveAudioComponent->IsPlaying())
			{
				SpawnSound(ActiveSound, 2);
				/*
				ActiveAudioComponent->SetSound(ActiveSound);
				ActiveAudioComponent->FadeIn(0.1f, 1.0f);*/
			}
		}
		
	}
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsActive)
	{
		CurrentTime += DeltaTime;
		if(CurrentTime >= TimeToGenerate)
		{
			SpawnScoreObject();
			SetActive(false);
			SetInPlay(false);
			CurrentTime = 0;
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, "tick");
	if(IsInPlay)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, "isinplay");
		if(GetLocalRole() == ROLE_Authority)
		{
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(),IdleSound, GetActorLocation());
			if(!AudioComponent->IsPlaying())
			{
				SpawnSound(IdleSound, 1);
			
			}
		}
		
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Client");
			if(!ClientSound)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Spawn client sound");
				//SpawnSound(IdleSound, 1);
				UGameplayStatics::PlaySoundAtLocation(GetWorld(),IdleSound, GetActorLocation());
				ClientSound = true;
				
			}
		}
		
	}
	
}

void AGenerator::GetLifetimeReplicatedProps(TArray <FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate the active status of the generator.
	DOREPLIFETIME(AGenerator, IsActive);
	DOREPLIFETIME(AGenerator, IsInPlay);
}


void AGenerator::SetInPlay_Implementation(bool ActiveParma)
{
	IsInPlay = ActiveParma;
	
}


void AGenerator::SpawnScoreObject()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnPara;
		SpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector Location = this->GetActorLocation()+PointSpawn->GetComponentLocation();
		GetWorld()->SpawnActor<AActor>(SpawnObject, Location,FRotator(0,0,0),SpawnPara);

		SpawnSound(CompletedSound, 2);
		
		/*
			ActiveAudioComponent->FadeOut(0.01f, 0.0f);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "PartDropped");
			ActiveAudioComponent->SetSound(CompletedSound);
			ActiveAudioComponent->FadeIn(0.1f, 1.0f);*/
		
		
		
	}
}


void AGenerator::SpawnSound(USoundBase* Sound, int AudioComponentNum)
{
	ServerSound(Sound, AudioComponentNum);	// run the server function for the vfx
}

void AGenerator::ServerSound_Implementation(USoundBase* Sound, int AudioComponentNum)
{
	MultiSound(Sound, AudioComponentNum);	// run the multicast from the server function to correctly spawn sound
}

void AGenerator::MultiSound_Implementation(USoundBase* Sound, int AudioComponentNum)
{
	UAudioComponent*  ChosenAudioComp = nullptr; 
	switch(AudioComponentNum)
	{
	case 1: ChosenAudioComp = AudioComponent;
		break;
	case 2: ChosenAudioComp = ActiveAudioComponent;
		break;
	default: break;
	}
	if(ChosenAudioComp)
	{
		
		if(ChosenAudioComp->IsPlaying())
		{
			ChosenAudioComp->FadeOut(0.1,0);
		}
		
		if(Sound)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f,FColor::Purple,Sound->GetName());
			ChosenAudioComp->SetSound(Sound);
			ChosenAudioComp->FadeIn(0.1f);
			
			//UGameplayStatics::SpawnSoundAttached(Sound, GetInstigator()->GetRootComponent());
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Purple,"No "+Sound->GetName());
		}
	}
}

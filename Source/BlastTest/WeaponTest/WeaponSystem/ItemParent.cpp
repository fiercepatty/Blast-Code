// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemParent.h"

#include "HealthDrop.h"
#include "ModParent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemParent::AItemParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AItemParent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemParent, LifeTime);
}


// Called every frame
void AItemParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!bNeverDestroy)	// only do things if this mod is able to be destroyed
	{
		LifeTime-=DeltaTime;	// decrease its lifetime over time
		if(LifeTime<0)
		{	
			Destroy();		// destroy if needed
		} else
		{
			if (!bHasSpeedUpOnce && LifeTime < (MaxLifeTime/3))	// if lifetime is x of the max lifetime 
			{
				// decrease the blink time
				bHasSpeedUpOnce = true;	// only lets the first speed up happen once
				ResetDropEffectBlinkTimerOff = ResetDropEffectBlinkTimerOff / 2.0;
				ResetDropEffectBlinkTimerOn = ResetDropEffectBlinkTimerOn / 2.0;
			} else if (!bHasSpeedUpTwice && LifeTime < (MaxLifeTime/4))
			{
				// decrease the blink time even further
				bHasSpeedUpTwice = true;	// only lets the second speed up happen once
				ResetDropEffectBlinkTimerOff = ResetDropEffectBlinkTimerOff / 2.0;
				ResetDropEffectBlinkTimerOn = ResetDropEffectBlinkTimerOn / 2.0;
			}
		}

		AModParent* Mod = Cast<AModParent>(this);	// try casting ourselves into a mod
		if (Mod)
		{
			if (!Mod->DropEffect || !Mod->MeshComponent)
				return;
			if (LifeTime < (MaxLifeTime/2))	// start blinking after x fraction of the lifetime
			{
				if( Mod->DropEffect->IsVisible() )	// the effect is on
				{
					DropEffectBlinkTimerOn -= DeltaTime;
					if (DropEffectBlinkTimerOn < 0.f)
					{
						Mod->DropEffect->SetVisibility(false);	// turn back on
						DropEffectBlinkTimerOn = ResetDropEffectBlinkTimerOn; // reset timer
					}
			
				} else		// effect is off
				{
					DropEffectBlinkTimerOff -= DeltaTime;
					if (DropEffectBlinkTimerOff < 0.f)
					{
						Mod->DropEffect->SetVisibility(true);	// turn back on
						DropEffectBlinkTimerOff = ResetDropEffectBlinkTimerOff; // reset timer
					}
				}
			}
		}
		AHealthDrop* Health = Cast<AHealthDrop>(this);
		if (Health)
		{
			if (!Health->DropEffect || !Health->MeshComponent)
				return;
			if (LifeTime < (MaxLifeTime/2))	// start blinking after x fraction of the lifetime
			{
				if( Health->DropEffect->IsVisible() )	// the effect is on
				{
					DropEffectBlinkTimerOn -= DeltaTime;
					if (DropEffectBlinkTimerOn < 0.f)
					{
						Health->DropEffect->SetVisibility(false);	// turn back on
						DropEffectBlinkTimerOn = ResetDropEffectBlinkTimerOn; // reset timer
					}
			
				} else		// effect is off
				{
					DropEffectBlinkTimerOff -= DeltaTime;
					if (DropEffectBlinkTimerOff < 0.f)
					{
						Health->DropEffect->SetVisibility(true);	// turn back on
						DropEffectBlinkTimerOff = ResetDropEffectBlinkTimerOff; // reset timer
					}
				}
			}
		}
	}
}

void AItemParent::BeginPlay()
{
	Super::BeginPlay();
	// set our initial values
	MaxLifeTime = LifeTime;
	ResetDropEffectBlinkTimerOff = DropEffectBlinkTimerOff;
	ResetDropEffectBlinkTimerOn = DropEffectBlinkTimerOn;
}


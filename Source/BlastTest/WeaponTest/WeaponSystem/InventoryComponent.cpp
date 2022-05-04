// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/InventoryComponent.h"

#include <string>

#include "HealthDrop.h"
#include "NetworkChar.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// set our current health to max at start
	CurrentPlayerHealth = MaxPlayerHealth;
	
	//SetIsReplicated(true);	// replicate this component

	

	

	//Health Regen Logic
	
}

UInventoryComponent::~UInventoryComponent()
{
	/*if (DefaultMod)
		DefaultMod->Destroy();
	if (ModSlot1)
		ModSlot1->Destroy();
	if (ModSlot2)
		ModSlot2->Destroy();*/
}



void UInventoryComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(UInventoryComponent, OwningController);
	DOREPLIFETIME(UInventoryComponent, CurrentPlayerHealth);
	DOREPLIFETIME(UInventoryComponent, DefaultMod);
	DOREPLIFETIME(UInventoryComponent, ModSlot1);
	DOREPLIFETIME(UInventoryComponent, ModSlot2);
	DOREPLIFETIME(UInventoryComponent, ActiveModAmmoCount);
	DOREPLIFETIME(UInventoryComponent, SecondaryModAmmoCount);
	DOREPLIFETIME(UInventoryComponent, ActiveModSlot);
	DOREPLIFETIME(UInventoryComponent, MaxActiveModAmmoCount);
	DOREPLIFETIME(UInventoryComponent, CurrentNumOfGeneratorPoints);
}

void UInventoryComponent::OnRep_CurrentHealth()
{
	OnHealthUpdate();	// when server updates health and it gets replicated to client this method will run OnHealthUpdate()
}

void UInventoryComponent::OnHealthUpdate()
{
	//Client-specific functionality
	if (GetOwner()->GetInstigator()->IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentPlayerHealth);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CurrentPlayerHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	//Server-specific functionality
	if (GetOwner()->GetInstigator()->GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetOwner()->GetInstigator()->GetFName().ToString(), CurrentPlayerHealth);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
	//Functions that occur on all machines. 
	/*  
	Any special functionality that should occur as a result of damage or death should be placed here. 
	*/
}

void UInventoryComponent::PlayerTakeDamage(const float DamageAmount)
{
	float DamageTaken = FMath::Max(0.0f, CurrentPlayerHealth - DamageAmount);		// calculate new health, clamp at zero
	SetCurrentPlayerHealth(DamageTaken);	// set our new health (will only run when on the server)
	HandleRegenerationTimerReset(); //Reset our health regeneration timers
}

void UInventoryComponent::SetCurrentPlayerHealth(const float NewHealth)
{
	if ( GetWorld()->IsServer() )	// if were running on the server
	{
		CurrentPlayerHealth = NewHealth;	// update health on server. Since health replicated the client will receive
											// this update and also call OnHealthUpdate()
		OnHealthUpdate();		// anything we want to do when health is updated
	}
}

void UInventoryComponent::HandleRegeneration(float DeltaTime)
{
	//Only doing the logic if we are not max health.
	if(!(this->CurrentPlayerHealth == this->MaxPlayerHealth))
	{
		//Countdown
		this->HealthRegenerationTimer = FMath::Max(0.0f, this->HealthRegenerationTimer - DeltaTime); //Clamp at zero

		

		if(this->HealthRegenerationTimer <= 0.0f)
		{
			//Countdown to regeneration scale
			this->HealthRegenerationScaleTimer = FMath::Max(0.0f,this->HealthRegenerationScaleTimer - DeltaTime); //Clamp at zero

			//Check to see if our scale timer has count down. Only when we are not at max HP
			if(this->GetCurrentPlayerHealth() <= this->MaxPlayerHealth)
			{
				//Only in our interval time.
				if(this->HealthIncreaseInterval <= 0.0)
				{
					if(this->HealthRegenerationScaleTimer > 0.0f)
					{
						//Use normal regeneration rate
						this->SetCurrentPlayerHealth(this->GetCurrentPlayerHealth()+this->HealthRegenerationRate);
					}
					else
					{
						//Use secondary regeneration rat
						this->SetCurrentPlayerHealth(this->GetCurrentPlayerHealth()+this->HealthRegenerationRateSecondary);
					}
					
					this->HealthIncreaseInterval = this->MaxHealthIncreaseInterval;
				}
				else
					this->HealthIncreaseInterval -= DeltaTime;
			}
			else
			{
				this->SetCurrentPlayerHealth(this->MaxPlayerHealth); //Making sure we never allow the player to be more than the max
				
			}
		}
	}
	
		
	
}

void UInventoryComponent::HandleRegenerationTimerReset()
{
	//Both reset inline calls
	this->ResetHealthRegenerationTimer();
	this->ResetHealthRegenerationSecondaryTimer();
	
}

void UInventoryComponent::PickupItem(AItemParent* ItemToPickup)
{
	if (Cast<AGeneratorPoints>(ItemToPickup))	// if the item being picked up is a generator point
	{
		AddGeneratorPoint();
	} else if (Cast<AModParent>(ItemToPickup))	// if the item being picked up is a mod 
	{
		// set our picking up item to have the proper instigator so it doesnt collide with player firing the mod
		ItemToPickup->SetInstigator(GetOwner()->GetInstigator());
		// Try to pickup the mod now that we now it is a mod
		PickupMod(Cast<AModParent>(ItemToPickup));
	} else if (Cast<AHealthDrop>(ItemToPickup))	// if player picked up a health mod
	{
		AHealthDrop* HealthDrop = Cast<AHealthDrop>(ItemToPickup);
		if (HealthDrop->GetIsFullHeal())
			SetCurrentPlayerHealth(MaxPlayerHealth);
		else
		{
			const float PossibleNew = CurrentPlayerHealth+HealthDrop->GetAmountToHeal();
			const float NewHealth = (PossibleNew > MaxPlayerHealth) ? MaxPlayerHealth : PossibleNew;
			SetCurrentPlayerHealth(NewHealth);
		}
		HealthDrop->Destroy();
	}
}

void UInventoryComponent::StartModFire(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	FireActiveMod(CameraComponent, MuzzleLocation);	// guaranteed this function runs on the server because of above code
}

void UInventoryComponent::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	if (!StoredMuzzleLocation)
		StoredMuzzleLocation = MuzzleLocation;
	//switch statement to handle which mod slot we actually have active right now
	switch(ActiveModSlot)
	{
		case EModSlots::Ve_DEFAULT:	// if default mod is active then fire default mod
			{
				// if mod is valid then fire it
				if (DefaultMod)
				{
					DefaultMod->FireActiveMod(CameraComponent,MuzzleLocation);
				}
				break;
			}
		case EModSlots::Ve_SLOT1:	// if slot 1 mod is active then fire default mod
			{
				// if mod is valid then fire it
				if (ModSlot1)
				{
					ModSlot1->FireActiveMod(CameraComponent,MuzzleLocation);
				}
				break;
			}
		case EModSlots::Ve_SLOT2:	// if slot 2 mod is active then fire default mod
			{
				// if mod is valid then fire it
				if (ModSlot2)
				{
					ModSlot2->FireActiveMod(CameraComponent,MuzzleLocation);
				}
				break;
			}
		default:
			{
				break;
			}
	}
}

void UInventoryComponent::ActiveModReleased(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	//switch statement to handle which mod slot we actually have active right now
	switch(ActiveModSlot)
	{
		case EModSlots::Ve_DEFAULT:
			{
				if(DefaultMod)
				{
					DefaultMod->ActiveModRelease(CameraComponent,MuzzleLocation);
				}
				break;
			}
		case EModSlots::Ve_SLOT1:
			{
				if(ModSlot1)
				{
					ModSlot1->ActiveModRelease(CameraComponent,MuzzleLocation);
				}
				break;
			}
		case EModSlots::Ve_SLOT2:
			{
				if(ModSlot2)
				{
					ModSlot2->ActiveModRelease(CameraComponent,MuzzleLocation);
				}
				break;
			}
		default:
			{
				break;
			}
	}
	// update active mod after fire is released
	UpdateActiveMod();
}

void UInventoryComponent::SwapWeapons(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	ActiveModReleased(CameraComponent, MuzzleLocation);//artificial mod release when they swap
	ServerSwitchMod();// do the swap on the server

	
}

void UInventoryComponent::ServerSwitchMod_Implementation()
{
	// before we do anything make sure the active mod slot is up and inventory is up to date
	UpdateActiveMod();

	//switch statement to handle which mod slot we actually have active right now
	switch(ActiveModSlot)
	{
	case EModSlots::Ve_DEFAULT:
		{
			// if we are have the default mod active then we cannot switch to anything so we return
			return;
		}
	case EModSlots::Ve_SLOT1:
		{
			if (ModSlot2)	// if we have a valid mod in slot 2 swap to it
				{
				SetActiveModSlot(EModSlots::Ve_SLOT2);
				SetMaxAmmoCount(EModSlots::Ve_SLOT2); //Additionally set what the max would be of the ammo type
				}
			break;
		}
	case EModSlots::Ve_SLOT2:
		{
			if (ModSlot1)	// if we have a valid mod in slot 1 swap to it
				{
				SetActiveModSlot(EModSlots::Ve_SLOT1);
				SetMaxAmmoCount(EModSlots::Ve_SLOT1); //Additionally set what the max would be of the ammo type
				}
			break;
		}
	default:
		{
			break;
		}
	}
}

void UInventoryComponent::SetMaxAmmoCount(EModSlots ModSlot)
{
	switch(ModSlot)
	{
	case(EModSlots::Ve_DEFAULT):
		this->MaxActiveModAmmoCount = 9999.9f;
		break;
	case(EModSlots::Ve_SLOT1):
		this->MaxActiveModAmmoCount = ModSlot1->GetMaxAmmoCount();
		break;
	case(EModSlots::Ve_SLOT2):
		this->MaxActiveModAmmoCount = ModSlot2->GetMaxAmmoCount();
		break;
		
	}
}

void UInventoryComponent::ResetMods()
{
	if (ModSlot1)	// if something is in mod slot 1 destroy it
	{
		ModSlot1->Destroy();
		ModSlot1 = nullptr;
	}
	if (ModSlot2)	// if something is in mod slot 2 destroy it
	{
		ModSlot2->Destroy();
		ModSlot2 = nullptr;
	}
	// need to make sure we set the active mod to default in case it wasn't
	SetActiveModSlot(EModSlots::Ve_DEFAULT);
	SetMaxAmmoCount(EModSlots::Ve_DEFAULT);
}

FString UInventoryComponent::GetActiveModName() const
{

	//switch statement to handle which mod slot we actually have active right now
	switch(ActiveModSlot)
	{
	case EModSlots::Ve_DEFAULT:
		{
			if (DefaultMod)
				return DefaultMod->GetModTypeName();
			break;
		}
	case EModSlots::Ve_SLOT1:
		{
			if (ModSlot1)
				return ModSlot1->GetModTypeName();
			break;
		}
	case EModSlots::Ve_SLOT2:
		{
			if (ModSlot2)
				return ModSlot2->GetModTypeName();
			break;
		}
	default:
		{
			return FString("None...Oops");
		}
	}
	return FString("None...Oops");
}

FString UInventoryComponent::GetSecondaryModName() const
{
	//switch statement to handle which mod slot we actually have active right now
	switch(ActiveModSlot)
	{
	case EModSlots::Ve_DEFAULT:
		{
			if (DefaultMod)
				return FString("Empty");	// we have no secondary mod
			break;
		}
	case EModSlots::Ve_SLOT1:
		{
			if (ModSlot2)
				return ModSlot2->GetModTypeName();
			return FString("Empty");		// we have no secondary mod
			break;
		}
	case EModSlots::Ve_SLOT2:
		{
			if (ModSlot1)
				return ModSlot1->GetModTypeName();
			return FString("Empty");		// we have no secondary mod
			break;
		}
	default:
		{
			return FString("None...Oops");
		}
	}
	return FString("None...Oops");
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	// must spawn in our default mod so that it becomes usable
	if (GetOwner()->GetInstigator()->HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetOwner()->GetInstigator();
		SpawnParams.Owner = GetOwner()->GetInstigator()->GetController();
		AModParent* ModParent = GetWorld()->SpawnActor<AModParent>(DefaultModClass,GetOwner()->GetActorLocation(), FRotator(0,0,0), SpawnParams);
		if(ModParent)
		{	// we want the default mod to stay forever so set never destroy to true and destroy the mesh component
			DefaultMod=ModParent;
			DefaultMod->bNeverDestroy = true;
			DefaultMod->bIsDefaultMod = true;
			DefaultMod->MeshComponent->DestroyComponent();
			DefaultMod->OwningPlayer = Cast<ANetworkChar>(GetOwner());
		}
	}

	this->SetUpHealthRegen();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick Tick, FActorComponentTickFunction* ThisTickFunction)
{
	if (!GetOwner()->GetInstigator()->HasAuthority())
		return;	// if we arent running on server then we dont want to continue so updates
				// are only made on the server

	this->HandleRegeneration(DeltaTime); //Health Regeneration
	
	if(ModSlot1)
	{
		if(ModSlot1->bReadyToDestroy)
		{
			ModSlot1->Destroy();
			ModSlot1=nullptr;
			UpdateActiveMod();
		}
	}
	if(ModSlot2)
	{
		if(ModSlot2->bReadyToDestroy)
		{
			ModSlot2->Destroy();
			ModSlot2=nullptr;
			UpdateActiveMod();
		}
	}
	// update our active mod ammo count
	switch(ActiveModSlot)
	{
	case EModSlots::Ve_DEFAULT:
		{
			if (DefaultMod)
				ActiveModAmmoCount = 9999.9f;
			SecondaryModAmmoCount = 0.0f;	// no secondary
			break;
		}
	case EModSlots::Ve_SLOT1:
		{
			if (ModSlot1)	// active mod
			{
				ActiveModAmmoCount = ModSlot1->GetCurrentAmmoCount();
			}
			if (ModSlot2)	// secondary mod
				SecondaryModAmmoCount = ModSlot2->GetCurrentAmmoCount();
			else		// no secondary mod
				SecondaryModAmmoCount = 0.0f;

			break;
		}
	case EModSlots::Ve_SLOT2:
		{
			if (ModSlot2)	// this is our active mod
				ActiveModAmmoCount = ModSlot2->GetCurrentAmmoCount();
			if (ModSlot1)	// this would be our secondary mod
				SecondaryModAmmoCount = ModSlot1->GetCurrentAmmoCount();
			else		// we dont have a secondary mod
				SecondaryModAmmoCount = 0.0f;

			break;
		}
	default:
		{
			break;
		}
	}
}

void UInventoryComponent::UpdateActiveMod()
{
	//switch statement to handle which mod slot we actually have active right now
	switch(ActiveModSlot)
	{
	case EModSlots::Ve_DEFAULT:
		{
			if (ModSlot1)
			{
				SetActiveModSlot(EModSlots::Ve_SLOT1);
				SetMaxAmmoCount(EModSlots::Ve_SLOT1);
				
			} else if (ModSlot2)
			{
				SetActiveModSlot(EModSlots::Ve_SLOT2);
				SetMaxAmmoCount(EModSlots::Ve_SLOT2);
			}
			
			break;
		}
	case EModSlots::Ve_SLOT1:
		{
			if (!ModSlot1)	// active mod is empty
			{
				if (!ModSlot2)	// other mod is empty therefore both mods are empty
				{
					SetActiveModSlot(EModSlots::Ve_DEFAULT);	// se we set our active to default
					SetMaxAmmoCount(EModSlots::Ve_DEFAULT);
				} else
				{
					SetActiveModSlot(EModSlots::Ve_SLOT2);	// since slot one is empty and slot 2 is not set active mod to slot 2
					SetMaxAmmoCount(EModSlots::Ve_SLOT2);
				}
			}
			break;
		}
	case EModSlots::Ve_SLOT2:
		{
			if (!ModSlot2)	// active mod is empty
			{
				if (!ModSlot1)	// other mod is empty there for both mods are empty
				{
					SetActiveModSlot(EModSlots::Ve_DEFAULT);	// se we set our active to default
					SetMaxAmmoCount(EModSlots::Ve_DEFAULT);
				} else
				{
					SetActiveModSlot(EModSlots::Ve_SLOT1);	// since active mod is empty and slot 1 is not set active mod to slot 1
					SetMaxAmmoCount(EModSlots::Ve_SLOT1);
				}
			}
			break;
		}
	default:
		{
			break;
		}
	}
	
}

void UInventoryComponent::PickupMod(AModParent* NewMod)
{
	// NOTE: all mods are set to never destroy so that the dont get destroyed on their timer and then get destroyed
	//		 while the player is using them
	
	// destroy the mesh component before the authority check so that the mesh is destroyed on the server and client
	if (NewMod && NewMod->MeshComponent && NewMod->DropEffect) {
		NewMod->MeshComponent->DestroyComponent();	// make sure to get rid of the MeshComponent
		NewMod->DropEffect->DestroyComponent();		// get rid of the vfx component as well
	}

	if (!GetOwner()->GetInstigator()->HasAuthority())
		return;	// if we arent running on server then we dont want to continue so updates
				// are only made on the server

	// if the NewMod is currently held
	const bool ModIsHeld = this->CheckIfModContained(NewMod);
	if (ModIsHeld && bDestroyDuplicateModOnPickup)	
	{
		NewMod->Destroy();	// destroy the mod
		return;
	}
	if (ModIsHeld && !bDestroyDuplicateModOnPickup)	// add ammo to the held mod
	{
		this->AddAmmoToHeldModOfName(NewMod->GetModTypeName());	// add our ammo
		NewMod->Destroy();			// destroy the mod
		return;
	}
	
	//switch statement to handle which mod slot we actually have active right now
	switch(ActiveModSlot)
	{
		case EModSlots::Ve_DEFAULT:
			{
				if (!ModSlot1)	// if mod slot 1 is empty then add new mod to that slot
				{
					ModSlot1 = NewMod;
					ModSlot1->bNeverDestroy = true;
					ModSlot1->SetInstigator(GetOwner()->GetInstigator());
					ModSlot1->OwningPlayer = Cast<ANetworkChar>(GetOwner());
					ModSlot1->OnPickup(StoredMuzzleLocation);
					
					//Play sound
					if(SuccessfulWeaponPickupSound)
					{
						Cast<ANetworkChar>(GetOwner())->SpawnWeaponSound(nullptr, SuccessfulWeaponPickupSound,3);
					}
					
				} else if (!ModSlot2)	// if mod slot 2 is empty then add new mod to that slot
				{
					ModSlot2 = NewMod;
					ModSlot2->bNeverDestroy = true;
					ModSlot2->SetInstigator(GetOwner()->GetInstigator());
					ModSlot2->OwningPlayer = Cast<ANetworkChar>(GetOwner());
					ModSlot2->OnPickup(StoredMuzzleLocation);
					
					//NewMod->MeshComponent->DestroyComponent();	// make sure to get rid of the MeshComponent
					//Play sound
					if(SuccessfulWeaponPickupSound)
					{
						Cast<ANetworkChar>(GetOwner())->SpawnWeaponSound(nullptr, SuccessfulWeaponPickupSound,3);
					}
				}
				break;
			}
		case EModSlots::Ve_SLOT1:
			{
				if (!ModSlot2)	// if mod slot 2 is empty then add new mod to that slot
				{
					ModSlot2 = NewMod;
					ModSlot2->bNeverDestroy = true;
					ModSlot2->SetInstigator(GetOwner()->GetInstigator());
					ModSlot2->OwningPlayer = Cast<ANetworkChar>(GetOwner());
					ModSlot2->OnPickup(StoredMuzzleLocation);
					//Play sound
					if(SuccessfulWeaponPickupSound)
					{
						Cast<ANetworkChar>(GetOwner())->SpawnWeaponSound(nullptr, SuccessfulWeaponPickupSound,3);
					}
				}
				else
				{
					if(FailedWeaponPickupSound)
					{
						Cast<ANetworkChar>(GetOwner())->SpawnWeaponSound(nullptr, FailedWeaponPickupSound,3);
					}
				}
				break;
			}
		case EModSlots::Ve_SLOT2:
			{
				if (!ModSlot1)	// if mod slot 1 is empty then add new mod to that slot
				{
					ModSlot1 = NewMod;
					ModSlot1->bNeverDestroy = true;
					ModSlot1->SetInstigator(GetOwner()->GetInstigator());
					ModSlot1->OwningPlayer = Cast<ANetworkChar>(GetOwner());
					ModSlot1->OnPickup(StoredMuzzleLocation);
					//Play sound
					if(SuccessfulWeaponPickupSound)
					{
						Cast<ANetworkChar>(GetOwner())->SpawnWeaponSound(nullptr, SuccessfulWeaponPickupSound,3);
					}
				}
				else
				{
					if(FailedWeaponPickupSound)
					{
						Cast<ANetworkChar>(GetOwner())->SpawnWeaponSound(nullptr, FailedWeaponPickupSound,3);
					}
				}
				break;
			}
		default:
			{
				break;
			}
	}
	// after we do our pick up logic call our update active mod in case we came into this with default mod active
	// now we want to update our active mod or had to change mods at all
	UpdateActiveMod();
}

bool UInventoryComponent::CheckIfModContained(AModParent* NewMod)
{
	if (ModSlot1)
	{
		if (ModSlot1->GetModTypeName() == NewMod->GetModTypeName())
		{
			return true;
		}
	}

	if (ModSlot2)
	{
		if (ModSlot2->GetModTypeName() == NewMod->GetModTypeName())
		{
			return true;
		}
	}
	return false;
}

void UInventoryComponent::AddAmmoToHeldModOfName(const FString ModTypeName)
{
	if (ModSlot1)
	{
		if (ModSlot1->GetModTypeName() == ModTypeName)
		{
			ModSlot1->ResetAmmo();
		}
	}

	if (ModSlot2)
	{
		if (ModSlot2->GetModTypeName() == ModTypeName)
		{
			ModSlot2->ResetAmmo();
		}
	}	
}

void UInventoryComponent::OnRep_ControllerUpdated()
{
	if (DefaultMod)
	{
		DefaultMod->SetOwner(OwningController);
	}
}



void UInventoryComponent::OnRep_DefaultModUpdate()
{
	if (DefaultMod && OwningController)
	{
		DefaultMod->SetOwner(OwningController);
	}
}

void UInventoryComponent::OnRep_Mod1Update()
{
	if (ModSlot1 && OwningController)
	{
		ModSlot1->SetOwner(OwningController);
	}
}

void UInventoryComponent::OnRep_Mod2Update()
{
	if (ModSlot2 && OwningController)
	{
		ModSlot2->SetOwner(OwningController);
	}
}

void UInventoryComponent::SetUpHealthRegen()
{
	//Set up max timers.
	this->MaxHealthIncreaseInterval = this->HealthIncreaseInterval;
	this->MaxHealthRegenerationTimer = this->HealthRegenerationTimer;
	this->MaxHealthRegenerationScaleTimer = this->HealthRegenerationScaleTimer;
}

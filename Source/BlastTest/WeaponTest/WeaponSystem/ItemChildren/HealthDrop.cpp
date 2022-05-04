// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/ItemChildren/HealthDrop.h"

AHealthDrop::AHealthDrop()
{
	if(!MeshComponent)
	{	
		MeshComponent= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
		MeshComponent->bHiddenInGame=true;
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RootComponent = MeshComponent;		// TODO: check if this fixes the no root component being set in the mods
	}
	
	if(!DropEffect)	// create the sub object for the vfx and set the system in the editor
	{
		DropEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Drop Effect"));
		DropEffect->Activate();
		
		DropEffect->SetupAttachment(MeshComponent);
	}
}

void AHealthDrop::BeginPlay()
{
	Super::BeginPlay();

	bNeverDestroy = false;
}

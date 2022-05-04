// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastInteratableMessage.h"
#include <string>

#include "NetworkChar.h"

void UBlastInteratableMessage::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBlastInteratableMessage::Update(int PlayerID)
{
	if(PlayerID >= 0)
	{
		if(IsValid(UGameplayStatics::GetPlayerCharacter(GetWorld(), PlayerID)))
		{
			Player = Cast<ANetworkChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), PlayerID));
			Generator = Cast<AGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AGenerator::StaticClass()));
			if(Player->HasAuthority())
			{
				if(Generator)
				{
					if(Player->FoundGenerator)
					{
						if(Generator->IsInPlay)
						{
							//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, "isinplay");
							InteractMessage->SetVisibility(ESlateVisibility::Visible);
							InteractMessage->SetText(FText::FromString("Press 'E' to Interact"));
						}
					}
					else
					{
						InteractMessage->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
			else
			{
				if(Generator)
				{
					if(Player->FoundGenerator)
					{
						if(Generator->IsInPlay)
						{
							//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, "isinplay");
							InteractMessage->SetVisibility(ESlateVisibility::Visible);
							InteractMessage->SetText(FText::FromString("Press 'E' to Interact"));
						}
					}
					else
					{
						InteractMessage->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		}
	}
}

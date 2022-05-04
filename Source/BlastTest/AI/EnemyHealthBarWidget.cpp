// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyHealthBarWidget.h"

void UEnemyHealthBarWidget::NativeConstruct()
{
}



void UEnemyHealthBarWidget::SetBarValuePercent(float Value) 
{
	HealthValue->SetPercent(Value);
}



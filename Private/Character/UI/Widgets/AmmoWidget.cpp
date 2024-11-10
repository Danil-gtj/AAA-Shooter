// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/Widgets/AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo)
{
	Ammo = NewAmmo;
	TotalAmmo = NewTotalAmmo;
}

void UAmmoWidget::UpdateFragGranadesCount(int32 FragGranades)
{
	FragGranadesCount = FragGranades;
}

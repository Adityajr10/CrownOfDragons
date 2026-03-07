// Fill out your copyright notice in the Description page of Project Settings.


#include "DragonPerchPoint.h"
ADragonPerchPoint::ADragonPerchPoint()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

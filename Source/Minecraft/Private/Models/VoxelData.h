// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelData.generated.h"

USTRUCT()
struct FVoxelData
{
	GENERATED_BODY()
	
public:
	
	FVoxelData() {}
	
	FVoxelData(UPrimitiveComponent* Component, AActor* Actor, FVector Location, FRotator Rotation, bool CanSpawn, bool IsSomething) :
		Component(Component),
		Actor(Actor),
		Location(Location),
		Rotation(Rotation),
		CanSpawn(CanSpawn),
		IsSomething(IsSomething)
	{}
	UPROPERTY()
		UPrimitiveComponent* Component = nullptr;
	UPROPERTY()
		AActor* Actor = nullptr;
	FVector Location;
	FRotator Rotation;
	bool CanSpawn;
	bool IsSomething;
};

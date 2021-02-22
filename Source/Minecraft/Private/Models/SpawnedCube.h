// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UserCube.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnedCube.generated.h"

USTRUCT()
struct FSpawnedCube
{
	GENERATED_BODY()

public:

	FSpawnedCube() {}

	FSpawnedCube(AUserCube* UserCube, FVector Location, bool IsSpawned, FRotator Rotation, UCubeMaterial::Type Type) :
		UserCube(UserCube),
		Location(Location),
		IsSpawned(IsSpawned),
		SelectedMaterialIndex(int(Type)),
		Rotation(Rotation),
		Type(Type)
	{}
	UPROPERTY()
		AUserCube* UserCube = nullptr;
	UPROPERTY()
		FVector Location;
	UPROPERTY()
		bool IsSpawned = false;
	UPROPERTY()
		int SelectedMaterialIndex = 0;
	UPROPERTY()
		FRotator Rotation;
	UCubeMaterial::Type Type = UCubeMaterial::Type::Basic;

	bool operator ==(const FVector& rhs) const {
		return Location == rhs;
	}
};

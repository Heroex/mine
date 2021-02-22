// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Models/SpawnedCube.h"
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UMySaveGame();
	
	// Properties
	UPROPERTY()
		FVector PlayerLocation = FVector(0.f, 0.f, 0.f);
	UPROPERTY()
		TArray<FSpawnedCube> SpawnedUserCubes;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Models/VoxelData.h"
#include "Components/BoxComponent.h"
#include "Setup/Configuration.h"
#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Actor.h"
#include "Cube.generated.h"

UCLASS()
class ACube : public AActor
{
	GENERATED_BODY()

public:

	ACube();

protected:

	virtual void BeginPlay() override;

private:

	// Properties
	UPROPERTY()
		UInstancedStaticMeshComponent* CubeInst = nullptr;
	UPROPERTY()
		USceneComponent* Root = nullptr;
	UPROPERTY()
		UStaticMesh* SM = nullptr;
	TArray<UInstancedStaticMeshComponent*> Instances;
	TArray<UMaterialInstanceDynamic*> MaterialsInGame;

	// Enum definition
	enum MaterialType { Grass = 0, Rock = 1, Snow = 2 };

	// Methods
	void SetupMaterials();
	void Initialize();
	void DestroyCube();
	void RemoveInstance(UInstancedStaticMeshComponent* SelectedVoxel);
	void Generate();
	void SpawnVoxelAndApplyNoise(FVector CalculatedCoords);
};

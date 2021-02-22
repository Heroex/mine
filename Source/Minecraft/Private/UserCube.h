// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Models/CubeMaterialType.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UserCube.generated.h"


UCLASS()
class AUserCube : public AActor
{
	GENERATED_BODY()

public:

	/* Constructor */
	AUserCube();

	/* Properties */
	UPROPERTY()
		UStaticMeshComponent* StaticMeshComponent = nullptr;

	/* Methods */
	void DestroyCube();
	void SpawnCube(UCubeMaterial::Type Type);
	void ChangeMaterial(UCubeMaterial::Type Type);

protected:

	virtual void BeginPlay() override;

private:

	/* Properties */
	UPROPERTY()
		USceneComponent* Root = nullptr;
	UPROPERTY()
		UStaticMesh* StaticMesh = nullptr;
	UPROPERTY()
		UMaterialInstanceDynamic* BaseMaterialMID = nullptr;

	TMap<UCubeMaterial::Type, UMaterialInstanceDynamic*> CubeMaterials;
};

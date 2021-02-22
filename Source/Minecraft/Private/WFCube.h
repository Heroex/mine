// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCube.generated.h"

UCLASS()
class AWFCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWFCube();

	USceneComponent* Root = nullptr;
	UStaticMesh* StaticMesh;
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	UMaterialInstanceDynamic* BaseMaterialMID;
	void SetWorldLocationAt(FVector Location, FRotator Rotation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

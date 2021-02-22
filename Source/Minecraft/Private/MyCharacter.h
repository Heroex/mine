// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <tuple>
#include "MySaveGame.h"
#include "Models/ChunkData.h"
#include "Models/CubeMaterialType.h"
#include "Kismet/GameplayStatics.h"
#include "Models/SpawnedCube.h"
#include "UserCube.h"
#include "Math/Rotator.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/WidgetTree.h"
#include "UserHUD.h"
#include "Setup/Configuration.h"
#include "CoreMinimal.h"
#include "Cube.h"
#include "Math/Vector.h"
#include "Components/InputComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

private:

	/* Properties */
	TArray<FChunkData> SpawnedChunksWithLocation;
	TArray<FVector> SpawnedChunksCoords;
	FVector2D LastActorPosition;
	UCubeMaterial::Type SelectedMaterialForVoxel = UCubeMaterial::Type::Basic;
	TMap<UCubeMaterial::Type, UMaterial*> CubeMaterials;
	FVector LastPositionForBuildingSpawn;
	TArray<FSpawnedCube> SpawnedUserCubes;
	TArray<ACube*> CurrentCubes;
	FVector2D PlayerCubeCoords;
	TArray<FVector> GeneratedCubeCoords;
	FVector ActorLocation;
	UPROPERTY()
		UCameraComponent* Camera = nullptr;
	UPROPERTY()
		UUserHUD* UserHUD = nullptr;
	UPROPERTY()
		AUserCube* CubeForBuilding = nullptr;
	UPROPERTY()
		UStaticMesh* StaticMesh = nullptr;
	UPROPERTY()
		UStaticMeshComponent* CubeMeshComponent = nullptr;

	bool IsJumping = false;
	bool IsDeleteButtonPressed = false;
	bool IsInBuildingMode = false;
	float StartTime;
	bool IsFirstInit = true;
	const float GRidValue = VoxelSize;
	const FString SlotName = "GameSlot";

	/* Mehods */
	std::tuple<FVector, FRotator> GetFirstPlayerControllerPosition() const;
	FHitResult GetFirstPhysicBodyInReach() const;
	FVoxelData GetInstanceTransformFromLineTrace(bool IsRemovingCube = false);
	FVector GetLineTraceEnd() const;
	FVector GetPlayerLocation() const;
	UMaterial* GetMaterialFor(UCubeMaterial::Type Cube) {
		return CubeMaterials[Cube];
	}

	void SpawnCubeAtLocation(FVector Location, bool CanSpawnNextChunk);
	void DestroyChunksOrCubesIfNeeded();
	void SetupMaterials();
	void ShowCubeAtLocation();
	void BuildMode();
	void LoadGame();
	void SaveGame();
	void SpawnCreatedCubesIfNeeded();
	void GenerateLandscape();
	bool IsActorMoving();

	/* Actions */
	void VerticalMovement(float Value);
	void HorizontalMovement(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void CheckJump();
	void JumpInputAction();
	void SetupInputMapping();
	void BindInputs();
	void SetupHUD();
	void AddVoxel();
	void RemoveVoxel();
	void AddWhiteCube();
	void AddBlueCube();
	void AddYellowCube();
	void AddRedCube();
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "Cube.h"

/****
// Life-cycle
#################################################################################################################*/
ACube::ACube()
{
	Initialize();
}

void ACube::Initialize()
{
	SetupMaterials();

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	check(Root != nullptr);
	RootComponent = Root;
	CubeInst = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cube"));
	for (int i = 0; i < MaterialsInGame.Num(); i++)
	{
		UInstancedStaticMeshComponent* NewInstance = NewObject<UInstancedStaticMeshComponent>(CubeInst);
		NewInstance->AttachTo(RootComponent);
		NewInstance->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		SM = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube")));
		NewInstance->SetStaticMesh(SM);
		NewInstance->SetMaterial(0, MaterialsInGame[i]);
		Instances.Add(NewInstance);
	}
}

void ACube::BeginPlay()
{
	Super::BeginPlay();
	Generate();
}

/****
// Initialize methods
#################################################################################################################*/
void ACube::SetupMaterials()
{
	// Set Grass material
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GrassMaterialInstance(TEXT("MaterialInstanceConstant'/Game/Minecraft/Materials/M_Grass.M_Grass'"));
	if (GrassMaterialInstance.Succeeded())
	{
		UMaterial* Material = (UMaterial*)GrassMaterialInstance.Object;
		MaterialsInGame.Add(UMaterialInstanceDynamic::Create(Material, nullptr));
	}

	// Set Rock material
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> RockMaterialInstance(TEXT("MaterialInstanceConstant'/Game/Minecraft/Materials/M_Powder.M_Powder'"));
	if (RockMaterialInstance.Succeeded())
	{
		UMaterial* Material = (UMaterial*)RockMaterialInstance.Object;
		MaterialsInGame.Add(UMaterialInstanceDynamic::Create(Material, nullptr));
	}

	// Set Snow material
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> SnowMaterialInstance(TEXT("MaterialInstanceConstant'/Game/Minecraft/Materials/M_Snow.M_Snow'"));
	if (SnowMaterialInstance.Succeeded())
	{
		UMaterial* Material = (UMaterial*)SnowMaterialInstance.Object;
		MaterialsInGame.Add(UMaterialInstanceDynamic::Create(Material, nullptr));
	}
}

void ACube::Generate()
{
	int XYStartIndex = -1 * IMCubeRenderDistance;
	int XYEndIndex = IMCubeRenderDistance;

	for (int x = XYStartIndex; x < XYEndIndex; x++)
	{
		for (int y = XYStartIndex; y < XYEndIndex; y++)
		{
			FVector SpawnVector(x, y, 0);
			SpawnVoxelAndApplyNoise(SpawnVector);
		}
	}
}

/****
// Methods
#################################################################################################################*/
void ACube::DestroyCube()
{
	Destroy();
}

void ACube::RemoveInstance(UInstancedStaticMeshComponent* SelectedVoxel)
{
	SelectedVoxel->RemoveInstance(0);
}

void ACube::SpawnVoxelAndApplyNoise(FVector CalculatedCoords)
{
	int CalculatedX = VoxelSize * CalculatedCoords.X;
	int CalculatedZ = VoxelSize * CalculatedCoords.Z;
	int CalculatedY = VoxelSize * CalculatedCoords.Y;
	FVector CubeLocation = RootComponent->GetOwner()->GetActorLocation();
	FVector Location = FVector(CubeLocation.X, CubeLocation.Y, CubeLocation.Z) / VoxelSize;
	FVector CubeWorldPosition = FVector(((FMath::FloorToInt(Location.X) + CalculatedCoords.X) * VoxelSize), ((FMath::FloorToInt(Location.Y) + CalculatedCoords.Y) * VoxelSize), ((FMath::FloorToInt(Location.Z) + CalculatedCoords.Z) * VoxelSize)) * NoiseDensity;
	const float NoiseValue_2D = FMath::PerlinNoise2D(FVector2D(CubeWorldPosition.X, CubeWorldPosition.Y)) * Amplitude;
	const int WorldZAxis = (FMath::FloorToInt(NoiseValue_2D) - CalculatedCoords.Z);
	const int Z_Axis = WorldZAxis * VoxelSize;
	FVector VoxelCoords = FVector(CalculatedX, CalculatedY, Z_Axis);
	FTransform instanceTransform = FTransform();
	instanceTransform.SetLocation(VoxelCoords);
	instanceTransform.SetScale3D(FVector(VoxelSize / 100));
	if (WorldZAxis < RockZAxis)
	{
		Instances[MaterialType::Rock]->AddInstance(instanceTransform);
	}
	else if (WorldZAxis < GrassZAxis)
	{
		Instances[MaterialType::Grass]->AddInstance(instanceTransform);
	}
	else
	{
		Instances[MaterialType::Snow]->AddInstance(instanceTransform);
	}
}

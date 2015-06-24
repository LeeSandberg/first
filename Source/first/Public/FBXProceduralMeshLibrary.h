// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KismetProceduralMeshLibrary.h"
#include "FBXProceduralMeshLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FIRST_API UFBXProceduralMeshLibrary : public UKismetProceduralMeshLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "FBX")
		static void GenerateBoxMesh(FVector BoxRadius, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents);

};

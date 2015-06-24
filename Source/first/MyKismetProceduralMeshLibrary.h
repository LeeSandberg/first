// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KismetProceduralMeshLibrary.h"
#include "MyKismetProceduralMeshLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FIRST_API UMyKismetProceduralMeshLibrary : public UKismetProceduralMeshLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "BadAssProceduralMeshLibrary")
		static FString GetHappyMessage();

	UFUNCTION(BlueprintCallable, Category = "BadAssProceduralMeshLibrary")
		static void GenerateBoxMesh(FVector BoxRadius, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents);


	/** Saves text to filename of your choosing, make sure include whichever file extension you want in the filename, ex: SelfNotes.txt . Make sure to include the entire file path in the save directory, ex: C:\MyGameDir\BPSavedTextFiles */
	//UFUNCTION(BlueprintCallable, Category = "MyKismetProceduralMeshLibrary")
		//static bool SaveStringTextToFile(FString SaveDirectory, FString FileName, FString SaveText, bool AllowOverWriting = false);
	
};

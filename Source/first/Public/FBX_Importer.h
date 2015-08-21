// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "D:/Autodesk/FBX/FBX SDK/2014.2.1/include/fbxsdk.h"

/**
*
*/
class FIRST_API FBX_Importer
{
public:
	FBX_Importer();
	~FBX_Importer();

	void LoadFBX(FString InFileName, TArray<TArray<FVector>>* pOutVertexArray, TArray<TArray<int32>>* pOutTriangleArray, TArray<TArray<FVector>>* pOutNormalArray, TArray<FVector>* pOutDiffuseArray, TArray<UTexture2D*>* pOutTextureArray, TArray<TArray<FVector2D>>* pOutUVArray, int* iOutNodeCount);

private:

	FbxManager* m_pFbxManager;

	void ReadNormal(FbxMesh* pInMesh, int iInControlPointIndex, int iInVertexCounter, FVector* vOutNormal);
	void ReadUV(FbxMesh* pInMesh, int iInControlPointIndex, int iInVertexCounter, FVector2D* pOutUV);

	FbxMatrix ComputeTotalMatrix(FbxNode* pInNode);
	UTexture2D* FBX_Importer::LoadTexture(FString TextureFilename);

};

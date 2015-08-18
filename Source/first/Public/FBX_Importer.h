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

	void LoadFBX(FString InFileName, TArray<TArray<FVector>>* pOutVertexArray, TArray<TArray<int32>>* pOutTriangleArray, TArray<TArray<FVector>>* pOutNormalArray, TArray<FVector>* pOutDiffuseArray, int* iOutNodeCount);

private:

	FbxManager* m_pFbxManager;

	void ReadNormal(FbxMesh* pInMesh, int iInControlPointIndex, int iInVertexCounter, FVector* vOutNormal);
	FbxMatrix ComputeTotalMatrix(FbxNode* pInNode);
};

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

	void LoadFBX(TArray<FVector>* pOutVertexArray, TArray<int32>* pOutTriangleArray);

private:

	FbxManager* m_pFbxManager;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "D:/Autodesk/FBX/FBXSDK/2016.0/include/fbxsdk.h"
//#include "D:/Autodesk/FBX/FBXSDK/2016.0/include/fbxsdk/fileio/fbxiosettings.h"
#include "GameFramework/Actor.h"
#include "FBX_Importer_test.generated.h"


UCLASS()
class FIRST_API AFBX_Importer_test : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFBX_Importer_test();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void LoadFBX(TArray<FVector>* pOutVertexArray);

private:

	FbxManager* m_pFbxManager;
	

};

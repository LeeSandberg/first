// Fill out your copyright notice in the Description page of Project Settings.

#include "first.h"
#include "FBX_Importer_test.h"


// Sets default values
AFBX_Importer_test::AFBX_Importer_test()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFBX_Importer_test::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFBX_Importer_test::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AFBX_Importer_test::LoadFBX(TArray<FVector>* pOutVertexArray)
{
	if (m_pFbxManager == nullptr)
	{
		m_pFbxManager = FbxManager::Create();

		FbxIOSettings* _pFbxIOSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
		m_pFbxManager->SetIOSettings(_pFbxIOSettings);
	}

	FbxImporter* _pFbxImporter = FbxImporter::Create(m_pFbxManager, "");
	FbxScene* _pFbxScene = FbxScene::Create(m_pFbxManager, "");

	bool _bSuccess = _pFbxImporter->Initialize("D:/Joel/FBX/Test Objects/test_object_1.fbx", -1, m_pFbxManager->GetIOSettings());
	if (!_bSuccess)	return;

	_bSuccess = _pFbxImporter->Import(_pFbxScene);
	if (!_bSuccess) return;

	_pFbxImporter->Destroy();

	FbxNode* _pRootNode = _pFbxScene->GetRootNode();

	if (_pRootNode)
	{
		for (int i = 0; i < _pRootNode->GetChildCount(); i++)
		{
			FbxNode* _pChildNode = _pRootNode->GetChild(i);

			if (_pChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType _AttributeType = _pChildNode->GetNodeAttribute()->GetAttributeType();
			if (_AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* _pMesh = (FbxMesh*)_pChildNode->GetNodeAttribute();

			FbxVector4* _pVertices = _pMesh->GetControlPoints();

			for (int j = 0; j < _pMesh->GetPolygonCount(); j++)
			{
				int _iNumVertices = _pMesh->GetPolygonSize(j);
				check(_iNumVertices == 3);

				for (int k = 0; k < _iNumVertices; k++)
				{
					int _iControlPointIndex = _pMesh->GetPolygonVertex(j, k);
					
					FVector _vertex;
					_vertex.X = (float)_pVertices[_iControlPointIndex].mData[0];
					_vertex.Y = (float)_pVertices[_iControlPointIndex].mData[1];
					_vertex.Z = (float)_pVertices[_iControlPointIndex].mData[2];

					pOutVertexArray->Push(_vertex);
				}
			}
		}
	}
}

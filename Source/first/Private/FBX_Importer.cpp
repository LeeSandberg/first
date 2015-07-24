// Fill out your copyright notice in the Description page of Project Settings.

#include "first.h"
#include "FBX_Importer.h"

FBX_Importer::FBX_Importer()
{
	m_pFbxManager = nullptr;
}

FBX_Importer::~FBX_Importer()
{
	if (m_pFbxManager != nullptr)
	{
		m_pFbxManager->Destroy();
	}
}

void FBX_Importer::LoadFBX(FString InFileName, TArray<FVector>* pOutVertexArray, TArray<int32>* pOutTriangleArray, TArray<FVector>* pOutNormalArray)
{
	if (m_pFbxManager == nullptr)
	{
		m_pFbxManager = FbxManager::Create();

		FbxIOSettings* _pFbxIOSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
		m_pFbxManager->SetIOSettings(_pFbxIOSettings);
	}

	FbxImporter* _pFbxImporter = FbxImporter::Create(m_pFbxManager, "Imp");
	FbxScene* _pFbxScene = FbxScene::Create(m_pFbxManager, "Scene");

	bool _bSuccess = _pFbxImporter->Initialize(TCHAR_TO_ANSI(*InFileName), -1, m_pFbxManager->GetIOSettings());
	if (!_bSuccess)	return;

	_bSuccess = _pFbxImporter->Import(_pFbxScene);
	if (!_bSuccess) return;

	_pFbxImporter->Destroy();

	FbxNode* _pRootNode = _pFbxScene->GetRootNode();

	if (_pRootNode)
	{
		TArray<FVector> _tempNormals;
		int _iVertexCounter = 0;

		for (int i = 0; i < _pRootNode->GetChildCount(); i++)
		{
			FbxNode* _pChildNode = _pRootNode->GetChild(i);

			if (_pChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType _AttributeType = _pChildNode->GetNodeAttribute()->GetAttributeType();
			if (_AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxVector4 _vNodePosition = (FbxVector4)_pChildNode->LclTranslation.Get();
			FbxDouble3 _dNodeScale = _pChildNode->LclScaling.Get();

			FbxMesh* _pMesh = (FbxMesh*)_pChildNode->GetNodeAttribute();

			FbxVector4* _pVertices = _pMesh->GetControlPoints();

			for (int j = 0; j < _pMesh->GetPolygonCount() * 3; j++)
				pOutTriangleArray->Push(j);

			for (int j = 0; j < _pMesh->GetPolygonCount(); j++)
			{

				int _iNumVertices = _pMesh->GetPolygonSize(j);
				check(_iNumVertices == 3);

				for (int k = 0; k < _iNumVertices; k++)
				{
					int _iControlPointIndex = _pMesh->GetPolygonVertex(j, k);

					FVector _vNormal;
					ReadNormal(_pMesh, _iControlPointIndex, _iVertexCounter, &_vNormal);
					_vNormal = -_vNormal;

					FbxVector4 _controlPoint = _pVertices[_iControlPointIndex];
					_controlPoint = (_controlPoint * _dNodeScale) +_vNodePosition;

					FVector _vertex;
					_vertex.X = (float)_controlPoint.mData[0];
					_vertex.Y = (float)_controlPoint.mData[1];
					_vertex.Z = (float)_controlPoint.mData[2];

					pOutVertexArray->Push(_vertex);
					_tempNormals.Push(_vNormal);
					_iVertexCounter++;

				}

			}
		}
		//TArray<FVector>::TIterator _it = _tempNormals.CreateIterator();
		for (int i = _tempNormals.Num() -1; i >= 0; i--)
		{
			pOutNormalArray->Push(_tempNormals[i]);
		}
	}
}

void FBX_Importer::ReadNormal(FbxMesh* pInMesh, int iInControlPointIndex, int iInVertexCounter, FVector* pOutNormal)
{
	if (pInMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = pInMesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
			switch (vertexNormal->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					pOutNormal->X = static_cast<float>(vertexNormal->GetDirectArray().GetAt(iInControlPointIndex).mData[0]);
					pOutNormal->Y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(iInControlPointIndex).mData[1]);
					pOutNormal->Z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(iInControlPointIndex).mData[2]);
				}
				break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexNormal->GetIndexArray().GetAt(iInControlPointIndex);
					pOutNormal->X = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
					pOutNormal->Y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
					pOutNormal->Z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
				}
				break;

				default:
					throw std::exception("Invalid Reference");
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			switch (vertexNormal->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					pOutNormal->X = static_cast<float>(vertexNormal->GetDirectArray().GetAt(iInVertexCounter).mData[0]);
					pOutNormal->Y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(iInVertexCounter).mData[1]);
					pOutNormal->Z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(iInVertexCounter).mData[2]);
				}
				break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexNormal->GetIndexArray().GetAt(iInVertexCounter);
					pOutNormal->X = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
					pOutNormal->Y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
					pOutNormal->Z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
				}
				break;

				default:
					throw std::exception("Invalid Reference");
			}
			break;
	}
}
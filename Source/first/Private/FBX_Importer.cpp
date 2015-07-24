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

void FBX_Importer::LoadFBX(FString InFileName, TArray<TArray<FVector>>* pOutVertexArray, TArray<TArray<int32>>* pOutTriangleArray, TArray<TArray<FVector>>* pOutNormalArray, int* iOutNodeCount)
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
		//TArray<FVector> _tempNormals;
		*iOutNodeCount = _pRootNode->GetChildCount();
		for (int i = 0; i < *iOutNodeCount; i++)
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
			//_pMesh->GetPolygonVertexCount();
			//pOutTriangleArray->Push(_pMesh->GetPolygonVertices());
			//for (int j = 0; j < _pMesh->GetPolygonVertexCount(); j++)
			//	pOutTriangleArray->Push(_pMesh->GetPolygonVertex[j]);

			TArray<FVector> _tempVertexArray;
			TArray<FVector> _tempNormalArray;
			TArray<int> _tempIndexArray;
			int _iVertexCounter = 0;
			FbxMatrix _mTotalMatrix = ComputeTotalMatrix(_pChildNode);
			//FbxVector _vNodeUp = _pChildNode->UpVectorProperty.Get();
			//for (int j = 0; j < _pMesh->GetPolygonVertexCount(); j++)
			//	_tempIndexArray.Push(j);

			for (int j = 0; j < _pMesh->GetPolygonCount(); j++)
			{

				int _iNumVertices = _pMesh->GetPolygonSize(j);
				check(_iNumVertices == 3);

				int _iVertexIndex = _pMesh->GetPolygonVertexIndex(j);

				int _pVertexIndexArray = _pMesh->GetPolygonVertices()[_iVertexIndex];

				for (int k = 0; k < _iNumVertices; k++)
				{
					int _iControlPointIndex = _pMesh->GetPolygonVertex(j, k);

					FVector _vNormal;
					ReadNormal(_pMesh, _iControlPointIndex, _iVertexCounter, &_vNormal);
					//_vNormal = -_vNormal;


					FbxVector4 _controlPoint = _pVertices[_iControlPointIndex];
					//FbxVector4 _foo(10.0, 10.0, 10.0);
					_controlPoint = _controlPoint + (_vNodePosition*0.0001);
					//_controlPoint = _mTotalMatrix.MultNormalize(_controlPoint);

					FVector _vertex;
					_vertex.X = (float)_controlPoint.mData[0];
					_vertex.Y = (float)_controlPoint.mData[1];
					_vertex.Z = (float)_controlPoint.mData[2];

					_tempVertexArray.Push(_vertex);
					_tempNormalArray.Push(_vNormal);
					_tempIndexArray.Push(_iVertexCounter);
					_iVertexCounter++;

				}

			}
			pOutVertexArray->Push(_tempVertexArray);
			pOutNormalArray->Push(_tempNormalArray);
			pOutTriangleArray->Push(_tempIndexArray);
		}
		//TArray<FVector>::TIterator _it = _tempNormals.CreateIterator();
		//for (int i = _tempNormals.Num() -1; i >= 0; i--)
		//{
		//	pOutNormalArray->Push(_tempNormals[i]);
		//}
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

FbxMatrix FBX_Importer::ComputeTotalMatrix(FbxNode* pInNode)
{
	FbxMatrix _mGeometry;
	FbxVector4 _vTranslation, _vRotation, _vScaling;
	_vTranslation = pInNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	_vRotation = pInNode->GetGeometricRotation(FbxNode::eSourcePivot);
	_vScaling = pInNode->GetGeometricScaling(FbxNode::eSourcePivot);
	_mGeometry.SetTRS(_vTranslation, _vRotation, _vScaling);

	FbxMatrix _mGlobalTransform = pInNode->EvaluateGlobalTransform();//FbxScene->GetEvaluator()->GetNodeGlobalTransform(Node);

	FbxMatrix _mTotalMatrix;
	_mTotalMatrix = _mGlobalTransform * _mGeometry;

	return _mTotalMatrix;
}
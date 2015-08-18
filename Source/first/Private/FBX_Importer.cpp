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

void FBX_Importer::LoadFBX(FString InFileName, TArray<TArray<FVector>>* pOutVertexArray, TArray<TArray<int32>>* pOutTriangleArray, TArray<TArray<FVector>>* pOutNormalArray, TArray<FVector>* pOutDiffuseArray, int* iOutNodeCount)
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
		*iOutNodeCount = _pRootNode->GetChildCount();
		for (int i = 0; i < *iOutNodeCount; i++)
		{
			FbxNode* _pChildNode = _pRootNode->GetChild(i);

			if (_pChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType _AttributeType = _pChildNode->GetNodeAttribute()->GetAttributeType();
			if (_AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxDouble3 _vNodePosition = _pChildNode->LclTranslation.Get();
			FbxDouble3 _dNodeScale = _pChildNode->LclScaling.Get();

			FbxMesh* _pMesh = (FbxMesh*)_pChildNode->GetNodeAttribute();

			FbxVector4* _pVertices = _pMesh->GetControlPoints();

			TArray<FVector> _tempVertexArray;
			TArray<FVector> _tempNormalArray;
			TArray<int> _tempIndexArray;
			int _iVertexCounter = 0;
			FbxMatrix _mNodeTransform = _pChildNode->EvaluateLocalTransform();


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
					//_controlPoint = (_controlPoint /* _dNodeScale /** 0.0001f*/) + (_vNodePosition /* 0.0001f*/);
					//_controlPoint = _mTotalMatrix.MultNormalize(_controlPoint);
					_controlPoint = _mNodeTransform.MultNormalize(_controlPoint);
					_controlPoint = _controlPoint * 0.1f;

					FVector _vertex;
					_vertex.X = (float)-_controlPoint.mData[0];
					_vertex.Y = (float)_controlPoint.mData[1];
					_vertex.Z = (float)_controlPoint.mData[2];

					_tempVertexArray.Add(_vertex);
					_tempNormalArray.Add(_vNormal);
					_tempIndexArray.Add(_iVertexCounter);
					_iVertexCounter++;

				}

			}
			pOutVertexArray->Push(_tempVertexArray);
			pOutNormalArray->Push(_tempNormalArray);
			pOutTriangleArray->Push(_tempIndexArray);


			int32 _iMatCount = _pChildNode->GetMaterialCount();

			// visit all materials
			int32 _iMaterialIndex;
			for (_iMaterialIndex = 0; _iMaterialIndex < _iMatCount; _iMaterialIndex++)
			{
				FbxSurfaceMaterial* _pMaterial = _pChildNode->GetMaterial(_iMaterialIndex);

				if (_pMaterial)
				{
					FbxProperty _Property = _pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
					FbxVector4 _vDiffuse = _Property.Get<FbxVector4>();
					//_Property = _pMaterial->FindProperty(FbxSurfaceMaterial::)
					FVector _vOut(_vDiffuse.mData[0], _vDiffuse.mData[1], _vDiffuse.mData[2]);
					pOutDiffuseArray->Add(_vOut);

					int32 _iTextureIndex;
					FBXSDK_FOR_EACH_TEXTURE(_iTextureIndex)
					{
						_Property = _pMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[_iTextureIndex]);

						if (_Property.IsValid())
						{
							int32 _iNbTextures = _Property.GetSrcObjectCount<FbxTexture>();
							for (int32 _iTexIndex = 0; _iTexIndex < _iNbTextures; _iTexIndex++)
							{

								FbxFileTexture* _pTexture = _Property.GetSrcObject<FbxFileTexture>(_iTexIndex);
								if (_pTexture)
								{
									FbxFileTexture* _pFbxTexture = _Property.GetSrcObject<FbxFileTexture>(_iTexIndex);

									// create an unreal texture asset
								}
							}

						//UTexture* UnrealTexture = ImportTexture(FbxTexture, bSetupAsNormalMap);
						}
					}
				}
			}
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

FbxMatrix FBX_Importer::ComputeTotalMatrix(FbxNode* pInNode)
{
	FbxMatrix _mGeometry;
	FbxVector4 _vTranslation, _vRotation, _vScaling;
	_vTranslation = pInNode->LclTranslation.Get();
	_vRotation = pInNode->LclRotation.Get();
	_vScaling = pInNode->LclScaling.Get();
	_mGeometry.SetTRS(_vTranslation, _vRotation, _vScaling);

	FbxMatrix _mGlobalTransform = pInNode->EvaluateGlobalTransform();//FbxScene->GetEvaluator()->GetNodeGlobalTransform(Node);

	FbxMatrix _mTotalMatrix;
	_mTotalMatrix = _mGlobalTransform * _mGeometry;

	return _mTotalMatrix;
}
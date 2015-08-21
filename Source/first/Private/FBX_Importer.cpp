// Fill out your copyright notice in the Description page of Project Settings.

#include "first.h"
#include "FBX_Importer.h"
#include <DDSLoader.h>
#include <ImageWrapper.h>

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

void FBX_Importer::LoadFBX(FString InFileName, TArray<TArray<FVector>>* pOutVertexArray, TArray<TArray<int32>>* pOutTriangleArray, TArray<TArray<FVector>>* pOutNormalArray, TArray<FVector>* pOutDiffuseArray, TArray<UTexture2D*>* pOutTextureArray, TArray<TArray<FVector2D>>* pOutUVArray, int* iOutNodeCount)
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
			TArray<FVector2D> _tempUVArray;
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

					FVector2D _vUV;
					ReadUV(_pMesh, _iControlPointIndex, _iVertexCounter, &_vUV);

					FbxVector4 _controlPoint = _pVertices[_iControlPointIndex];

					_controlPoint = _mNodeTransform.MultNormalize(_controlPoint);
					_controlPoint = _controlPoint * 0.1f;

					FVector _vertex;
					_vertex.X = (float)-_controlPoint.mData[0]; //invert x, otherwise everything is mirrored.
					_vertex.Y = (float)_controlPoint.mData[1];
					_vertex.Z = (float)_controlPoint.mData[2];

					_tempVertexArray.Add(_vertex);
					_tempNormalArray.Add(_vNormal);
					_tempIndexArray.Add(_iVertexCounter);
					_tempUVArray.Add(_vUV);
					_iVertexCounter++;

				}

			}
			pOutVertexArray->Push(_tempVertexArray);
			pOutNormalArray->Push(_tempNormalArray);
			pOutTriangleArray->Push(_tempIndexArray);
			pOutUVArray->Push(_tempUVArray);


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
									FString _sFilename = UTF8_TO_TCHAR(_pFbxTexture->GetFileName());

									UTexture2D* _pUnrealTexture = LoadTexture(_sFilename);

									if (_pUnrealTexture != NULL)
										pOutTextureArray->Push(_pUnrealTexture);


								}
							}
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

void FBX_Importer::ReadUV(FbxMesh* pInMesh, int iInControlPointIndex, int iInVertexCounter, FVector2D* pOutUV)
{
	if (pInMesh->GetElementUVCount() < 1)
	{
		return;//throw std::exception("Invalid UV Number");
	}

	if (pInMesh->GetElementUVCount() > 0)
	{
		FbxGeometryElementUV* _vertexUV = pInMesh->GetElementUV(0);
		switch (_vertexUV->GetMappingMode())
		{
			case FbxGeometryElement::eByControlPoint:
			{
				switch (_vertexUV->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						pOutUV->X = static_cast<float>(_vertexUV->GetDirectArray().GetAt(iInControlPointIndex).mData[0]);
						pOutUV->Y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(iInControlPointIndex).mData[1]);
					}
					break;
				
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = _vertexUV->GetIndexArray().GetAt(iInControlPointIndex);
						pOutUV->X = static_cast<float>(_vertexUV->GetDirectArray().GetAt(index).mData[0]);
						pOutUV->Y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(index).mData[1]);
					}
					break;

					default:
						throw std::exception("Invalid Reference");
				}
			}
			break;

			case FbxGeometryElement::eByPolygonVertex:
			{
				switch (_vertexUV->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						pOutUV->X = static_cast<float>(_vertexUV->GetDirectArray().GetAt(iInVertexCounter).mData[0]);
						pOutUV->Y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(iInVertexCounter).mData[1]);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						int index = _vertexUV->GetIndexArray().GetAt(iInVertexCounter);
						pOutUV->X = static_cast<float>(_vertexUV->GetDirectArray().GetAt(index).mData[0]);
						pOutUV->Y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(index).mData[1]);
					}
					break;

					default:
						throw std::exception("Invalid Reference");
				}
			}
			break;

		}
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

/** Loads a texture from the data path */
UTexture2D* FBX_Importer::LoadTexture(FString TextureFilename)
{
	UTexture2D* Texture = NULL;

	FString TexturePath = TextureFilename;
	const TCHAR* s = *TextureFilename;
	TArray<uint8> FileData;

	
	FString _sExtension = FPaths::GetExtension(TextureFilename).ToLower();

	TArray<uint8> RawFileData;

	if (FFileHelper::LoadFileToArray(RawFileData, *TextureFilename))
	{
		IImageWrapperModule& _ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		// Note: PNG format.  Other formats are supported
		
		EImageFormat::Type _Format = EImageFormat::BMP;
		if (_sExtension == "jpg" || "jpeg")
			_Format = EImageFormat::JPEG;

		if (_sExtension == "png")
			_Format = EImageFormat::PNG;

		IImageWrapperPtr ImageWrapper = _ImageWrapperModule.CreateImageWrapper(_Format);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			const TArray<uint8>* UncompressedBGRA = NULL;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				// Create the UTexture for rendering
				Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

				// Fill in the source data from the file
				uint8* TextureData = (uint8*)Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();

				// Update the rendering resource from data.
				Texture->UpdateResource();

			}
		}
	}





	/* Load DDS texture */
	if (FFileHelper::LoadFileToArray(FileData, *TexturePath, 0))
	{
		FDDSLoadHelper DDSLoadHelper(&FileData[0], FileData.Num());
		if (DDSLoadHelper.IsValid2DTexture())
		{
			int32 NumMips = DDSLoadHelper.ComputeMipMapCount();
			EPixelFormat Format = DDSLoadHelper.ComputePixelFormat();
			int32 BlockSize = 16;

			if (NumMips == 0)
			{
				NumMips = 1;
			}

			if (Format == PF_DXT1)
			{
				BlockSize = 8;
			}

			/* Create transient texture */
			Texture = UTexture2D::CreateTransient(DDSLoadHelper.DDSHeader->dwWidth, DDSLoadHelper.DDSHeader->dwHeight, Format);
			//Texture->MipGenSettings = TMGS_LeaveExistingMips;
			Texture->PlatformData->NumSlices = 1;
			Texture->NeverStream = true;
			Texture->Rename((TCHAR*)&"Foo");

			/* Get pointer to actual data */
			uint8* DataPtr = (uint8*)DDSLoadHelper.GetDDSDataPointer();

			uint32 CurrentWidth = DDSLoadHelper.DDSHeader->dwWidth;
			uint32 CurrentHeight = DDSLoadHelper.DDSHeader->dwHeight;

			/* Iterate through mips */
			for (int32 i = 0; i < NumMips; i++)
			{
				/* Lock to 1x1 as smallest size */
				CurrentWidth = (CurrentWidth < 1) ? 1 : CurrentWidth;
				CurrentHeight = (CurrentHeight < 1) ? 1 : CurrentHeight;

				/* Get number of bytes to read */
				int32 NumBytes = CurrentWidth * CurrentHeight * 4;
				if (Format == PF_DXT1 || Format == PF_DXT3 || Format == PF_DXT5)
				{
					/* Compressed formats */
					NumBytes = ((CurrentWidth + 3) / 4) * ((CurrentHeight + 3) / 4) * BlockSize;
				}

				/* Write to existing mip */
				if (i < Texture->PlatformData->Mips.Num())
				{
					FTexture2DMipMap& Mip = Texture->PlatformData->Mips[i];

					void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memcpy(Data, DataPtr, NumBytes);
					Mip.BulkData.Unlock();
				}

				/* Add new mip */
				else
				{
					FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
					Mip->SizeX = CurrentWidth;
					Mip->SizeY = CurrentHeight;

					Mip->BulkData.Lock(LOCK_READ_WRITE);
					Mip->BulkData.Realloc(NumBytes);
					Mip->BulkData.Unlock();

					void* Data = Mip->BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memcpy(Data, DataPtr, NumBytes);
					Mip->BulkData.Unlock();
				}

				/* Set next mip level */
				CurrentWidth /= 2;
				CurrentHeight /= 2;

				DataPtr += NumBytes;
			}

			Texture->UpdateResource();
		}
	}

	return Texture;
}
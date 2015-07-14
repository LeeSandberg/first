
#include "first.h"

// A simple procedural cylinder example
// 27. May 2015 - Sigurdur G. Gunnarsson

//include "ProceduralMesh01.h"  // Was ProceduralMesh01.h his own demo solution project file name
//#include "UProceduralMeshComponent.h" // not this one take it away
#include "SimpleCylinderActor.h"

#define MAT_HEADER_LENGTH 200

/*
ASimpleCylinderActor::ASimpleCylinderActor()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	mesh = PCIP.CreateDefaultSubobject(this, TEXT("ProceduralMesh"));
	RootComponent = mesh;
}*/

ASimpleCylinderActor::ASimpleCylinderActor()  // Orginal
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = mesh;
	lSdkManager = 0;
	ios = 0;
	lImporter = 0;
	lFilename = "";
	lImportStatus = 0; 
	lScene = 0;
}

// Draw a small box where the node is located.
void ASimpleCylinderActor::DrawMarker(FbxAMatrix& pGlobalPosition)
{
	//// GlDrawMarker(pGlobalPosition);
}

void ASimpleCylinderActor::DrawMesh(FbxNode* pNode) //, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
//	FbxAMatrix& pGlobalPosition, FbxPose* pPose) //, ShadingMode pShadingMode)
{
	FbxMesh* lMesh = pNode->GetMesh();
	const int lVertexCount = lMesh->GetControlPointsCount();

	// No vertex to draw.
	if (lVertexCount == 0)
	{
		return;
	}

	// Clear out the arrays passed in
	Triangles.Reset();

	Vertices.Reset();
	Vertices.AddUninitialized(lVertexCount);

	Normals.Reset();
	Normals.AddUninitialized(lVertexCount);

	Tangents.Reset();
	Tangents.AddUninitialized(lVertexCount);

	UVs.Reset();
	UVs.AddUninitialized(lVertexCount);

	//TArray<FVector> Vertices;
	//TArray<FVector>* lMeshCache = static_cast<TArray<FVector>*>(lMesh->GetUserDataPtr());
	//const VBOMesh * lMeshCache = static_cast<const VBOMesh *>(lMesh->GetUserDataPtr());
	
	fbxsdk::FbxVector4* normals = 0;
	lMesh->GetNormals(&normals);
	lMesh->
	// If it has some defomer connection, update the vertices position
	//const bool lHasVertexCache = lMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
		//(static_cast<FbxVertexCacheDeformer*>(lMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->IsActive();
	const bool lHasShape = lMesh->GetShapeCount() > 0;
	const bool lHasSkin = lMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
	//const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;

	FbxVector4* lVertexArray = NULL;
	if (!lMeshCache ) //|| lHasDeformation)
	{
		lVertexArray = new FbxVector4[lVertexCount];
		memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
	}

	/*if (lHasDeformation)
	{
		// Active vertex cache deformer will overwrite any other deformer
		if (lHasVertexCache)
		{
			ReadVertexCacheData(lMesh, pTime, lVertexArray);
		}
		else
		{
			if (lHasShape)
			{
				// Deform the vertex array with the shapes.
				ComputeShapeDeformation(lMesh, pTime, pAnimLayer, lVertexArray);
			}

			//we need to get the number of clusters
			const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
			int lClusterCount = 0;
			for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
			{
				lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
			}
			if (lClusterCount)
			{
				// Deform the vertex array with the skin deformer.
				ComputeSkinDeformation(pGlobalPosition, lMesh, pTime, lVertexArray, pPose);
			}
		}

		if (lMeshCache)
			lMeshCache->UpdateVertexPosition(lMesh, lVertexArray);
	} */

///	glPushMatrix();
///	glMultMatrixd((const double*)pGlobalPosition);
	/*
	if (lMeshCache)
	{
		lMeshCache->BeginDraw(pShadingMode);
		const int lSubMeshCount = lMeshCache->GetSubMeshCount();
		for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex)
		{
			if (pShadingMode == SHADING_MODE_SHADED)
			{
				const FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lIndex);
				if (lMaterial)
				{
					const MaterialCache * lMaterialCache = static_cast<const MaterialCache *>(lMaterial->GetUserDataPtr());
					if (lMaterialCache)
					{
						lMaterialCache->SetCurrentMaterial();
					}
				}
				else
				{
					// Draw green for faces without material
					MaterialCache::SetDefaultMaterial();
				}
			}

			lMeshCache->Draw(lIndex, pShadingMode);
		}
		lMeshCache->EndDraw();
	}
	else
	{
		// OpenGL driver is too lower and use Immediate Mode
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		const int lPolygonCount = lMesh->GetPolygonCount();
		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
		{
			const int lVerticeCount = lMesh->GetPolygonSize(lPolygonIndex);
			glBegin(GL_LINE_LOOP);
			for (int lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)
			{
				glVertex3dv((GLdouble *)lVertexArray[lMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex)]);
			}
			glEnd();
		}
	}

	glPopMatrix(); */

	delete[] lVertexArray;
}


void ASimpleCylinderActor::DisplayMesh(FbxNode* pNode)
{

	FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();

	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		// All lights has been processed before the whole scene because they influence every geometry.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMarker)
		{
		////	DrawMarker(pGlobalPosition);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
		////	DrawSkeleton(pNode, pParentGlobalPosition, pGlobalPosition);
		}
		// NURBS and patch have been converted into triangluation meshes.
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			// DrawMesh(pNode, pTime, pAnimLayer, pGlobalPosition, pPose, pShadingMode);
			//void DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
			// FbxAMatrix& pGlobalPosition, FbxPose* pPose, ShadingMode pShadingMode)
			DrawMesh(pNode);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eCamera)
		{
			///// DrawCamera(pNode, pTime, pAnimLayer, pGlobalPosition);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull)
		{
			///// DrawNull(pGlobalPosition);
		}
	}
	else
	{
		// Draw a Null for nodes without attribute.
		//// DrawNull(pGlobalPosition);
	}

////	DisplayString("Mesh Name: ", (char *)pNode->GetName());
//	DisplayMetaDataConnections(lMesh);
	DisplayControlsPoints(lMesh);
	DisplayPolygons(lMesh);
	DisplayMaterialMapping(lMesh);
//	DisplayMaterial(lMesh);
//	DisplayTexture(lMesh);
	DisplayMaterialConnections(lMesh);
//	DisplayLink(lMesh);
//	DisplayShape(lMesh);

//	DisplayCache(lMesh);

	//GenerateCylinder(Vertices, Triangles, Normals, UVs, Tangents, Height, Radius, CrossSectionCount, bCapEnds, bDoubleSided, bSmoothNormals);

	mesh->ClearAllMeshSections();
	mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	mesh->MarkRenderStateDirty();  // Needs to be here in order for collision to work on things we need collision on.

	void GenerateMesh();
}


void ASimpleCylinderActor::DisplayControlsPoints(FbxMesh* pMesh)
{
	int i, lControlPointsCount = pMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();

////	DisplayString("    Control Points");

	for (i = 0; i < lControlPointsCount; i++)
	{
	////	DisplayInt("        Control Point ", i);
	////	Display3DVector("            Coordinates: ", lControlPoints[i]);

		for (int j = 0; j < pMesh->GetElementNormalCount(); j++)
		{
			FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(j);
			if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				char header[100];
				FBXSDK_sprintf(header, 100, "            Normal Vector: ");
				if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					////	Display3DVector(header, leNormals->GetDirectArray().GetAt(i));
				}
			}
		}
	}

	//// DisplayString("");
}


void ASimpleCylinderActor::DisplayPolygons(FbxMesh* pMesh)
{
	int i, j, lPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	char header[100];

	//// DisplayString("    Polygons");

	int vertexId = 0;
	for (i = 0; i < lPolygonCount; i++)
	{
	////	DisplayInt("        Polygon ", i);
		int l;

		for (l = 0; l < pMesh->GetElementPolygonGroupCount(); l++)
		{
			FbxGeometryElementPolygonGroup* lePolgrp = pMesh->GetElementPolygonGroup(l);
			switch (lePolgrp->GetMappingMode())
			{
			case FbxGeometryElement::eByPolygon:
				if (lePolgrp->GetReferenceMode() == FbxGeometryElement::eIndex)
				{
					FBXSDK_sprintf(header, 100, "        Assigned to group: ");
					int polyGroupId = lePolgrp->GetIndexArray().GetAt(i);
			////		DisplayInt(header, polyGroupId);
					break;
				}
			default:
				// any other mapping modes don't make sense
			////	DisplayString("        \"unsupported group assignment\"");
				break;
			}
		}

		int lPolygonSize = pMesh->GetPolygonSize(i);

		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

		////	Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);

			for (l = 0; l < pMesh->GetElementVertexColorCount(); l++)
			{
				FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor(l);
				FBXSDK_sprintf(header, 100, "            Color vertex: ");

				switch (leVtxc->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					////	DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
				////		DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					////	DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leVtxc->GetIndexArray().GetAt(vertexId);
					////	DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}
			for (l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);
				FBXSDK_sprintf(header, 100, "            Texture UV: ");

				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
				////		Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
				////		Display2DVector(header, leUV->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
				////		Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}
			for (l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);
				FBXSDK_sprintf(header, 100, "            Normal: ");

				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
				////		Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
				////		Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(l);
				FBXSDK_sprintf(header, 100, "            Tangent: ");

				if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leTangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
				////		Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leTangent->GetIndexArray().GetAt(vertexId);
				////		Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			{

				FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal(l);

				FBXSDK_sprintf(header, 100, "            Binormal: ");
				if (leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leBinormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
				////		Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leBinormal->GetIndexArray().GetAt(vertexId);
				////		Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
			}
			vertexId++;
		} // for polygonSize
	} // for polygonCount


	//check visibility for the edges of the mesh
	for (int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
	{
		FbxGeometryElementVisibility* leVisibility = pMesh->GetElementVisibility(l);
		FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
	////	DisplayString(header);
		switch (leVisibility->GetMappingMode())
		{
		default:
			break;
			//should be eByEdge
		case FbxGeometryElement::eByEdge:
			//should be eDirect
			for (int j = 0; j != pMesh->GetMeshEdgeCount(); ++j)
			{
		////		DisplayInt("        Edge ", j);
		////		DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
			}

			break;
		}
	}
////	DisplayString("");
}

void ASimpleCylinderActor::DisplayTextureNames(FbxProperty &pProperty, FbxString& pConnectionString)
{
	int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
	if (lLayeredTextureCount > 0)
	{
		for (int j = 0; j<lLayeredTextureCount; ++j)
		{
			FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
			int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
			pConnectionString += " Texture ";

			for (int k = 0; k<lNbTextures; ++k)
			{
				//lConnectionString += k;
				pConnectionString += "\"";
				pConnectionString += (char*)lLayeredTexture->GetName();
				pConnectionString += "\"";
				pConnectionString += " ";
			}
			pConnectionString += "of ";
			pConnectionString += pProperty.GetName();
			pConnectionString += " on layer ";
			pConnectionString += j;
		}
		pConnectionString += " |";
	}
	else
	{
		//no layered texture simply get on the property
		int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();

		if (lNbTextures > 0)
		{
			pConnectionString += " Texture ";
			pConnectionString += " ";

			for (int j = 0; j<lNbTextures; ++j)
			{
				FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
				if (lTexture)
				{
					pConnectionString += "\"";
					pConnectionString += (char*)lTexture->GetName();
					pConnectionString += "\"";
					pConnectionString += " ";
				}
			}
			pConnectionString += "of ";
			pConnectionString += pProperty.GetName();
			pConnectionString += " |";
		}
	}
}

void ASimpleCylinderActor::DisplayMaterialTextureConnections(FbxSurfaceMaterial* pMaterial, char * header, int pMatId, int l)
{
	if (!pMaterial)
		return;

	FbxString lConnectionString = "            Material %d -- ";
	//Show all the textures

	FbxProperty lProperty;
	//Diffuse Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	DisplayTextureNames(lProperty, lConnectionString);

	//DiffuseFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Emissive Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
	DisplayTextureNames(lProperty, lConnectionString);

	//EmissiveFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);
	DisplayTextureNames(lProperty, lConnectionString);


	//Ambient Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
	DisplayTextureNames(lProperty, lConnectionString);

	//AmbientFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbientFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Specular Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
	DisplayTextureNames(lProperty, lConnectionString);

	//SpecularFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Shininess Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	DisplayTextureNames(lProperty, lConnectionString);

	//Bump Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sBump);
	DisplayTextureNames(lProperty, lConnectionString);

	//Normal Map Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
	DisplayTextureNames(lProperty, lConnectionString);

	//Transparent Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
	DisplayTextureNames(lProperty, lConnectionString);

	//TransparencyFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Reflection Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflection);
	DisplayTextureNames(lProperty, lConnectionString);

	//ReflectionFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Update header with material info
	bool lStringOverflow = (lConnectionString.GetLen() + 10 >= MAT_HEADER_LENGTH); // allow for string length and some padding for "%d"
	if (lStringOverflow)
	{
		// Truncate string!
		lConnectionString = lConnectionString.Left(MAT_HEADER_LENGTH - 10);
		lConnectionString = lConnectionString + "...";
	}
	FBXSDK_sprintf(header, MAT_HEADER_LENGTH, lConnectionString.Buffer(), pMatId, l);
	//// DisplayString(header);
}

void ASimpleCylinderActor::DisplayMaterialConnections(FbxMesh* pMesh)
{
	int i, l, lPolygonCount = pMesh->GetPolygonCount();

	char header[MAT_HEADER_LENGTH];

////	DisplayString("    Polygons Material Connections");

	//check whether the material maps with only one mesh
	bool lIsAllSame = true;
	for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
	{

		FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
		if (lMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
		{
			lIsAllSame = false;
			break;
		}
	}

	//For eAllSame mapping type, just out the material and texture mapping info once
	if (lIsAllSame)
	{
		for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
		{

			FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
			if (lMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame)
			{
				FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(0));
				int lMatId = lMaterialElement->GetIndexArray().GetAt(0);
				if (lMatId >= 0)
				{
					//// DisplayInt("        All polygons share the same material in mesh ", l);
					DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
				}
			}
		}

		//no material
		if (l == 0)
		{
			////			DisplayString("        no material applied");
		}
	}

	//For eByPolygon mapping type, just out the material and texture mapping info once
	else
	{
		for (i = 0; i < lPolygonCount; i++)
		{
		/////	DisplayInt("        Polygon ", i);

			for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
			{

				FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
				FbxSurfaceMaterial* lMaterial = NULL;
				int lMatId = -1;
				lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(i));
				lMatId = lMaterialElement->GetIndexArray().GetAt(i);

				if (lMatId >= 0)
				{
					DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
				}
			}
		}
	}
}


void ASimpleCylinderActor::DisplayMaterialMapping(FbxMesh* pMesh)
{
	const char* lMappingTypes[] = { "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" };
	const char* lReferenceMode[] = { "Direct", "Index", "Index to Direct" };

	int lMtrlCount = 0;
	FbxNode* lNode = NULL;
	if (pMesh){
		lNode = pMesh->GetNode();
		if (lNode)
			lMtrlCount = lNode->GetMaterialCount();
	}

	for (int l = 0; l < pMesh->GetElementMaterialCount(); l++)
	{
		FbxGeometryElementMaterial* leMat = pMesh->GetElementMaterial(l);
		if (leMat)
		{
			char header[100];
			FBXSDK_sprintf(header, 100, "    Material Element %d: ", l);
		////	DisplayString(header);


	////		DisplayString("           Mapping: ", lMappingTypes[leMat->GetMappingMode()]);
	////		DisplayString("           ReferenceMode: ", lReferenceMode[leMat->GetReferenceMode()]);

			int lMaterialCount = 0;
			FbxString lString;

			if (leMat->GetReferenceMode() == FbxGeometryElement::eDirect ||
				leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				lMaterialCount = lMtrlCount;
			}

			if (leMat->GetReferenceMode() == FbxGeometryElement::eIndex ||
				leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				int i;

				lString = "           Indices: ";

				int lIndexArrayCount = leMat->GetIndexArray().GetCount();
				for (i = 0; i < lIndexArrayCount; i++)
				{
					lString += leMat->GetIndexArray().GetAt(i);

					if (i < lIndexArrayCount - 1)
					{
						lString += ", ";
					}
				}

				lString += "\n";

				FBXSDK_printf(lString);
			}
		}
	}

////	DisplayString("");
}


void ASimpleCylinderActor::SetupImport()
{
	// Create the FBX SDK manager
	lSdkManager = FbxManager::Create();

	// Create an IOSettings object.
	ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// ... Configure the FbxIOSettings object ...

	// Create an importer.
	lImporter = FbxImporter::Create(lSdkManager, "");

	// Declare the path and filename of the file containing the scene.
	// In this case, we are assuming the file is in the same directory as the executable.
	lFilename = "file.fbx";

	// Initialize the importer.
	lImportStatus = lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings());

	if (!lImportStatus) {
		//"This is a message to yourself during runtime!"
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Call to FbxImporter::Initialize() failed.\n"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, lImporter->GetStatus().GetErrorString());
		//printf("Call to FbxImporter::Initialize() failed.\n");
		//printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
		//exit(-1);
	}

	// Create a new scene so it can be populated by the imported file.
	lScene = FbxScene::Create(lSdkManager, "myScene");
	
}

void ASimpleCylinderActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateMesh();
}

void ASimpleCylinderActor::BeginPlay()
{
	Super::BeginPlay();
	GenerateMesh();

	// The file has been imported; we can get rid of the importer.
	lImporter->Destroy();
}



void ASimpleCylinderActor::DisplayHierarchy(FbxScene* pScene)
{
	int i;
	FbxNode* lRootNode = pScene->GetRootNode();

	for (i = 0; i < lRootNode->GetChildCount(); i++)
	{
		DisplayHierarchy(lRootNode->GetChild(i), 0);
	}
}

void ASimpleCylinderActor::DisplayHierarchy(FbxNode* pNode, int pDepth)
{

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;

	//FbxString lString;
	int i;
	//GenerateCylinder(Vertices, Triangles, Normals, UVs, Tangents, Height, Radius, CrossSectionCount, bCapEnds, bDoubleSided, bSmoothNormals);

	mesh->ClearAllMeshSections();
	mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	mesh->MarkRenderStateDirty();  // Needs to be here in order for collision to work on things we need collision on.
	
	const char * name  = pNode->GetClassId().GetName();
	
	if (true)
	{
		DisplayMesh(pNode);
	}
	
/*	for (i = 0; i < pDepth; i++)
	{
		lString += "     ";
	}

	lString += pNode->GetName();
	lString += "\n";

	FBXSDK_printf(lString.Buffer());
	*/



	for (i = 0; i < pNode->GetChildCount(); i++)
	{
		DisplayHierarchy(pNode->GetChild(i), pDepth + 1);
	}
}

void ASimpleCylinderActor::GenerateMesh()
{
	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	FbxNode* lRootNode = lScene->GetRootNode();

	DisplayHierarchy(lScene);
}

void ASimpleCylinderActor::GenerateCylinder(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents, float InHeight, float InWidth, int32 InCrossSectionCount, bool bInCapEnds, bool bInDoubleSided, bool bInSmoothNormals)
{
	// -------------------------------------------------------
	// Basic setup
	int VertexIndex = 0;
	int32 NumVerts = InCrossSectionCount * 4; // InCrossSectionCount x 4 verts per face

	// Count extra vertices if double sided
	if (bInDoubleSided)
	{
		NumVerts = NumVerts * 2;
	}

	// Count vertices for caps if set
	if (bInCapEnds)
	{
		NumVerts += 2 * (InCrossSectionCount - 1) * 3;
	}

	// Clear out the arrays passed in
	Triangles.Reset();

	Vertices.Reset();
	Vertices.AddUninitialized(NumVerts);

	Normals.Reset();
	Normals.AddUninitialized(NumVerts);

	Tangents.Reset();
	Tangents.AddUninitialized(NumVerts);

	UVs.Reset();
	UVs.AddUninitialized(NumVerts);

	// -------------------------------------------------------
	// Make a cylinder section
	const float AngleBetweenQuads = (2.0f / (float)(InCrossSectionCount)) * PI;
	const float VMapPerQuad = 1.0f / (float)InCrossSectionCount;
	FVector Offset = FVector(0, 0, InHeight);

	// Start by building up vertices that make up the cylinder sides
	for (int32 QuadIndex = 0; QuadIndex < InCrossSectionCount; QuadIndex++)
	{
		float Angle = (float)QuadIndex * AngleBetweenQuads;
		float NextAngle = (float)(QuadIndex + 1) * AngleBetweenQuads;

		// Set up the vertices
		FVector p0 = FVector(FMath::Cos(Angle) * InWidth, FMath::Sin(Angle) * InWidth, 0.f);
		FVector p1 = FVector(FMath::Cos(NextAngle) * InWidth, FMath::Sin(NextAngle) * InWidth, 0.f);
		FVector p2 = p1 + Offset;
		FVector p3 = p0 + Offset;

		// Set up the quad triangles
		int VertIndex1 = VertexIndex++;
		int VertIndex2 = VertexIndex++;
		int VertIndex3 = VertexIndex++;
		int VertIndex4 = VertexIndex++;

		Vertices[VertIndex1] = p0;
		Vertices[VertIndex2] = p1;
		Vertices[VertIndex3] = p2;
		Vertices[VertIndex4] = p3;

		// Now create two triangles from those four vertices
		// The order of these (clockwise/counter-clockwise) dictates which way the normal will face. 
		Triangles.Add(VertIndex4);
		Triangles.Add(VertIndex3);
		Triangles.Add(VertIndex1);

		Triangles.Add(VertIndex3);
		Triangles.Add(VertIndex2);
		Triangles.Add(VertIndex1);

		// UVs
		UVs[VertIndex1] = FVector2D(VMapPerQuad * QuadIndex, 0.0f);
		UVs[VertIndex2] = FVector2D(VMapPerQuad * (QuadIndex + 1), 0.0f);
		UVs[VertIndex3] = FVector2D(VMapPerQuad * (QuadIndex + 1), 1.0f);
		UVs[VertIndex4] = FVector2D(VMapPerQuad * QuadIndex, 1.0f);

		// Normals
		FVector NormalCurrent = FVector::CrossProduct(Vertices[VertIndex1] - Vertices[VertIndex3], Vertices[VertIndex2] - Vertices[VertIndex3]).GetSafeNormal();

		if (bInSmoothNormals)
		{
			// To smooth normals you give the vertices a different normal value than the polygon they belong to, gfx hardware then knows how to interpolate between those.
			// I do this here as an average between normals of two adjacent polygons
			// TODO re-use calculations between loop iterations (do them once and cache them!), no need to calculate same values every time :)
			float NextNextAngle = (float)(QuadIndex + 2) * AngleBetweenQuads;
			FVector p4 = FVector(FMath::Cos(NextNextAngle) * InWidth, FMath::Sin(NextNextAngle) * InWidth, 0.f);

			// p1 to p4 to p2
			FVector NormalNext = FVector::CrossProduct(p1 - p2, p4 - p2).GetSafeNormal();
			FVector AverageNormalRight = (NormalCurrent + NormalNext) / 2;
			AverageNormalRight = AverageNormalRight.GetSafeNormal();

			float PreviousAngle = (float)(QuadIndex - 1) * AngleBetweenQuads;
			FVector pMinus1 = FVector(FMath::Cos(PreviousAngle) * InWidth, FMath::Sin(PreviousAngle) * InWidth, 0.f);

			// p0 to p3 to pMinus1
			FVector NormalPrevious = FVector::CrossProduct(p0 - pMinus1, p3 - pMinus1).GetSafeNormal();
			FVector AverageNormalLeft = (NormalCurrent + NormalPrevious) / 2;
			AverageNormalLeft = AverageNormalLeft.GetSafeNormal();

			Normals[VertIndex1] = AverageNormalLeft;
			Normals[VertIndex2] = AverageNormalRight;
			Normals[VertIndex3] = AverageNormalRight;
			Normals[VertIndex4] = AverageNormalLeft;
		}
		else
		{
			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			Normals[VertIndex1] = NormalCurrent;
			Normals[VertIndex2] = NormalCurrent;
			Normals[VertIndex3] = NormalCurrent;
			Normals[VertIndex4] = NormalCurrent;
		}

		// Tangents (perpendicular to the surface)
		FVector SurfaceTangent = p0 - p1;
		SurfaceTangent = SurfaceTangent.GetSafeNormal();
		Tangents[VertIndex1] = FProcMeshTangent(SurfaceTangent, true);
		Tangents[VertIndex2] = FProcMeshTangent(SurfaceTangent, true);
		Tangents[VertIndex3] = FProcMeshTangent(SurfaceTangent, true);
		Tangents[VertIndex4] = FProcMeshTangent(SurfaceTangent, true);

		// If double sides, create extra polygons but face the normals the other way.
		if (bInDoubleSided)
		{
			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			VertIndex4 = VertexIndex++;

			Vertices[VertIndex1] = p0;
			Vertices[VertIndex2] = p1;
			Vertices[VertIndex3] = p2;
			Vertices[VertIndex4] = p3;

			Triangles.Add(VertIndex1);
			Triangles.Add(VertIndex3);
			Triangles.Add(VertIndex4);

			Triangles.Add(VertIndex2);
			Triangles.Add(VertIndex3);
			Triangles.Add(VertIndex4);
		}

		if (QuadIndex != 0 && bInCapEnds)
		{
			// Cap is closed by triangles that start at 0, then use the points at the angles for the other corners

			// Bottom
			FVector capVertex0 = FVector(FMath::Cos(0) * InWidth, FMath::Sin(0) * InWidth, 0.f);
			FVector capVertex1 = FVector(FMath::Cos(Angle) * InWidth, FMath::Sin(Angle) * InWidth, 0.f);
			FVector capVertex2 = FVector(FMath::Cos(NextAngle) * InWidth, FMath::Sin(NextAngle) * InWidth, 0.f);

			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			Vertices[VertIndex1] = capVertex0;
			Vertices[VertIndex2] = capVertex1;
			Vertices[VertIndex3] = capVertex2;

			Triangles.Add(VertIndex1);
			Triangles.Add(VertIndex2);
			Triangles.Add(VertIndex3);

			FVector2D UV1 = FVector2D(FMath::Sin(0), FMath::Cos(0));
			FVector2D UV2 = FVector2D(FMath::Sin(Angle), FMath::Cos(Angle));
			FVector2D UV3 = FVector2D(FMath::Sin(NextAngle), FMath::Cos(NextAngle));

			UVs[VertIndex1] = UV1;
			UVs[VertIndex2] = UV2;
			UVs[VertIndex3] = UV3;

			// Top
			capVertex0 = capVertex0 + Offset;
			capVertex1 = capVertex1 + Offset;
			capVertex2 = capVertex2 + Offset;

			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			Vertices[VertIndex1] = capVertex0;
			Vertices[VertIndex2] = capVertex1;
			Vertices[VertIndex3] = capVertex2;

			Triangles.Add(VertIndex3);
			Triangles.Add(VertIndex2);
			Triangles.Add(VertIndex1);

			UVs[VertIndex1] = UV1;
			UVs[VertIndex2] = UV2;
			UVs[VertIndex3] = UV3;
		}
	}
}

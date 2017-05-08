#include "FbxConverter.h"

#include <stdint.h>
#include <vector>
#include "Mesh.h"

FbxConverter::FbxConverter()
{
    this->mFbxManager   = nullptr;
    this->mFbxScene     = nullptr;
    this->mHasAnimation = true;
}

FbxConverter::~FbxConverter()
{
    mFbxScene->Destroy( true );
    mFbxManager->Destroy();
}

void FbxConverter::Initialize()
{
    mFbxManager = FbxManager::Create();
    assert( mFbxManager );

    FbxIOSettings* pSettings = FbxIOSettings::Create( mFbxManager, IOSROOT );
    assert( pSettings );

    mFbxManager->SetIOSettings( pSettings );
    mFbxScene = FbxScene::Create( mFbxManager, "NewScene" );
}

void FbxConverter::LoadScene( const char* pFileName )
{
    FbxImporter* pImporter = FbxImporter::Create( mFbxManager, "NewImporter" );
    assert( pImporter );

    bool success = pImporter->Initialize( pFileName, -1, mFbxManager->GetIOSettings() );
    assert( success );

    success = pImporter->Import( mFbxScene );
    assert( success );

    pImporter->Destroy();
}

void FbxConverter::Export( const char* pFileName )
{
    ProcessGeometry( mFbxScene->GetRootNode() );
}

void FbxConverter::ProcessMesh( FbxNode* pNode )
{
    FbxMesh* inMesh = pNode->GetMesh();
    uint32_t triangleCount = inMesh->GetPolygonCount();
    uint32_t vertexCounter = 0;

    Mesh* outMesh = new Mesh();
    outMesh->Triangles.reserve( triangleCount );

    for (uint32_t i = 0; i < triangleCount; i++)
    {
        Triangle* triangle = new Triangle();

        for (uint32_t j = 0; j < 3; j++)
        {
            int cpIndex = inMesh->GetPolygonVertex( i, j );
            ControlPoint* cp = mControlPoints[cpIndex];
            
            Vertex* vertex = new Vertex();
            vertex->Position = glm::vec3(cp->Coordinates);
            GetNormal( inMesh, vertexCounter, &vertex->Normal );

            triangle->Vertices[j] = vertex;
        }

        outMesh->Triangles.push_back( triangle );
    }

    delete outMesh;
}

void FbxConverter::ProcessControlPoints( FbxNode* pNode )
{
    FbxMesh* inMesh = pNode->GetMesh();

    uint32_t count = inMesh->GetControlPointsCount();

    FbxVector4 ctrlPt;
    for (uint32_t i = 0; i < count; i++)
    {
        ctrlPt = inMesh->GetControlPointAt( i );

        ControlPoint* cp = new ControlPoint();
        cp->Coordinates = glm::vec3( ctrlPt.mData[0], ctrlPt.mData[1], ctrlPt.mData[2] );

        mControlPoints[i] = cp;
    }
}

void FbxConverter::ProcessGeometry( FbxNode* pNode )
{
    if (pNode->GetNodeAttribute())
    {
        switch (pNode->GetNodeAttribute()->GetAttributeType())
        {
        case FbxNodeAttribute::eMesh:
            ProcessControlPoints( pNode );
            ProcessMesh( pNode );
            FreeControlPoints();
            break;
        default:
            break;
        }
    }

    for (uint32_t i = 0; i < pNode->GetChildCount(); i++)
    {
        ProcessGeometry( pNode->GetChild( i ) );
    }
}

void FbxConverter::GetNormal( const FbxMesh* pMesh, int vertexIndex, glm::vec3* pNormal )
{
    assert( pMesh->GetElementNormalCount() > 0, "This mesh has no normal data" );

    const FbxGeometryElementNormal* normal = pMesh->GetElementNormal( 0 );

    switch (normal->GetMappingMode())
    {
    case FbxLayerElement::eByPolygonVertex:
        {
            switch (normal->GetReferenceMode())
            {
            case FbxLayerElement::eDirect:
                pNormal->x = static_cast<float>(normal->GetDirectArray().GetAt( vertexIndex ).mData[0]);
                pNormal->y = static_cast<float>(normal->GetDirectArray().GetAt( vertexIndex ).mData[1]);
                pNormal->z = static_cast<float>(normal->GetDirectArray().GetAt( vertexIndex ).mData[2]);
                break;
            default:
                throw std::exception( "Feature not implemented" );
            }

            break;
        }
    default:
        throw std::exception( "Feature not implemented" );
    }
}

void FbxConverter::FreeControlPoints()
{
    for (auto it = mControlPoints.begin(); it != mControlPoints.end(); ++it)
    {
        delete it->second;
    }

    mControlPoints.clear();
}

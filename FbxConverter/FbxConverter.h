#pragma once

#include "fbxsdk.h"
#include "ControlPoint.h"

#include <unordered_map>

class FbxConverter
{
public:
    FbxConverter();
    ~FbxConverter();

    void Initialize();
    void LoadScene( const char* pFileName );
    void Export( const char* pFileName );

protected:
    FbxManager* mFbxManager;
    FbxScene* mFbxScene;
    bool mHasAnimation;
    std::unordered_map<uint32_t, ControlPoint*> mControlPoints;

    void ProcessControlPoints( FbxNode* pNode );
    void ProcessGeometry( FbxNode* pNode );
    void ProcessMesh( FbxNode* pNode );
    
    void GetNormal( const FbxMesh* pMesh, int vertexIndex, glm::vec3* pNormal );

    void FreeControlPoints();
};

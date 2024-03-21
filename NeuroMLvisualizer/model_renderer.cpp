#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "model_renderer.h"



ModelRenderer::ModelRenderer(const char* path)
{
    //VAO
    glGenVertexArrays(1, &modelVAO);
    glBindVertexArray(modelVAO);

    std::vector<unsigned short> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs; //Discard uvs.
    std::vector<glm::vec3> normals;
    bool res2 = loadAssImp(path, indices, positions, uvs, normals);
    modelIndexCount = indices.size();

    // indecies
    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    // 1rst attribute buffer : positions
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 2nd attribute buffer : normals
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ModelRenderer::~ModelRenderer()
{
    glDeleteBuffers(1, &indexbuffer);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteVertexArrays(1, &modelVAO);
}

void ModelRenderer::RenderModel()
{
    glBindVertexArray(modelVAO);

    glDrawElements(GL_TRIANGLES, modelIndexCount, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
}

bool ModelRenderer::loadAssImp(
    const char * path, 
    std::vector<unsigned short> & indices,
    std::vector<glm::vec3> & vertices,
    std::vector<glm::vec2> & uvs,
    std::vector<glm::vec3> & normals
){

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
    if( !scene) {
        fprintf( stderr, importer.GetErrorString());
        getchar();
        return false;
    }
    const aiMesh* mesh = scene->mMeshes[0]; // Only use the 1rst mesh (in OBJ files there is often only one anyway)

    // Fill vertices positions
    vertices.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D pos = mesh->mVertices[i];
        vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
    }

    // Fill vertices texture coordinates
    uvs.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
        uvs.push_back(glm::vec2(UVW.x, UVW.y));
    }

    // Fill vertices normals
    normals.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D n = mesh->mNormals[i];
        normals.push_back(glm::vec3(n.x, n.y, n.z));
    }


    // Fill face indices
    indices.reserve(3*mesh->mNumFaces);
    for (unsigned int i=0; i<mesh->mNumFaces; i++){
        // Assume the model has only triangles.
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }
    
    // The "scene" pointer will be deleted automatically by "importer"
    return true;
}



InstancedModelRenderer::InstancedModelRenderer(ModelRenderer& modelRenderer)
{
    instanceCount = 0;
    modelIndexCount = modelRenderer.modelIndexCount;

    //VAO
    glGenVertexArrays(1, &modelVAO);
    glBindVertexArray(modelVAO);

    // indecies
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelRenderer.indexbuffer);

    // 1rst attribute buffer : positions
    glBindBuffer(GL_ARRAY_BUFFER, modelRenderer.vertexbuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 2nd attribute buffer : normals
    glBindBuffer(GL_ARRAY_BUFFER, modelRenderer.normalbuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 3rd attribute buffer : topScale (per instance)
    glGenBuffers(1, &instanceTopScaleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceTopScaleBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(2, 1);

    // 4th attribute buffer : bottomScale (per instance)
    glGenBuffers(1, &instanceBottomScaleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBottomScaleBuffer);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(3, 1);

    // 5th attribute buffer : model matrices (per instance)
    glGenBuffers(1, &instanceMatrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glVertexAttribDivisor(6, 1);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glVertexAttribDivisor(7, 1);
    

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

InstancedModelRenderer::~InstancedModelRenderer()
{
    glDeleteVertexArrays(1, &modelVAO);
}

void InstancedModelRenderer::RenderModel()
{
    glBindVertexArray(modelVAO);

    glDrawElementsInstanced(GL_TRIANGLES, modelIndexCount, GL_UNSIGNED_SHORT, 0, instanceCount);

    glBindVertexArray(0);
}

void InstancedModelRenderer::setTopScales(float* topScales, int count)
{
    glBindVertexArray(modelVAO);

    glBindBuffer(GL_ARRAY_BUFFER, instanceTopScaleBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), &topScales[0], GL_STATIC_DRAW);
    instanceCount = count;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void InstancedModelRenderer::setBottomScales(float* bottomScales, int count)
{
    glBindVertexArray(modelVAO);

    glBindBuffer(GL_ARRAY_BUFFER, instanceBottomScaleBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), &bottomScales[0], GL_STATIC_DRAW);
    instanceCount = count;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedModelRenderer::setModelMatreces(glm::mat4* modelMatrices, int count)
{
    glBindVertexArray(modelVAO);

    glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    instanceCount = count;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
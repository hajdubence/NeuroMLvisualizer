#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


class ModelRenderer
{
public:

    int modelIndexCount;
    GLuint indexbuffer;
    GLuint vertexbuffer;
    GLuint normalbuffer;

    ModelRenderer(const char* path);
    ~ModelRenderer();
    void RenderModel();

private:

    GLuint modelVAO;

    bool loadAssImp(
        const char* path,
        std::vector<unsigned short>& indices,
        std::vector<glm::vec3>& vertices,
        std::vector<glm::vec2>& uvs,
        std::vector<glm::vec3>& normals);
};

class InstancedModelRenderer
{
public:

    InstancedModelRenderer(ModelRenderer& modelRenderer);
    ~InstancedModelRenderer();
    void RenderModel();

    void setTopScales(float* topScales, int count);
    void setBottomScales(float* bottomScales, int count);
    void setModelMatreces(glm::mat4* modelMatrices, int count);

private:

    GLuint modelVAO;
    int modelIndexCount;

    GLuint instanceTopScaleBuffer;
    GLuint instanceBottomScaleBuffer;
    GLuint instanceMatrixBuffer;
    int instanceCount;

};


#endif
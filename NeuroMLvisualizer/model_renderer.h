#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


class ModelRenderer
{
public:

    ModelRenderer(const char* path);
    ~ModelRenderer();
    void RenderModel();

private:

    GLuint modelVAO;
    GLuint indexbuffer;
    GLuint vertexbuffer;
    GLuint normalbuffer;
    int modelIndexCount;

    bool loadAssImp(
        const char* path,
        std::vector<unsigned short>& indices,
        std::vector<glm::vec3>& vertices,
        std::vector<glm::vec2>& uvs,
        std::vector<glm::vec3>& normals);
};


#endif
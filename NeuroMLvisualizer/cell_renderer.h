#ifndef CELL_RENDERER_H
#define CELL_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "common_structs.h"
#include "model_renderer.h"
#include "shader.h"

class CellRenderer
{
public:

    CellRenderer();
    CellRenderer(Cell cell);
    ~CellRenderer();

    void RenderCell(CylinderShader* cylinderShader, glm::mat4 modelMatrix);

private:

    ModelRenderer cylinder = ModelRenderer("cylinder3.obj");
    ModelRenderer sphere = ModelRenderer("sphere.obj");
    Cell cellData;
    
};



#endif
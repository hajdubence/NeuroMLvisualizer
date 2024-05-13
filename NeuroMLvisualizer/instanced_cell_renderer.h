#ifndef INSTANCED_CELL_RENDERER_H
#define INSTANCED_CELL_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "common_structs.h"
#include "model_renderer.h"
#include "shader.h"

class InstancedCellRenderer
{
public:

    InstancedCellRenderer(Cell cell);
    ~InstancedCellRenderer();

    void RenderCell(CylinderShader* cylinderShader, glm::mat4 modelMatrix);

private:

    Cell cellData;

    static ModelRenderer& getSphere();
    static ModelRenderer& getCylinder();

    InstancedModelRenderer spheres = InstancedModelRenderer(getSphere());
    InstancedModelRenderer cylinders = InstancedModelRenderer(getCylinder());

};



#endif
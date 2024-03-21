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

protected:

    Cell cellData;

    static ModelRenderer& getSphere();
    static ModelRenderer& getCylinder();

};

class InstancedCellRenderer : public CellRenderer
{
public:

    InstancedCellRenderer(Cell cell);
    ~InstancedCellRenderer();

    void RenderCell(CylinderShader* cylinderShader, glm::mat4 modelMatrix);

private:

    InstancedModelRenderer sphere = InstancedModelRenderer(getSphere());
    InstancedModelRenderer cylinder = InstancedModelRenderer(getCylinder());

};



#endif
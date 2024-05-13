#include "instanced_cell_renderer.h"
#include "pugixml/pugixml.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <map>



InstancedCellRenderer::InstancedCellRenderer(Cell cell)
{
    cellData = cell;
    int sphereInstanceCount = cellData.sphereVector.size();
    float* sphereTopScales;
    float* sphereBottomScales;
    glm::mat4* sphereModelMatrices;
    sphereModelMatrices = new glm::mat4[sphereInstanceCount];
    sphereTopScales = new float[sphereInstanceCount];
    sphereBottomScales = new float[sphereInstanceCount];
    for (int i = 0; i < sphereInstanceCount; i++)
    {
        glm::vec4 center = cellData.sphereVector[i];
        glm::mat4 model;

        // 1. scale
        model = glm::scale(glm::mat4(1.0f), glm::vec3(center.w, center.w, center.w));

        // 2. translate
        model = glm::translate(glm::mat4(1.0f), glm::vec3(center)) * model;

        sphereTopScales[i] = 1.0f;
        sphereBottomScales[i] = 1.0f;
        sphereModelMatrices[i] = model;
    }
    spheres.setTopScales(sphereTopScales, sphereInstanceCount);
    spheres.setBottomScales(sphereBottomScales, sphereInstanceCount);
    spheres.setModelMatreces(sphereModelMatrices, sphereInstanceCount);
    delete[] sphereTopScales;
    delete[] sphereBottomScales;
    delete[] sphereModelMatrices;
    
    int cylinderInstanceCount = cellData.cylinderVector.size() / 2;
    float* cylinderTopScales;
    float* cylinderBottomScales;
    glm::mat4* cylinderModelMatrices;
    cylinderTopScales = new float[cylinderInstanceCount];
    cylinderBottomScales = new float[cylinderInstanceCount];
    cylinderModelMatrices = new glm::mat4[cylinderInstanceCount];
    for (int i = 0; i < cylinderInstanceCount; i++)
    {
        // top = proximal  bottom = distal
        glm::vec4 prox = cellData.cylinderVector[i * 2];
        glm::vec4 dist = cellData.cylinderVector[i * 2 + 1];
        glm::mat4 model;
        
        // 1. scale
        float distance = glm::distance(glm::vec3(prox), glm::vec3(dist));
        model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, distance, 1.0f));

        // 2. rotate
        glm::vec3 oldDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 newDirection = glm::normalize(glm::vec3(prox) - glm::vec3(dist));
        glm::vec3 rotAxis = glm::cross(oldDirection, newDirection);
        if (rotAxis == glm::vec3(0, 0, 0)) { // when they are pararell
            rotAxis = glm::vec3(1, 0, 0); // use the X axis
        }
        float cosTheta = glm::dot(oldDirection, newDirection);
        float rotAngle = acos(cosTheta);
        model = glm::rotate(glm::mat4(1.0f), rotAngle, rotAxis) * model;

        // 3. translate
        glm::vec3 midPoint = (glm::vec3(prox) + glm::vec3(dist)) / 2.0f;
        model = glm::translate(glm::mat4(1.0f), midPoint) * model;

        cylinderTopScales[i] = prox.w;
        cylinderBottomScales[i] = dist.w;
        cylinderModelMatrices[i] = model;
    }
    cylinders.setTopScales(cylinderTopScales, cylinderInstanceCount);
    cylinders.setBottomScales(cylinderBottomScales, cylinderInstanceCount);
    cylinders.setModelMatreces(cylinderModelMatrices, cylinderInstanceCount);
    delete[] cylinderTopScales;
    delete[] cylinderBottomScales;
    delete[] cylinderModelMatrices;
}

InstancedCellRenderer::~InstancedCellRenderer() {

}

void InstancedCellRenderer::RenderCell(CylinderShader* cylinderShader, glm::mat4 modelMatrix) {

    cylinderShader->SetTopScale(1.0);
    cylinderShader->SetBottomScale(1.0);
    cylinderShader->SetModel(modelMatrix);
    cylinderShader->EnableShader();
    spheres.RenderModel();
    cylinders.RenderModel();
    cylinderShader->DisableShader();
    
}

ModelRenderer& InstancedCellRenderer::getSphere() {
    static ModelRenderer sphere = ModelRenderer("sphere.obj");
    return sphere;
}

ModelRenderer& InstancedCellRenderer::getCylinder() {
    static ModelRenderer cylinder = ModelRenderer("cylinder.obj");
    return cylinder;
}
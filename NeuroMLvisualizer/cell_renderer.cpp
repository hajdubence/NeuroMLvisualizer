#include "cell_renderer.h"
#include "pugixml/pugixml.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <map>



CellRenderer::CellRenderer() {

}

CellRenderer::CellRenderer(Cell cell) {

    cellData = cell;

}

CellRenderer::~CellRenderer() {

}



void CellRenderer::RenderCell(CylinderShader* cylinderShader, glm::mat4 modelMatrix) {

    for (int i = 0; i < cellData.sphereVector.size(); i++)
    {
        glm::vec4 center = cellData.sphereVector[i];
        glm::mat4 model = glm::mat4(1.0f);

        // 3. apply modelMatrix
        model = model * modelMatrix;

        // 2. translate
        model = glm::translate(model, glm::vec3(center));

        // 1. scale
        model = glm::scale(model, glm::vec3(center.w, center.w, center.w));

        cylinderShader->SetTopScale(1.0);
        cylinderShader->SetBottomScale(1.0);
        cylinderShader->SetModel(model);
        cylinderShader->EnableShader();
        getSphere().RenderModel();
        cylinderShader->DisableShader();
    }

    for (int i = 0; i < cellData.cylinderVector.size(); i += 2)
    {
        // top = proximal  bottom = distal
        glm::vec4 prox = cellData.cylinderVector[i];
        glm::vec4 dist = cellData.cylinderVector[i + 1];
        glm::mat4 model = glm::mat4(1.0f);

        // 4. apply modelMatrix
        model = model * modelMatrix;

        // 3. translate
        glm::vec3 midPoint = (glm::vec3(prox) + glm::vec3(dist)) / 2.0f;
        model = glm::translate(model, midPoint);

        // 2. rotate
        glm::vec3 oldDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 newDirection = glm::normalize(glm::vec3(prox) - glm::vec3(dist));
        glm::vec3 rotAxis = glm::cross(oldDirection, newDirection);
        if (rotAxis == glm::vec3(0, 0, 0)) { // when they are pararell
            rotAxis = glm::vec3(1, 0, 0); // use the X axis
        }
        float cosTheta = glm::dot(oldDirection, newDirection);
        float rotAngle = acos(cosTheta);
        model = glm::rotate(model, rotAngle, rotAxis);

        // 1. scale
        float distance = glm::distance(glm::vec3(prox), glm::vec3(dist));
        model = glm::scale(model, glm::vec3(1.0f, distance, 1.0f));

        cylinderShader->SetTopScale(prox.w);
        cylinderShader->SetBottomScale(dist.w);
        cylinderShader->SetModel(model);
        cylinderShader->EnableShader();
        getCylinder().RenderModel();
        cylinderShader->DisableShader();
    }
}

ModelRenderer& CellRenderer::getSphere() {
    static ModelRenderer sphere = ModelRenderer("sphere.obj");
    return sphere;
}

ModelRenderer& CellRenderer::getCylinder() {
    static ModelRenderer cylinder = ModelRenderer("cylinder.obj");
    return cylinder;
}


InstancedCellRenderer::InstancedCellRenderer(Cell cell) : CellRenderer(cell)
{
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
        glm::mat4 model = glm::mat4(1.0f);

        // 2. translate
        model = glm::translate(model, glm::vec3(center));

        // 1. scale
        model = glm::scale(model, glm::vec3(center.w, center.w, center.w));

        sphereTopScales[i] = 1.0f;
        sphereBottomScales[i] = 1.0f;
        sphereModelMatrices[i] = model;
    }
    sphere.setTopScales(sphereTopScales, sphereInstanceCount);
    sphere.setBottomScales(sphereBottomScales, sphereInstanceCount);
    sphere.setModelMatreces(sphereModelMatrices, sphereInstanceCount);
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
        glm::mat4 model = glm::mat4(1.0f);
        
        // 3. translate
        glm::vec3 midPoint = (glm::vec3(prox) + glm::vec3(dist)) / 2.0f;
        model = glm::translate(model, midPoint);

        // 2. rotate
        glm::vec3 oldDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 newDirection = glm::normalize(glm::vec3(prox) - glm::vec3(dist));
        glm::vec3 rotAxis = glm::cross(oldDirection, newDirection);
        if (rotAxis == glm::vec3(0, 0, 0)) { // when they are pararell
            rotAxis = glm::vec3(1, 0, 0); // use the X axis
        }
        float cosTheta = glm::dot(oldDirection, newDirection);
        float rotAngle = acos(cosTheta);
        model = glm::rotate(model, rotAngle, rotAxis);

        // 1. scale
        float distance = glm::distance(glm::vec3(prox), glm::vec3(dist));
        model = glm::scale(model, glm::vec3(1.0f, distance, 1.0f));

        cylinderTopScales[i] = prox.w;
        cylinderBottomScales[i] = dist.w;
        cylinderModelMatrices[i] = model;
    }
    cylinder.setTopScales(cylinderTopScales, cylinderInstanceCount);
    cylinder.setBottomScales(cylinderBottomScales, cylinderInstanceCount);
    cylinder.setModelMatreces(cylinderModelMatrices, cylinderInstanceCount);
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
    sphere.RenderModel();
    cylinder.RenderModel();
    cylinderShader->DisableShader();
    
}
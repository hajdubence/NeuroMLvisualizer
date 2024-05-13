#include "shader.h"


CylinderShader::CylinderShader(const char* vertex_file_path, const char* fragment_file_path) : Shader{ vertex_file_path, fragment_file_path }
{
    uniform_topScale = GetUniform("uniformTopScale");
    uniform_bottomScale = GetUniform("uniformBottomScale");
    uniform_model = GetUniform("uniformModel");
    uniform_view = GetUniform("view");
    uniform_projection = GetUniform("projection");

    uniform_lightPosition = GetUniform("lightPosition");
    uniform_viewPosition = GetUniform("viewPosition");
    uniform_objectColor = GetUniform("objectColor");
}

CylinderShader::~CylinderShader() {}


void CylinderShader::SetTopScale(float topScale)
{
    EnableShader();
    glUniform1f(uniform_topScale, topScale);
    DisableShader();
}

void CylinderShader::SetBottomScale(float bottomScale)
{
    EnableShader();
    glUniform1f(uniform_bottomScale, bottomScale);
    DisableShader();
}

void CylinderShader::SetModel(glm::mat4 model)
{
    EnableShader();
    glUniformMatrix4fv(uniform_model, 1, GL_FALSE, &model[0][0]);
    DisableShader();
}

void CylinderShader::SetView(glm::mat4 view)
{
    EnableShader();
    glUniformMatrix4fv(uniform_view, 1, GL_FALSE, &view[0][0]);
    DisableShader();
}

void CylinderShader::SetProjection(glm::mat4 projection)
{
    EnableShader();
    glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, &projection[0][0]);
    DisableShader();
}

void CylinderShader::SetLightPosition(glm::vec3 position)
{
    EnableShader();
    glUniform3fv(uniform_lightPosition, 1, &position[0]);
    DisableShader();
}

void CylinderShader::SetViewPosition(glm::vec3 position)
{
    EnableShader();
    glUniform3fv(uniform_viewPosition, 1, &position[0]);
    DisableShader();
}

void CylinderShader::SetObjectColor(glm::vec3 color)
{
    EnableShader();
    glUniform3fv(uniform_objectColor, 1, &color[0]);
    DisableShader();
}
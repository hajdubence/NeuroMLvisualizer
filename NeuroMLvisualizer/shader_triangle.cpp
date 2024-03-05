#include "shader.h"


TriangleShader::TriangleShader() : Shader{ "triangle.vs", "triangle.fs" }
{
    uniform_MVP = GetUniform("MVP");
    uniform_color = GetUniform("color");
}

TriangleShader::~TriangleShader() {}


void TriangleShader::SetMVP(glm::mat4 MVP)
{
    EnableShader();
    glUniformMatrix4fv(uniform_MVP, 1, GL_FALSE, &MVP[0][0]);
    DisableShader();
}

void TriangleShader::SetColor(glm::vec3 color)
{
    EnableShader();
    glUniform3fv(uniform_color, 1, &color[0]);
    DisableShader();
}
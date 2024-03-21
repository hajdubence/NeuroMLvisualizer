#ifndef SHADER_H
#define SHADER_H


#include <GL/glew.h>
#include <glm/glm.hpp>



class Shader
{
public:

    Shader(const char* vertex_file_path, const char* fragment_file_path);
    ~Shader();
    void EnableShader();
    void DisableShader();

protected:

    GLuint GetUniform(const char* uniformName);

private:
    GLuint shaderProgram;
    GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
};



class TriangleShader : public Shader
{
public:

    TriangleShader();
    ~TriangleShader();

    void SetMVP(glm::mat4 MVP);
    void SetColor(glm::vec3 color);

private:

    GLuint uniform_MVP;
    GLuint uniform_color;
};

class CylinderShader : public Shader
{
public:

    CylinderShader(const char* vertex_file_path, const char* fragment_file_path);
    ~CylinderShader();

    void SetTopScale(float topScale);
    void SetBottomScale(float bottomScale);
    void SetModel(glm::mat4 model);
    void SetView(glm::mat4 view);
    void SetProjection(glm::mat4 projection);

    void SetLightPosition(glm::vec3 color);
    void SetViewPos(glm::vec3 color);
    void SetObjectColor(glm::vec3 color);

private:

    GLuint uniform_topScale;
    GLuint uniform_bottomScale;
    GLuint uniform_model;
    GLuint uniform_view;
    GLuint uniform_projection;

    GLuint uniform_lightPosition;
    GLuint uniform_viewPos;
    GLuint uniform_objectColor;
};


#endif
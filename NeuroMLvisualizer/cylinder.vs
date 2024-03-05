#version 410

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

out vec3 fPosition;
out vec3 fNormal;

uniform float topScale;
uniform float bottomScale;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    if (vPosition.y > 0.0)
        fPosition = vec3(model * vec4(vPosition.x * topScale, vPosition.y, vPosition.z * topScale, 1.0));
    else
        fPosition = vec3(model * vec4(vPosition.x * bottomScale, vPosition.y, vPosition.z * bottomScale, 1.0));
    gl_Position = projection * view * vec4(fPosition, 1.0);
    
    fNormal = vNormal;
    if (vNormal.y == 0.0)
        fNormal.y = (bottomScale - topScale) / 2;
    fNormal = mat3(transpose(inverse(model))) * fNormal;
}
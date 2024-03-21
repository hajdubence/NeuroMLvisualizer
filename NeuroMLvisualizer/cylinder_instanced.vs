#version 410

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in float instanceTopScale;
layout (location = 3) in float instanceBottomScale;
layout (location = 4) in mat4 instanceMatrix;

out vec3 fPosition;
out vec3 fNormal;

uniform float topScale;
uniform float bottomScale;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	float topS = topScale * instanceTopScale;
	float bottomS = bottomScale * instanceBottomScale;
	mat4 modelMatrix = model * instanceMatrix;
    if (vPosition.y > 0.0)
        fPosition = vec3(modelMatrix * vec4(vPosition.x * topS, vPosition.y, vPosition.z * topS, 1.0));
    else
        fPosition = vec3(modelMatrix * vec4(vPosition.x * bottomS, vPosition.y, vPosition.z * bottomS, 1.0));
    gl_Position = projection * view * vec4(fPosition, 1.0);
    
    fNormal = vNormal;
    if (vNormal.y == 0.0)
        fNormal.y = (bottomS - topS) / 2;
    fNormal = mat3(transpose(inverse(modelMatrix))) * fNormal;
}
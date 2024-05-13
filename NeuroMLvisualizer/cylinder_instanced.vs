#version 410

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in float instanceTopScale;
layout (location = 3) in float instanceBottomScale;
layout (location = 4) in mat4 instanceModel;

out vec3 fPosition;
out vec3 fNormal;

uniform float uniformTopScale;
uniform float uniformBottomScale;
uniform mat4 uniformModel;
uniform mat4 view;
uniform mat4 projection;

void main() {
	float topScale = uniformTopScale * instanceTopScale;
	float bottomScale = uniformBottomScale * instanceBottomScale;
	mat4 model = uniformModel * instanceModel;
    if (vPosition.y > 0.0)
        fPosition = vec3(model * vec4(vPosition.x * topScale, vPosition.y, vPosition.z * topScale, 1.0));
    else
        fPosition = vec3(model * vec4(vPosition.x * bottomScale, vPosition.y, vPosition.z * bottomScale, 1.0));
    gl_Position = projection * view * vec4(fPosition, 1.0);
    
    fNormal = vNormal;
    if (fNormal.y == 0.0) {
        fNormal.y = (bottomScale - topScale) / 2.0;
        fNormal = normalize(fNormal);
    }
    fNormal = mat3(transpose(inverse(model))) * fNormal;
}
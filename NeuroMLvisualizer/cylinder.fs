#version 410

out vec4 FragColor;

in vec3 fPosition;
in vec3 fNormal;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 objectColor;

void main() {
    vec3 lightAmbient = vec3(1.0, 1.0, 1.0) * 0.1;
    vec3 lightDiffuse = vec3(1.0, 1.0, 1.0) * 0.6;
    vec3 lightSpecular = vec3(1.0, 1.0, 1.0) * 0.6;
    
    vec3 ambient = lightAmbient * objectColor;

    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(lightPosition - fPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * objectColor;

    vec3 viewDir = normalize(viewPosition - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    if (diff == 0.0)
        spec = 0.0;
    vec3 specular = lightSpecular * spec;

    vec3 result = (ambient + diffuse + specular);
    
    FragColor = vec4(result, 1.0);
}
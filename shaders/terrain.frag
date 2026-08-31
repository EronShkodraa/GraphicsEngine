#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 viewPos;

vec3 getTerrainColor(float height) {
    vec3 snowColor = vec3(1.0, 1.0, 1.0);
    vec3 rockColor = vec3(0.5, 0.5, 0.5);
    vec3 grassColor = vec3(0.2, 0.6, 0.1);
    vec3 sandColor = vec3(0.76, 0.7, 0.5);
    
    if (height > 12.0f) {
        float t = smoothstep(12.0f, 18.0f, height);
        return mix(rockColor, snowColor, t);
    }
    else if (height > 8.0f) {
        float t = smoothstep(8.0f, 12.0f, height);
        return mix(grassColor, rockColor, t);
    }
    else if (height > 1.0f) {
        float t = smoothstep(1.0f, 3.0f, height);
        return mix(sandColor, grassColor, t);
    }
    else {
        return sandColor;
    }
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 light = normalize(lightDir);
    
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    
    float diff = max(dot(norm, light), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    
    float specularStrength = 0.3;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);
    
    vec3 terrainColor = getTerrainColor(FragPos.y);
    vec3 result = (ambient + diffuse) * terrainColor + specular;
    
    float distance = length(viewPos - FragPos);
    float fogFactor = 1.0 - exp(-0.001 * distance * distance);
    vec3 fogColor = vec3(0.53, 0.81, 0.92);
    result = mix(result, fogColor, fogFactor);
    
    FragColor = vec4(result, 1.0);
}
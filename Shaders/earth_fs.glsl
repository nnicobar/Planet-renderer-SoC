#version 330 core
out vec4 result;
in vec3 FragPos;
in vec3 normal;
in vec2 texCoord;

struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform mat4 normalMatrix;
uniform vec3 viewLoc;

void main(){
    vec3 ambient = light.ambient * texture(material.ambient, texCoord).rgb;
    
    vec3 normal_n = mat3(normalMatrix) * normalize(normal);
    vec3 lightDir;
    
    lightDir = -light.direction;
    
    
    vec3 reflectDir = reflect(-lightDir, normal_n);
    vec3 viewDir = normalize(viewLoc - FragPos);
    vec3 halfway = normalize(lightDir + viewDir);
    
    float spec = pow(max(dot(normal_n, halfway), 0.0), material.shininess);
    
    vec3 specular =  spec * texture(material.specular, texCoord).r * light.specular;
    float diffuseStrength = max(dot(lightDir, normal_n), 0.0);
    vec3 diffuse = diffuseStrength * texture(material.diffuse, texCoord).rgb * light.diffuse;
    
    
    vec3 Color = specular + diffuse + ambient;
    result = vec4(Color, 1.0);
}


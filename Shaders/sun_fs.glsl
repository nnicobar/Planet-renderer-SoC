#version 330 core
out vec4 result;

uniform vec3 light;
void main(){
    result = vec4(light, 1.0);
}


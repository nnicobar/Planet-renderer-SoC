#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utility.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Shader
{

GLuint Program;

public:
//Constructor
Shader(const char *vertPath, const char *fragPath)
{
// 1. Retrieve the vertex/fragment source code from filePath
std::string vert = readFile(vertPath);
std::string frag = readFile(fragPath);

// 2. Compile shaders
int success;
char errorLog[512];

const char* vertShader = vert.c_str();
const char* fragShader = frag.c_str();



//vertex shader
GLuint vshad;
vshad = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vshad, 1, &vertShader, NULL);
glCompileShader(vshad);
//Print compile errors if any
glGetShaderiv(vshad, GL_COMPILE_STATUS, &success);
if (!success)
{
glGetShaderInfoLog(vshad, 512, NULL, errorLog);
std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << errorLog << std::endl;
}


//Fragment shader
GLuint fshad;
fshad = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fshad, 1, &fragShader, NULL);
glCompileShader(fshad);
//Print compile errors if any
glGetShaderiv(fshad, GL_COMPILE_STATUS, &success);
if (!success)
{
glGetShaderInfoLog(fshad, 512, NULL, errorLog);
std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << errorLog << std::endl;
}

//3.Shader Program
this->Program = glCreateProgram();
glAttachShader(this->Program, fshad);
glAttachShader(this->Program, vshad);
glLinkProgram(this->Program);

//Print linking errors if any
glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
if (!success)
{
glGetProgramInfoLog(this->Program, 512, NULL, errorLog);
std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << errorLog << std::endl;
}

// Delete the shaders as they're linked into our program now and no longer necessary
glDeleteShader(vshad);
glDeleteShader(fshad);
}

void use(){
glUseProgram(this->Program);
}

void setFloat(const char* uniformID, float value){
glUniform1f(glGetUniformLocation(this->Program, uniformID), value);
}
void setInt(const char* uniformID, int value){
glUniform1i(glGetUniformLocation(this->Program, uniformID), value);
}

void setVec3f(const char* uniformID, glm::vec3 vector){
glUniform3fv(glGetUniformLocation(this->Program, uniformID), 1, glm::value_ptr(vector));
}

void setVec4f(const char* uniformID, glm::vec4 vector){
glUniform4fv(glGetUniformLocation(this->Program, uniformID), 1, glm::value_ptr(vector));
}
void setMatrix4f(const char* uniformID, glm::mat4 matrix){
glUniformMatrix4fv(glGetUniformLocation(this->Program, uniformID), 1, GL_FALSE, glm::value_ptr(matrix));
}

};


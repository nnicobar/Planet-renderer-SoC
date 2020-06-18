#include <iostream>
#include <vector>
#include <string>
#include <cmath>

//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

// GLM Mathemtics
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

//STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "Headers/stb_image.h"

//Header includes
#include "Headers/Shader.h"
#include "Headers/Camera.h"
#include "Headers/Screenfiller.h"
#include "Headers/Cube.h"
#include "Headers/sphere.hpp"
#include "Headers/Skybox.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;
int globalTextureCount = 0;

//Time properties
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame

// Initializing the camera
Camera camera(glm::vec3(0.0f, 0.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f));

//Decay rate for time counter of camera
float decay_rate(glm::vec3 current)
{
    float decay_control = 0.01f, offset = 0.2f, length = glm::length(current);
    return (1 - exp(-(decay_control*length+offset)));
}

//Function prototypes
void reshape_viewport(GLFWwindow *window, int width, int height);
void cursor_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* window, double x, double y);
void processInput(GLFWwindow *window);
GLuint load_texture(char const* path);

//For skybox
GLuint load_cube_texture(std::vector< std::string > paths, std::string base);
std::vector<std::string> skybox = {"right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png"};
std::string skybox_base_path = "Textures/Skybox/";



int main(){
    
    // Init GLFW
    glfwInit( );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "MyEarth", nullptr, nullptr ); // Windowed
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent( window );
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    // Options, removes the mouse cursor for a more immersive experience
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    // Setup some OpenGL options
    glEnable( GL_DEPTH_TEST );
    // enable alpha support
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetFramebufferSizeCallback(window, reshape_viewport);
    glfwSetScrollCallback(window, scroll_callback);

    //Loading skybox texture
    GLuint skyTexture = load_cube_texture(skybox, skybox_base_path);
    
    //Setting lighting maps for the earth
    GLuint ambient = load_texture("Textures/night.jpg");
    GLuint diffuse = load_texture("Textures/day.jpg");
    GLuint specular = load_texture("Textures/spec.jpg");
    
    //Shaders
    Shader *SkyboxShader = new Shader("Shaders/skyboxvertex.glsl", "Shaders/skyboxfragment.glsl");
    Shader *SunShader = new Shader("Shaders/sphere_vs.glsl", "Shaders/sun_fs.glsl");
    Shader *EarthShader = new Shader("Shaders/sphere_vs.glsl", "Shaders/earth_fs.glsl");
    Shader *Atmosphere = new Shader("Shaders/scatter_vert.glsl", "Shaders/scatter_frag.glsl");
    
    SkyBox skybox; // Skybox
    
    Sphere sph(5.0f); //Earth
    Sphere sourcesph(20.0f); //Sun
    
    //Light Properties
    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.5f);
    glm::vec3 lightPos(0.0f, 0.0f, 0.0f) ;
    glm::vec3 lightDirection(0.0f, 0.0f, 0.0f);
    
    float theta = 0;
    
    ScreenFiller sf;
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_SRC_ALPHA);
    
    
    //Main loop
    while (!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        glm::vec3 current = camera.cameraPos;
        float decay = decay_rate(current);
        camera.updateTimeCounter(decay);
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 800.0f/600.0f, 0.1f, 100.0f);
        
        camera.updateLookAt();
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec4 earth = view*(model*glm::vec4(0.0, 0.0, 0.0, 1.0));
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        processInput(window);
        
        //Moving the sun
        lightPos.x = 100.0f*cos(theta);
        lightPos.z = 100.0f*sin(theta);
        lightDirection = glm::normalize(-lightPos);
        theta+= 0.125*deltaTime;
        
        EarthShader->use();
        EarthShader->setMatrix4f("view", view);
        EarthShader->setMatrix4f("model", model);
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
        EarthShader->setMatrix4f("normalMatrix", normalMatrix);
        EarthShader->setVec3f("viewLoc", camera.cameraPos);
        EarthShader->setMatrix4f("view", view);
        EarthShader->setMatrix4f("projection", projection);
        EarthShader->setVec3f("light.position", lightPos);
        EarthShader->setVec3f("light.direction", lightDirection);
        EarthShader->setFloat("light.constant",  1.0f);
        EarthShader->setFloat("light.linear",    0.00f);
        EarthShader->setFloat("light.quadratic", 0.004f);
        
        EarthShader->setVec3f("light.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        EarthShader->setVec3f("light.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        EarthShader->setVec3f("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        
        EarthShader->setInt("material.ambient", 0);
        EarthShader->setInt("material.diffuse", 1);
        EarthShader->setInt("material.specular", 2);
        EarthShader->setFloat("material.shininess", 10.0f);
        
        glBindTexture(GL_TEXTURE0, ambient);
        glBindTexture(GL_TEXTURE1, diffuse);
        glBindTexture(GL_TEXTURE2, specular);
        glBindVertexArray(sph.VAO);
        glDrawArrays(GL_TRIANGLES, 0, sph.vertex_count);
        
        SunShader->use();
        model = glm::translate(glm::mat4(1.0f), lightPos);
        SunShader->setMatrix4f("model", model);
        SunShader->setMatrix4f("view", view);
        SunShader->setMatrix4f("projection", projection);
        SunShader->setVec3f("light", lightColor);
        glBindVertexArray(sourcesph.VAO);
        
        
        glDrawArrays(GL_TRIANGLES, 0, sourcesph.vertex_count);
        
        glm::mat4 skyMapView = glm::mat4(glm::mat3(view));
        glDepthFunc(GL_LEQUAL);
        SkyboxShader->use();
        SkyboxShader->setMatrix4f("view", skyMapView);
        SkyboxShader->setMatrix4f("projection", projection);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
        glBindVertexArray(skybox.VAO);
        glDrawArrays(GL_TRIANGLES, 0, skybox.vertex_count);
        glDepthFunc(GL_LESS);
        
        
        Atmosphere->use();
        Atmosphere->setFloat("tangent", tan(glm::radians(camera.fov)/2));
        Atmosphere->setVec3f("earth_center", glm::vec3(earth));
        glm::vec3 lDir = glm::mat3(view)*lightDirection;
        Atmosphere->setVec3f("light_direction", lDir);
        Atmosphere->setFloat("earth_radius", 5.0f);
        Atmosphere->setFloat("atm_radius", 5.0f*(1.025f));
        glBindVertexArray(sf.VAO);
        glDrawArrays(GL_TRIANGLES, 0, sf.vertex_count);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void reshape_viewport(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void cursor_callback(GLFWwindow* window, double x, double y){
    float delta_x = x - camera.globalX;
    float delta_y = camera.globalY - y;
    camera.globalX = x;
    camera.globalY = y;
    camera.modify_pitch_yaw(delta_x, delta_y);
}


void scroll_callback(GLFWwindow* w, double x, double y){
    camera.modify_fov(y);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.KeyboardInput(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.KeyboardInput(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.KeyboardInput(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.KeyboardInput(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.KeyboardInput(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        camera.KeyboardInput(UP, deltaTime);
}

GLuint load_texture(char const * path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    int width , height , nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;
        
        glActiveTexture(GL_TEXTURE0 + globalTextureCount);
        globalTextureCount++;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

GLuint load_cube_texture(std::vector< std::string > paths, std::string base=""){
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    int w, h, nrChannels;
    for (int i=0; i<paths.size(); ++i){
        unsigned char* data = stbi_load((base+paths[i]).c_str(), &w, &h, &nrChannels, 0);
        if (data){
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cerr << "FAILED TO LOAD CUBE TEXTURE AT LOCATION " << paths[i] << std::endl;
        }
        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return texture;
}


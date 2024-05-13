#include <GL/glew.h>    
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include <vector>
#include "model_renderer.h"
#include <map>
#include "glm/gtx/color_space.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "instanced_cell_renderer.h"
#include "enviroment.h"

#define MAX_VALUE 0.01f
#define MIM_VALUE -0.07f
#define FRAME_SPEED 5


static GLFWwindow* setupGLFW() {

    GLFWwindow* window = NULL;
    const GLubyte* renderer;
    const GLubyte* version;

    if (!glfwInit()) {
        throw std::runtime_error("ERROR: could not start GLFW3\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "NeuroML Visualizer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("ERROR: could not open window with GLFW3\n");
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    renderer = glGetString(GL_RENDERER);
    version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    return window;
}

static glm::vec3 valueToColor(float value) {
    float max = MAX_VALUE; // -> 0° (red)
    float min = MIM_VALUE; // -> 240° (blue)
    float m = 240.0f / (min - max);
    float b = -m * max;
    float hue = m * value + b;
    return glm::rgbColor(glm::vec3(hue, 1, 1));
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Run the program by providing the file path as an argument.\n";
        std::cerr << "Some examples are included:\n";
        std::cerr << "examples/Purk2M9s.nml\n";
        std::cerr << "examples/c302_D1_Full/c302_D1_Full.net.nml\n";
        std::cerr << "examples/c302_D1_Full/LEMS_c302_D1_Full.xml\n";
        std::cerr << "examples/c302_C2_FW/LEMS_c302_C2_FW.xml\n";
        std::cerr << "examples/c302_A_Full/LEMS_c302_A_Full.xml\n";
        
        return 1;
    }

    // GLFW

    GLFWwindow* window = setupGLFW();

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // Camera

    Camera* camera = new Camera();
    camera->horizontalAngle = glm::pi<float>(); // Looking in -Z direction.

    // Shader

    CylinderShader* cylinderShader = new CylinderShader("cylinder_instanced.vs", "cylinder.fs");
    
    // Enviroment

    Enviroment enviroment;
    std::string simulationFile = argv[1];
    enviroment.readFile(simulationFile.c_str());

    std::map<std::string, bool> outputFileVisibleMap;
    for (auto it = enviroment.outputFileCulumns.begin(); it != enviroment.outputFileCulumns.end(); it++) {
        std::string outputFilename = it->first;
        outputFileVisibleMap[outputFilename] = false;
    }

    int totalFrames = 0;
    if (enviroment.outputFiles.size()) {
        totalFrames = enviroment.outputFiles.begin()->second.size();
    }

    // Rendering

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        static int frame = 0;
        static bool playing = false;
        if (playing)
        {
            frame = frame + FRAME_SPEED;
        }
        if (frame >= totalFrames) {
            frame = 0;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin(simulationFile.c_str());
        ImGui::Text("Frame: %d/%d", frame, totalFrames - 1);
        ImGui::SliderInt("##",&frame, 0, totalFrames - 1);
        if (playing)
        {
            if (ImGui::Button("Pause"))
            {
                playing = false;
            }
        }
        else
        {
            if (ImGui::Button("Play"))
            {
                playing = true;
            }
        }
        ImGui::Text("Simulation Output Files:");
        for (auto it = outputFileVisibleMap.begin(); it != outputFileVisibleMap.end(); it++) {
            ImGui::Checkbox(it->first.c_str(), &it->second);
        }
        ImGui::End();

        // Calculate matrices
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            camera->OnRender(window);
        }
        glm::mat4 model = glm::mat4(1.0f); // identity matrix
        model = glm::rotate(model, -glm::half_pi<float>() / 2, glm::vec3(0, 1, 0));
        model = glm::rotate(model, -glm::half_pi<float>(), glm::vec3(1, 0, 0));
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = camera->GetProjectionMatrix();

        cylinderShader->SetView(view);
        cylinderShader->SetProjection(projection);
        cylinderShader->SetLightPosition(camera->cameraPosition);
        cylinderShader->SetObjectColor(glm::vec3(1.0, 1.0, 0.0));
        cylinderShader->SetViewPosition(camera->cameraPosition);

        // Clear frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render cells

        for (NetworkCell neuron : enviroment.networkCells) {
            cylinderShader->SetObjectColor(glm::vec3(0.5, 0.5, 0.5));
            for (auto it = outputFileVisibleMap.begin(); it != outputFileVisibleMap.end(); it++) {
                std::map<std::string, int>& outputFileColumnMap = enviroment.outputFileCulumns[it->first];
                if (it->second && outputFileColumnMap.find(neuron.id) != outputFileColumnMap.end()) {
                    float value = enviroment.outputFiles[it->first][frame][outputFileColumnMap[neuron.id]];
                    cylinderShader->SetObjectColor(valueToColor(value));
                }
            }
            enviroment.cellRenderers[neuron.component]
                ->RenderCell(cylinderShader, glm::translate(model, neuron.position));
        }

        if (enviroment.networkCells.size() == 0)
        {
            cylinderShader->SetObjectColor(glm::vec3(0.5, 0.5, 0.5));
            for (auto it = enviroment.cellRenderers.begin(); it != enviroment.cellRenderers.end(); it++) {
                it->second->RenderCell(cylinderShader, glm::mat4(1.0f));
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

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
#include "cell_renderer.h"
#include "enviroment.h"


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
    float max = 0.01f; // -> 0� (red)
    float min = -0.07f; // -> 240� (blue)
    float m = 240.0f / (min - max);
    float b = -m * max;
    float hue = m * value + b;
    return glm::rgbColor(glm::vec3(hue, 1, 1));
}

int main(int argc, char* argv[]) {

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

    CylinderShader* cylinderShader = new CylinderShader();

    // Enviroment

    Enviroment enviroment;
    std::string simulationFile = "examples/c302_A_Full/LEMS_c302_A_Full.xml";
    //std::string simulationFile = "examples/c302_C2_FW/LEMS_c302_C2_FW.xml";
    //std::string simulationFile = "examples/c302_D1_Full/LEMS_c302_D1_Full.xml";
    enviroment.readFile(simulationFile.c_str());

    std::map<std::string, bool> datFileVisibleMap;
    for (auto it = enviroment.outputFileCulumns.begin(); it != enviroment.outputFileCulumns.end(); ++it) {
        std::string datFilename = it->first;
        datFileVisibleMap[datFilename] = false;
    }

    int totalFrames = enviroment.outputFiles.begin()->second.size();
    
    // Add 2 predefinied cells

    glm::vec4 point3DWithDiam_1 = glm::vec4(0, 0, 0, 5);
    Cell generic_neuron_iaf_cell = Cell();
    generic_neuron_iaf_cell.AddSegment(point3DWithDiam_1, point3DWithDiam_1);
    generic_neuron_iaf_cell.id = "generic_neuron_iaf_cell";
    enviroment.cellRenderers["generic_neuron_iaf_cell"] = new CellRenderer(generic_neuron_iaf_cell);

    Cell generic_muscle_iaf_cell = Cell();
    generic_muscle_iaf_cell.AddSegment(point3DWithDiam_1, point3DWithDiam_1);
    generic_muscle_iaf_cell.id = "generic_muscle_iaf_cell";
    enviroment.cellRenderers["generic_muscle_iaf_cell"] = new CellRenderer(generic_muscle_iaf_cell);

    // Rendering

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        static int frame = -1;
        static bool playing = true;
        if (playing)
        {
            frame = frame + 5;
        }
        if (frame >= totalFrames) {
            frame = 0;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

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
        int i = 0;
        for (auto it = datFileVisibleMap.begin(); it != datFileVisibleMap.end(); ++it) {
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
        cylinderShader->SetViewPos(camera->cameraPosition);

        // Clear frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render cells

        for (NetworkCell neuron : enviroment.networkCells) {
            cylinderShader->SetObjectColor(glm::vec3(0.5, 0.5, 0.5));
            for (auto it = datFileVisibleMap.begin(); it != datFileVisibleMap.end(); ++it) {
                std::map<std::string, int>& outputFileColumnMap = enviroment.outputFileCulumns[it->first];
                if (it->second && outputFileColumnMap.find(neuron.id) != outputFileColumnMap.end()) {
                    float value = enviroment.outputFiles[it->first][frame][outputFileColumnMap[neuron.id]];
                    cylinderShader->SetObjectColor(valueToColor(value));
                }
            }
            enviroment.cellRenderers[neuron.component]->RenderCell(cylinderShader, glm::translate(model, neuron.position));
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 backpackPosition = glm::vec3(0.0f);
    glm::vec3 tablePosition = glm::vec3(-1.0f, -3.9f, 3.0f);
    glm::vec3 hollowknightPosition = glm::vec3(0.0f, 0.3f, 13.0f);
    glm::vec3 paintbrushPosition = glm::vec3(5.0f, -0.85f, -1.0f);
    glm::vec3 hornetPosition = glm::vec3(0.0f, -0.15f, 0.0f);
    glm::vec3 statuePosition = glm::vec3(-9.0f, 4.5f, 7.0f);
    glm::vec3 gemPosition = glm::vec3(7.0f, 0.0f, 15.6f);
    glm::vec3 candlePosition = glm::vec3(-9.0f, 0.3f, 22.0f);
    glm::vec3 booksPosition = glm::vec3(6.0f, 2.75f, -23.0f);
    glm::vec3 ghostPosition = glm::vec3(-5.0f, 9.5f, -8.0f);
    glm::vec3 rubikscubePosition = glm::vec3(-9.0f, 0.3f, 31.0f);
    glm::vec3 bushPosition = glm::vec3(10.0f, 1.3f, 19.0f);
    glm::vec3 doorPosition = glm::vec3(54.0f, 76.0f, 5.0f);
    glm::vec3 HKPosition = glm::vec3(-10.0f, -1.2f, -18.0f);
    glm::vec3 notebookPosition = glm::vec3(8.0f, -0.8f, 32.0f);

    float tableScale = 40.0f;
    float paintbrushScale = 1.5f;
    float statueScale = 6.0f;
    float gemScale = 2.0f;
    float candleScale = 1.0f;
    float booksScale = 0.2f;
    float ghostScale = 11.0f;
    float rubikscubeScale = 0.5f;
    float bushScale = 5.0f;
    float doorScale = 20.0f;
    float HKScale = 0.5f;
    float notebookScale = 0.05f;




    PointLight pointLight;

    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    //  Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //  Face-Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");

    // load models
    // -----------


    Model hornet("resources/objects/hornet_-_hollow_knight/scene.gltf");
    hornet.SetShaderTextureNamePrefix("material.");

    Model hollowknight("resources/objects/hollowKnight/untitled.obj");
    hollowknight.SetShaderTextureNamePrefix("material.");

    Model table("resources/objects/antique_wooden_desk/scene.gltf");
    table.SetShaderTextureNamePrefix("material.");

    Model paintBrush("resources/objects/cc0_-_paint_brush_3/scene.gltf");
    paintBrush.SetShaderTextureNamePrefix("material.");

    Model statue("resources/objects/hollow_knight_statue_test/scene.gltf");
    statue.SetShaderTextureNamePrefix("material.");

    Model gem("resources/objects/gem_pack/scene.gltf");
    gem.SetShaderTextureNamePrefix("material.");

    Model candle("resources/objects/candle/scene.gltf");
    candle.SetShaderTextureNamePrefix("material.");

    Model books("resources/objects/pile_of_books/scene.gltf");
    books.SetShaderTextureNamePrefix("material.");

    Model ghost("resources/objects/hollow_knight_grimmchild_animation/scene.gltf");
    ghost.SetShaderTextureNamePrefix("material.");

    Model rubiksCube("resources/objects/rubiks_cube/scene.gltf");
    rubiksCube.SetShaderTextureNamePrefix("material.");

    Model bush1("resources/objects/stylized_bush_v1/scene.gltf");
    bush1.SetShaderTextureNamePrefix("material.");

    Model door("resources/objects/wooden_door/scene.gltf");
    door.SetShaderTextureNamePrefix("material.");

    Model HK("resources/objects/hollowKnight2/untitled.obj");
    HK.SetShaderTextureNamePrefix("material.");

    Model notebook("resources/objects/notebook/scene.gltf");
    notebook.SetShaderTextureNamePrefix("material.");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);


    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(4.0f, 4.0, 0.0);
    pointLight.ambient = glm::vec3(0.15, 0.15, 0.15);
    pointLight.diffuse = glm::vec3(0.8, 0.8, 0.8);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);
    glm::vec3 color1 = glm::vec3(1.0f, 0.7f, 0.0f);
    glm::vec3 color2 = glm::vec3(0.5f, 0.0f, 1.0f);

    pointLight.constant = 0.8f;
    pointLight.linear = 0.01f;
    pointLight.quadratic = 0.001f;




    //------------------skybox--------------------------
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
//--------------------skyboxVAO-------------------------------
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    vector<std::string> faces
            {
                    FileSystem::getPath("resources/Skybox_textures/left.png"),
                    FileSystem::getPath("resources/Skybox_textures/right.png"),
                    FileSystem::getPath("resources/Skybox_textures/bottom.png"),
                    FileSystem::getPath("resources/Skybox_textures/top.png"),
                    FileSystem::getPath("resources/Skybox_textures/back.png"),
                    FileSystem::getPath("resources/Skybox_textures/front.png")
            };

    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // point lights
        ourShader.use();
        pointLight.position = glm::vec3(-9.0f, 2.0f, 22.0f);
        ourShader.setVec3("pointLight[0].position", pointLight.position);
        ourShader.setVec3("pointLight[0].ambient", pointLight.ambient);
        ourShader.setVec3("pointLight[0].diffuse", pointLight.diffuse);
        ourShader.setVec3("pointLight[0].specular", pointLight.specular);
        ourShader.setFloat("pointLight[0].constant", pointLight.constant);
        ourShader.setFloat("pointLight[0].linear", pointLight.linear);
        ourShader.setFloat("pointLight[0].quadratic", pointLight.quadratic);
        ourShader.setVec3("color[0]", color1);
        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        pointLight.position = programState->ghostPosition + glm::vec3(0.0f, cos(currentFrame)*2, 0.0f);
        ourShader.setVec3("pointLight[1].position", pointLight.position);
        ourShader.setVec3("pointLight[1].ambient", pointLight.ambient);
        ourShader.setVec3("pointLight[1].diffuse", pointLight.diffuse);
        ourShader.setVec3("pointLight[1].specular", pointLight.specular);
        ourShader.setFloat("pointLight[1].constant", pointLight.constant);
        ourShader.setFloat("pointLight[1].linear", pointLight.linear);
        ourShader.setFloat("pointLight[1].quadratic", pointLight.quadratic);
        ourShader.setVec3("color[1]", color2);


        //Directional Light
        ourShader.setVec3("dirLight.direction", glm::vec3(0.2f, -0.7f, 0.2f));
        ourShader.setVec3("dirLight.ambient", glm::vec3(0.25f));
        ourShader.setVec3("dirLight.diffuse", glm::vec3(0.35f));
        ourShader.setVec3("dirLight.specular", glm::vec3(0.45f));
        ourShader.setVec3("lightColor", glm::vec3(0.0f, 0.8f, 1.0f));



        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, programState->hornetPosition);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.7f));
        ourShader.setMat4("model", model);
        hornet.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->hollowknightPosition);
        model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.02f));
        ourShader.setMat4("model", model);
        hollowknight.Draw(ourShader);


        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->tablePosition);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->tableScale));
        ourShader.setMat4("model", model);
        table.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->paintbrushPosition);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->paintbrushScale));
        ourShader.setMat4("model", model);
        paintBrush.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->statuePosition);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->statueScale));
        ourShader.setMat4("model", model);
        statue.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->gemPosition);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->gemScale));
        ourShader.setMat4("model", model);
        gem.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->candlePosition);
        //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->candleScale));
        ourShader.setMat4("model", model);
        candle.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->booksPosition);
        model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->booksScale));
        ourShader.setMat4("model", model);
        books.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->ghostPosition + glm::vec3(0.0f, cos(currentFrame)*2, 0.0f));
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->ghostScale));
        ourShader.setMat4("model", model);
        ghost.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->rubikscubePosition);
        model = glm::rotate(model, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->rubikscubeScale));
        ourShader.setMat4("model", model);
        rubiksCube.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->bushPosition);
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->bushScale));
        ourShader.setMat4("model", model);
        bush1.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->doorPosition);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->doorScale));
        ourShader.setMat4("model", model);
        door.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->HKPosition);
        model = glm::scale(model, glm::vec3(programState->HKScale));
        ourShader.setMat4("model", model);
        HK.Draw(ourShader);


        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->notebookPosition);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(programState->notebookScale));
        ourShader.setMat4("model", model);
        notebook.Draw(ourShader);


//------------------------------------------------
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();

        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        glDepthFunc(GL_LESS);



        if (programState->ImGuiEnabled)
            DrawImGui(programState);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        ImGui::DragFloat3("ghost position", (float*)&programState->ghostPosition);
        ImGui::DragFloat3("hornet position", (float*)&programState->hornetPosition);
        ImGui::DragFloat3("np position", (float*)&programState->notebookPosition);
        ImGui::DragFloat("np scale", &programState->notebookScale, 0.05, 0.1, 40.0);


        ImGui::DragFloat("pointLight.constant", &programState->pointLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);
        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

unsigned int loadCubemap(vector<std::string> faces) {
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
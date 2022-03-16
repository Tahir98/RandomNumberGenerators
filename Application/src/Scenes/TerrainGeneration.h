#pragma once
#pragma once

#pragma once
#include "Scene/Scene.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "Input/InputHandler.h"
#include "Log/Log.h"
#include <Math/Math.h>
#include <RandomNumberGenerators.h>
#include <cmath>

#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/Shader.h"
#include "OpenGL/FrameBuffer.h"
#include "Camera/PerspectiveCamera.h"
#include "Skybox/SpherecalSkybox.h"


class TerrainGeneration : public Scene {
private:
    struct Material {
        Vec3 ambient;
        Vec3 diffuse;
        Vec3 specular;
        float shininess = 200;
    };

    struct Light {
        Vec3 direction;
        float ambient = 0.3f;
        float diffuse = 0.5f;
        float specular = 0.7f;
        float intensity = 1;
    };

    struct Vertex {
        Vec3 pos;
        Vec3 normal;
    };

    struct PerlinNoiseLayer {
        PerlinNoiseLayer() {}
        uint32_t seed = 12345;
        float scale = 10;
        float scaleMax = 256;
        int rng = 0;
        int gradientSize = 8;
        int smoothness = 2;
        Vec2* gradientVector = nullptr;
    };

    GLFWwindow* window;
    InputHandler handler;

    //vertices, line indices, triangle indices
    std::vector<Vertex> vertices;
    std::vector<uint32_t> lIndices;
    std::vector<uint32_t> tIndices;

    std::vector<Random*> RNG;

    std::vector<PerlinNoiseLayer> layers;
    std::vector<std::string> layerNames;
    int selected_layer = 0;
   
    //OpenGL objects
    OpenGL::VertexArray va;
    OpenGL::VertexBuffer* vb = nullptr;
    OpenGL::IndexBuffer* lib = nullptr;
    OpenGL::IndexBuffer* tib = nullptr;
    OpenGL::Shader* shader1 = nullptr;
    OpenGL::Shader* shader2 = nullptr;

    OpenGL::FrameBuffer* fb = nullptr;
    Vec2 fbSize = { 0,0 };

    //Camera and properties
    PerspectiveCamera camera;
    Vec2 prev, now;
    float sensivity = 0.025f;
    float cameraSpeed = 50;
    bool isClicked = false;

    uint32_t size = 512;

    Material material;
    Vec3 color = { 1,1,1 };
    Light light;

    bool drawLines = false;
    bool drawTriangles = true;

    bool firstTime = true;
    int chosen = 0;

    SpherecalSkybox* skybox = nullptr;

    int counter = 0;
public:
    TerrainGeneration(std::string name, GLFWwindow* window) : Scene(name), window(window), handler(window),
        camera(70, 9.0f / 16.0f, 0.1f, 2000) {
       
        material.ambient = { 0.3f,0.3f,0.3f };
        material.diffuse = { 1.0f,1.0f,1.0f };
        material.specular = { 0.7f,0.7f,0.7f };

        light.direction = { -1,-1,0 };

        RNG.push_back(new MersenneTwister());
        RNG.push_back(new LinearCongruentialGenerator());
        RNG.push_back(new LaggedFibonacciGenerator);
        RNG.push_back(new AddWithCarry());
        RNG.push_back(new Xorshift());
        RNG.push_back(new LehmerGenerator());
        RNG.push_back(new MultiplyWithCarry());

        layers.push_back(PerlinNoiseLayer());
        layerNames.push_back("Layer " + std::to_string(layers.size()));
        layers[0].gradientSize = 1;
        layers[0].scale = 70;
        layers[0].seed = 7;

        layers.push_back(PerlinNoiseLayer());
        layerNames.push_back("Layer " + std::to_string(layers.size()));
        layers[1].gradientSize = 2;
        layers[1].scale = 33;
        layers[1].seed = 6;

        layers.push_back(PerlinNoiseLayer());
        layerNames.push_back("Layer " + std::to_string(layers.size()));
        layers[2].gradientSize = 3;
        layers[2].scale = 8;
        layers[2].seed = 7;

        layers.push_back(PerlinNoiseLayer());
        layerNames.push_back("Layer " + std::to_string(layers.size()));
        layers[3].gradientSize = 4;
        layers[3].scale = 4;
        layers[3].seed = 7;

        layers.push_back(PerlinNoiseLayer());
        layerNames.push_back("Layer " + std::to_string(layers.size()));
        layers[4].gradientSize = 5;
        layers[4].scale = 2;
        layers[4].seed = 7;

        generate();

        vb = new OpenGL::VertexBuffer((float*)vertices.data(), sizeof(Vertex) * (uint32_t)vertices.size(), GL_STATIC_DRAW);

        va.bind();
        va.addVertexAttribute(vb, 3, GL_FLOAT, false);
        va.addVertexAttribute(vb, 3, GL_FLOAT, false);

        shader1 = new OpenGL::Shader("Shaders/line.shader");
        shader2 = new OpenGL::Shader("Shaders/triangle.shader");

        lib = new OpenGL::IndexBuffer(lIndices.data(), sizeof(unsigned int) * (uint32_t)lIndices.size(), GL_STATIC_DRAW);
        tib = new OpenGL::IndexBuffer(tIndices.data(), sizeof(unsigned int) * (uint32_t)tIndices.size(), GL_STATIC_DRAW);

        fb = new OpenGL::FrameBuffer(800, 600);

        camera.setPosition(0, 120, 0);

        skybox = new SpherecalSkybox("Fonts/studio_garden.jpg");
    }

    ~TerrainGeneration() {
        delete shader1;
        delete shader2;

        delete vb;
        delete lib;
        delete tib;

        delete fb;

        delete skybox;

        for (auto layer : layers)
            delete layer.gradientVector;
    }

    void onCreate() override {
        //APP_LOG_INFO("{0} scene onCreate method called", name.c_str());
    }
    
    void fillGradientVector(PerlinNoiseLayer& layer) {
        delete layer.gradientVector;
        uint32_t gSize = (uint32_t)std::pow(2,layer.gradientSize) + 1;
        layer.gradientVector = new Vec2[gSize * gSize];

        RNG[layer.rng]->setSeed(layer.seed);
        for (uint32_t i = 0; i < gSize; i++) {
            for (uint32_t j = 0; j < gSize; j++) {
                float radian = 2.0f * PI * RNG[layer.rng]->nextFloat();
                layer.gradientVector[i * gSize + j] = { sinf(radian),cosf(radian) };
            }
        }    
    }

    float interpolate(float a0, float a1, float w,int smoothness) {
        if (w < 0) w = 0;
        else if (w > 1.0f) w = 1.0f;

        switch (smoothness) {
        case 0: 
            return (a1 - a0) * w + a0;
            break;
        case 1:
            return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
            break;
        case 2:
            return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
            break;
        default:
            return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
            break;
        }
        
        return 0;
    }

    float dotGridGradient(PerlinNoiseLayer& layer,int ix, int iz, float x, float z) {
        Vec2 gradient = layer.gradientVector[ix * ((uint32_t)std::pow(2,layer.gradientSize)) + iz];
        Vec2 distance = { x - ix , z - iz };
        return Math::dotProduct(gradient, distance);
    }

    float PerlinNoise2D(PerlinNoiseLayer& layer,float x,float z) {
        int x0 = (int)x;
        int x1 = x + 1;
        int z0 = (int)z;
        int z1 = z0 + 1;

        float sx = x - (float)x0;
        float sz = z - (float)z0;

        float n0, n1, n2, n3, ix0, ix1, value;

        n0 = dotGridGradient(layer, x0, z0, x, z);
        n1 = dotGridGradient(layer, x1, z0, x, z);
        ix0 = interpolate(n0, n1, sx, layer.smoothness);

        n2 = dotGridGradient(layer,x0, z1, x, z);
        n3 = dotGridGradient(layer,x1, z1, x, z);
        ix1 = interpolate(n2, n3, sx, layer.smoothness);

        value = interpolate(ix0, ix1, sz, layer.smoothness);

        return value;
    }

    void onUpdate(float delta) override {
        //counter++;
        //if (counter >= 30) {
        //    counter -= 30;
        //    layers[0].seed++;
        //    generate();
        //    vb->subData(0, sizeof(Vertex) * (uint32_t)vertices.size(), (float*)vertices.data());
        //}

        input(delta);
        camera.update(delta);
        drawPanel();
        calculateMaterial();

        drawTerrain();

        ImGui::Begin("Terrain Generation");
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        Vec2 ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        if (ViewportSize.x != fbSize.x || ViewportSize.y != fbSize.y) {
            fb->resize((uint32_t)ViewportSize.x, (uint32_t)ViewportSize.y);
            fbSize = ViewportSize;
            camera.setAspectRatio(ViewportSize.y / viewportPanelSize.x);
        }

        ImGui::Image(reinterpret_cast<void*>(fb->getTextureID()), ImVec2(ViewportSize.x, ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }

    void input(float delta) {
        if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && !isClicked) {
            prev = now = handler.getMousePos();
            isClicked = true;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2)) {
            prev = now;
            now = handler.getMousePos();
        }
        else {
            isClicked = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if (isClicked) {
            camera.rotY((now.x - prev.x) * sensivity * delta * 60.0f);
            camera.rotX(-(now.y - prev.y) * sensivity * delta * 60.0f);
        }

        if (handler.getKeyState(GLFW_KEY_W)) {
            camera.translate2(0, 0, cameraSpeed * delta);
        }
        else if (handler.getKeyState(GLFW_KEY_S)) {
            camera.translate2(0, 0, -cameraSpeed * delta);
        }

        if (handler.getKeyState(GLFW_KEY_A)) {
            camera.translate2(-cameraSpeed * delta, 0, 0);
        }
        else if (handler.getKeyState(GLFW_KEY_D)) {
            camera.translate2(cameraSpeed * delta, 0, 0);
        }

        if (handler.getKeyState(GLFW_KEY_SPACE)) {
            camera.translate(0, cameraSpeed * delta, 0);
        }
        else if (handler.getKeyState(GLFW_KEY_LEFT_SHIFT)) {
            camera.translate(0, -cameraSpeed * delta, 0);
        }   
    }

    void drawTerrain() {
        fb->bind();
        fb->clear();
        fb->clearColor(1, 1, 1, 1);
        glViewport(0, 0, (GLsizei)fbSize.x, (GLsizei)fbSize.y);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glLineWidth(1.2f);
        glEnable(GL_CULL_FACE);

        skybox->draw(camera);

        if (drawTriangles) {
            shader2->bind();
            va.bind();
            tib->bind();

            shader2->setUniform3f("camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

            shader2->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
            shader2->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
            shader2->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
            shader2->setUniform1f("material.shininess", material.shininess);

            shader2->setUniform3f("light.direction", light.direction.x, light.direction.y, light.direction.z);
            shader2->setUniform3f("light.ambient", light.ambient, light.ambient, light.ambient);
            shader2->setUniform3f("light.diffuse", light.diffuse, light.diffuse, light.diffuse);
            shader2->setUniform3f("light.specular", light.specular, light.specular, light.specular);
            shader2->setUniform1f("light.intensity", light.intensity);

            shader2->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
            shader2->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());
            glEnable(GL_BLEND);

            glDrawElements(GL_TRIANGLES, (uint32_t)tIndices.size(), GL_UNSIGNED_INT, 0);
        }

        if (drawLines) {
            shader1->bind();
            va.bind();
            lib->bind();
            glDisable(GL_BLEND);
            shader1->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
            shader1->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());
            shader1->setUniform3f("camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

            glDisable(GL_BLEND);

            glDrawElements(GL_LINES, (uint32_t)lIndices.size(), GL_UNSIGNED_INT, 0);
        }

        fb->unbind();
    }

    void drawPanel() {
        ImGui::Begin("Settings");
        ImGui::Checkbox("Filled", &drawTriangles);
        ImGui::Checkbox("Wireframe", &drawLines);

        ImGui::Separator();
        ImGui::Text("\nLight properties");
        ImGui::SliderFloat3("direction", (float*)&light.direction, -1, 1);
        ImGui::SliderFloat("ambient", (float*)&light.ambient, 0, 1);
        ImGui::SliderFloat("diffuse", (float*)&light.diffuse, 0, 1);
        ImGui::SliderFloat("specular", (float*)&light.specular, 0, 1);
        ImGui::SliderFloat("Intensity", &light.intensity, 0.1f, 10);

        ImGui::Separator();
        ImGui::Text("\nMaterial properties");
        ImGui::ColorEdit3("Terrain Color", (float*)&color);

        ImGui::Separator();
        ImGui::Text("\nPerlin noise properties");
       
        const char* l_name = layerNames.data()->c_str();
        const char* current_name = layerNames[selected_layer].c_str();

        if (ImGui::BeginCombo("Layers", current_name)) {
            for (unsigned int i = 0; i < layerNames.size(); i++) {
                bool isSelected = (bool)!strcmp(current_name,layerNames[i].c_str());
                if (ImGui::Selectable(layerNames[i].c_str(), isSelected)) {
                    current_name = layerNames[i].c_str();
                    selected_layer = i;
                }
            }

            ImGui::EndCombo();
        }

        auto& layer = layers[selected_layer];

        ImGui::SliderFloat("Scale", (float*)&layer.scale, 0, size / (float)std::pow(2.0f,layer.gradientSize) );
       
        const char* g_size[] = {
            "2x2", "4x4",
            "8x8","16x16",
            "32x32", "64x64",
            "128x128","256x256",
            "512x512"
        };
        const char* g_selected = g_size[layer.gradientSize - 1];

        if (ImGui::BeginCombo("Grad Size", g_selected)) {
            for (unsigned int i = 0; i < 9; i++) {
                bool isSelected = (bool)!strcmp(g_selected, g_size[i]);
                if (ImGui::Selectable(g_size[i], isSelected)) {
                    g_selected = g_size[i];
                    layer.gradientSize = i + 1;
                }
            }

            ImGui::EndCombo();
        }

        const char* sLevels[] = {
            "1",
            "2",
            "3"
        };

        const char* combo_preview_value = sLevels[layer.smoothness];  // Pass in the preview value visible before opening the combo (it could be anything)
        static ImGuiComboFlags flags = 0;
        if (ImGui::BeginCombo("Smoothness", combo_preview_value, flags)) {
            for (int n = 0; n < IM_ARRAYSIZE(sLevels); n++) {
                const bool is_selected = (layer.smoothness == n);
                if (ImGui::Selectable(sLevels[n], is_selected)) {
                    layer.smoothness = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        const char* RNGs[] = {
            "Mersenne Twister",
            "Linear Cogruential Generator",
            "Lagged Fibonacci Generator",
            "Add With Carry",
            "Xorshift",
            "Lehmer Generator",
            "Multiply With Carry"
        };

        combo_preview_value = RNGs[layer.rng];
        static ImGuiComboFlags flags2 = 0;

        if (ImGui::BeginCombo("RNG", combo_preview_value, flags2)) {
            for (int n = 0; n < IM_ARRAYSIZE(RNGs); n++) {
                const bool is_selected = (layer.rng == n);
                if (ImGui::Selectable(RNGs[n], is_selected)) {
                    layer.rng = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::InputInt("Seed", (int*)&layer.seed);
        ImGui::Separator();
       
        ImGui::Text("\n");
        if(ImGui::Button("Add Layer")) {
            layers.push_back(PerlinNoiseLayer());
            layerNames.push_back("Layer " + std::to_string(layers.size()));
            layers[layers.size() - 1].scaleMax = size / ((float)std::pow(2, layers[layers.size() - 1].gradientSize - 1));
            selected_layer = layers.size() - 1;
        }

        ImGui::SameLine();
        if (ImGui::Button("Delete Layer") && layers.size() > 0) {
            layers.erase(layers.begin() + selected_layer);
            layerNames.clear();
            if(selected_layer > 0)
                selected_layer--;

            for (unsigned int i = 0; i < layers.size(); i++) {
                layerNames.push_back("Layer " + std::to_string(i + 1));
            }
        }

        if (ImGui::Button("Generate", ImVec2(200, 50))) {
            generate();
            vb->subData(0, sizeof(Vertex) * (uint32_t)vertices.size(), (float*)vertices.data());
        }

        //ImGui::Text("Camera position(x:%.2f,y:%.2f,z:%.2f)",camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        ImGui::End();
    }

    void generate() {
        float cornerX = -(float)size / 2.0f;
        float cornerZ = -(float)size / 2.0f;

        //vertices calculation
        vertices.clear();
        for (unsigned int i = 0; i < size; i++) {
            for (unsigned int j = 0; j < size; j++) {
                Vec3 pos = { cornerX + i, 0, cornerZ + j };
                Vec3 normal = { 0,1,0 };

                vertices.push_back({ pos,normal });
            }
        }

        
        for (unsigned int a = 0; a < layers.size(); a++) {
            fillGradientVector(layers[a]);
            uint32_t gSize = (uint32_t)std::pow(2, layers[a].gradientSize);

            for (unsigned int i = 0; i < size; i++) {
                for (unsigned int j = 0; j < size; j++) {
                    float noise = PerlinNoise2D(layers[a], (float)i * gSize / size, (float)j * gSize / size);
                    vertices[i * size + j].pos.y += noise * layers[a].scale;
                }
            }
        }  

        //normal calculation
        calculateNormals();

        if (firstTime) {
            calculateIndices();
        }

        firstTime = false;
    }

    void calculateNormals() {
        for (unsigned int i = 1; i < size - 1; i++) {
            for (unsigned int j = 1; j < size - 1; j++) {
                Vec3 normal = { 0,0,0 };
                Vec3 pos = vertices[i * size + j].pos;

                Vec3 v1 = pos - vertices[(i - 1) * size + j - 1].pos;
                Vec3 v2 = vertices[(i - 1) * size + j].pos - vertices[(i)*size + j - 1].pos;
                normal = normal + Math::crossProduct(v1, v2);

                v1 = vertices[(i)*size + j + 1].pos - vertices[(i - 1) * size + j].pos;
                v2 = vertices[(i - 1) * size + j + 1].pos - pos;
                normal = normal + Math::crossProduct(v1, v2);

                v1 = vertices[(i + 1) * size + j + 1].pos - pos;
                v2 = vertices[(i)*size + j + 1].pos - vertices[(i + 1) * size + j].pos;
                normal = normal + Math::crossProduct(v1, v2);

                v1 = vertices[(i + 1) * size + j].pos - vertices[(i)*size + j - 1].pos;
                v2 = pos - vertices[(i + 1) * size + j - 1].pos;
                normal = normal + Math::crossProduct(v1, v2);

                vertices[i * size + j].normal = Math::normalize(normal) * -1.0f;
            }

        }
    }

    void calculateIndices() {
        //Lines indices
        for (unsigned int i = 0; i < size - 1; i++) {
            for (unsigned int j = 0; j < size - 1; j++) {
                lIndices.push_back(i * size + j);
                lIndices.push_back(i * size + j + 1);

                lIndices.push_back(i * size + j);
                lIndices.push_back((i + 1) * size + j + 1);

                lIndices.push_back(i * size + j);
                lIndices.push_back((i + 1) * size + j);

                lIndices.push_back(i * size + j + 1);
                lIndices.push_back((i + 1) * size + j + 1);

                lIndices.push_back((i + 1) * size + j);
                lIndices.push_back((i + 1) * size + j + 1);
            }
        }

        //Triangles indices
        for (unsigned int i = 0; i < size - 1; i++) {
            for (unsigned int j = 0; j < size - 1; j++) {
                tIndices.push_back(i * size + j);
                tIndices.push_back(i * size + j + 1);
                tIndices.push_back((i + 1) * size + j + 1);

                tIndices.push_back(i * size + j);
                tIndices.push_back((i + 1) * size + j + 1);
                tIndices.push_back((i + 1) * size + j);
            }
        }
    }

    void calculateMaterial() {
        material.ambient = color * 0.3f;
        material.diffuse = color * 0.5f;
        material.shininess = 200;
    }

    void onSuspend() override {
        //APP_LOG_INFO("{0} scene onSuspend method called", name.c_str());
    }

    void onResume() override {
        //APP_LOG_INFO("{0} scene onResume method called", name.c_str());
    }

    void onDestroy() override {
        //APP_LOG_INFO("{0} scene onDestroy method called", name.c_str());
    }
};






















#pragma once
#include "Scene/Scene.h"
#include "GLFW/glfw3.h"
#include "RandomNumberGenerators.h"
#include <vector>
#include "imgui.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/IndexBuffer.h"
#include "Utility/Properties.h"

class Distribution : public Scene {
private:
	GLFWwindow* window;

	OpenGL::FrameBuffer* fb = nullptr;
	OpenGL::Shader* shader = nullptr;
	OpenGL::VertexArray va;
	OpenGL::VertexBuffer* vb = nullptr;

	OpenGL::VertexArray bva;
	OpenGL::VertexBuffer* bvb = nullptr;
	Vec3 bColor = { 1,1,1 };
	Vec3 pColor = { 1,0,0 };

	Vec2 viewportSize;

	std::vector<Vec3> points;
	int count = 2000;

	std::vector<Random*> RNG;
	uint32_t seed = 1;
	unsigned int chosen = 0;

	bool square = false, tsquare = false;
	bool circle = true, tcircle = true;
public:
	Distribution(std::string name, GLFWwindow* window) : Scene(name), window(window) {
		fb = new OpenGL::FrameBuffer(1000, 1000);
		shader = new OpenGL::Shader("Shaders/dist.shader");
		
		float vertices[] = {
			-1,-1,0, //0
			1,-1,0, //1
			1,1,0, //2

			-1,1,0, //3
			-1,-1,0, //0
			1,1,0, //2
			
		};

		vb = new OpenGL::VertexBuffer(nullptr, sizeof(Vec3) * 1000000, GL_STATIC_DRAW);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		bvb = new OpenGL::VertexBuffer(vertices, sizeof(vertices), GL_STATIC_DRAW);
		bva.addVertexAttribute(bvb, 3, GL_FLOAT, false);

		RNG.push_back(new MersenneTwister());
		RNG.push_back(new LinearCongruentialGenerator());
		RNG.push_back(new LaggedFibonacciGenerator);
		RNG.push_back(new AddWithCarry());
		RNG.push_back(new Xorshift());
		RNG.push_back(new LehmerGenerator());
		RNG.push_back(new MultiplyWithCarry());

		generatePoints();
	}

	~Distribution() {
		delete fb;
		delete shader;
		delete vb;

		for(auto& random :RNG) {
			delete random;
		}


	}


	void onCreate() override {
		
	}

	void onUpdate(float delta) override {
		drawPanel1();
		drawPanel2();
	}

	void drawPanel1() {
		fb->bind();
		fb->clear();
		fb->clearColor(1, 1, 1, 1);
		glViewport(0, 0, 1000, 1000);
		glPointSize(3.0f);

		shader->bind();
		
		bva.bind();
		shader->setUniform1i("i", 0);
		shader->setUniform3f("color", bColor.x, bColor.y, bColor.z);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		shader->setUniform3f("color", 0, 0, 0);
		glLineWidth(5);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		
		shader->setUniform1i("i", 1);
		shader->setUniform3f("color", pColor.x, pColor.y, pColor.z);
		va.bind();

		glDrawArrays(GL_POINTS, 0, points.size());

		fb->unbind();

		ImGui::Begin("Distrubition");

		ImGui::NewLine();
		ImGui::NewLine();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		Vec2 ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		float width;
		float space;
		if (ViewportSize.x < ViewportSize.y) {
			width = ViewportSize.x;
			space = (ViewportSize.y - ViewportSize.x)  / 2.0f;
		}
		else {
			width = ViewportSize.y;
			space = (ViewportSize.x - ViewportSize.y) / 2.0f;
			ImGui::SameLine(0, space);
		}
		ImGui::Image(reinterpret_cast<void*>(fb->getTextureID()), ImVec2 (width, width), ImVec2(0, 1), ImVec2(1, 0));
		
		ImGui::End();
	}

	void drawPanel2() {
		ImGui::Begin("Settings");
		const char* items[] = {
			"Mersenne Twister",
			"Linear Cogruential Generator",
			"Lagged Fibonacci Generator",
			"Add With Carry",
			"Xorshift",
			"Lehmer Generator",
			"Multiply With Carry" };
		static int item_current_idx = 0; // Here we store our selection data as an index.
		const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
		static ImGuiComboFlags flags = 0;
		if (ImGui::BeginCombo("RNG", combo_preview_value, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected)) {
					item_current_idx = n;
					chosen = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImVec2 viewport = ImGui::GetContentRegionAvail();

		ImGui::InputInt("Count", (int*)&count);
		ImGui::InputInt("Seed", (int*)&seed);

		ImGui::ColorEdit3("Background color",(float*)&bColor);
		ImGui::ColorEdit3("Point color", (float*)&pColor);

		ImGui::Text("\n");
		//ImGui::NewLine();

		ImGui::Checkbox("Square",&square);
		ImGui::SameLine();
		ImGui::Checkbox("Circle", &circle);

		if (square != tsquare) {
			tsquare = square;
			circle = tcircle = !square;
		}
		else if (circle != tcircle) {
			tcircle = circle;
			square = tsquare = !circle;
		}

		ImGui::NewLine();
		ImGui::NewLine();

		uint32_t width = Window::properties.width;
		uint32_t height = Window::properties.height;

		ImGui::SameLine(0, (viewport.x - width / 5.0f) / 3.0f);
		if (ImGui::Button("Reseed", ImVec2(width / 10.0f, height/20.0f))) {
			RNG[chosen]->setSeed(seed);
		}

		//ImGui::SameLine(0,(viewport.x * 2.0f - width / 10.0f) / 3.0f);
		ImGui::SameLine(0, (viewport.x - width / 5.0f) / 3.0f);
		if (ImGui::Button("Generate", ImVec2(width / 10.0f, height / 20.0f))) {
			generatePoints();
		}

		ImGui::End();
	}

	void generatePoints() {
		points.clear();
		for (uint32_t i = 0; i < count; i++) {

			Vec3 point;

			if(square) {
				point = { RNG[chosen]->nextFloat(),RNG[chosen]->nextFloat() ,0 };
			}
			else {
				float angle = 2.0f * PI * RNG[chosen]->nextFloat();
				double radius = 1 - RNG[chosen]->nextDouble();
				radius = (1 - radius * radius) * 0.5;
				point = { 0.5f  + std::cosf(angle) * (float)radius, 0.5f + std::sinf(angle) * (float)radius,0 };
			}
			

			points.push_back(point);
		}

		vb->bind();
		vb->subData(0, sizeof(Vec3) * points.size(), (float*)points.data());
	}

	void onSuspend() override {
	}

	void onResume() override {
	}

	void onDestroy() override {
	}

};
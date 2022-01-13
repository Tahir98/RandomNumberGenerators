#pragma once
#include "Scene/Scene.h"
#include "ImGui/ImGuiLayer.h"
#include "RandomNumberGenerators.h"
#include <sstream>

class PerformanceScene : public Scene {
private:
	GLFWwindow* window;

	MersenneTwister mt;
	LinearCongruentialGenerator lcg;
	MiddleSquareMethod msm;
	LaggedFibonacciGenerator lfg;
	AddWithCarry awc;
	Xorshift xorshift;
	LehmerGenerator lehmer;
	MultiplyWithCarry mwc;

	uint32_t duration32[8] = {0};
	uint32_t count32 = 2000000;
	uint32_t highest32 = 0;

	bool excludeMSM = true;

	MersenneTwister64 mt64;
	LaggedFibonacciGenerator64 lfg64;
	LehmerGenerator64 lehmer64;
	Xorshift64 xorshift64;

	uint32_t duration64[4] = { 0 };
	uint32_t count64 = 2000000;
	uint32_t highest64 = 0;
public:
	PerformanceScene(std::string name,GLFWwindow* window) : Scene(name),window(window) {

	}

	~PerformanceScene() {

	}

	void onCreate() override {
		lfg.setMode(MODE::XOR);

		calculate32();
		calculate64();
	}

	

	void onUpdate(float delta) override {
		calculateHighest();

		ImGui::Begin("Properties1");
		ImGui::InputInt("Count",(int*)&count32,1);
		if (ImGui::Button("Calculate")) {
			calculate32();
		}
		ImGui::Checkbox("Exclude Middle Square Method", &excludeMSM);
		ImGui::End();


		ImGui::Begin("Properties2");
		ImGui::InputInt("Count", (int*)&count64, 1);
		if (ImGui::Button("Calculate")) {
			calculate64();
		}
		ImGui::End();


		ImGui::Begin("Result1");
		ImGui::Text("32bit PRNGs");

		char buf[32];
		sprintf(buf, "%dms", (int)(duration32[0]));
		ImGui::ProgressBar((float)duration32[0] / highest32, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Mersenne Twister");

		sprintf(buf, "%dms", (int)(duration32[1]));
		ImGui::ProgressBar((float)duration32[1] / highest32, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Linear Congruential Generator");

		if (!excludeMSM && duration32[2] != 0) {
			sprintf(buf, "%dms", (int)(duration32[2]));
			ImGui::ProgressBar((float)duration32[2] / highest32, ImVec2(0.f, 0.f), buf);
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			ImGui::Text("Middle Square Method");
		}
	
		sprintf(buf, "%dms", (int)(duration32[3]));
		ImGui::ProgressBar((float)duration32[3] / highest32, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Lagged Fibonacci Generator");

		sprintf(buf, "%dms", (int)(duration32[4]));
		ImGui::ProgressBar((float)duration32[4] / highest32, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Add with carry");

		sprintf(buf, "%dms", (int)(duration32[5]));
		ImGui::ProgressBar((float)duration32[5] / highest32, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Xorshift");

		sprintf(buf, "%dms", (int)(duration32[6]));
		ImGui::ProgressBar((float)duration32[6] / highest32, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Lehmer Generator");

		sprintf(buf, "%dms", (int)(duration32[7]));
		ImGui::ProgressBar((float)duration32[7] / highest32, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Multiply With Carry");

		ImGui::End();


		ImGui::Begin("Result2");
		ImGui::Text("64bit PRNGs");

		sprintf(buf, "%dms", (int)(duration64[0]));
		ImGui::ProgressBar((float)duration64[0] / highest64, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Mersenne Twister64");

		sprintf(buf, "%dms", (int)(duration64[1]));
		ImGui::ProgressBar((float)duration64[1] / highest64, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Lagged Fibonacci Generator64");

		sprintf(buf, "%dms", (int)(duration64[2]));
		ImGui::ProgressBar((float)duration64[2] / highest64, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Xorshift64");

		sprintf(buf, "%dms", (int)(duration64[3]));
		ImGui::ProgressBar((float)duration64[3] / highest64, ImVec2(0.f, 0.f), buf);
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Lehmer Generator64");

		ImGui::End();
	}

	void onSuspend() override {
	}

	void onResume() override {
	}

	void onDestroy() override {
	}

	void calculate32() {
		highest32 = 0;
		//32bit Random number generators
		
		for (uint32_t i = 0; i < 8; i++) {
			auto start = std::chrono::system_clock::now();
			
			switch (i)
			{
			case 0:
				for (uint32_t i = 0; i < count32; i++) 
					mt.nextUInt();
				break;
			case 1:
				for (uint32_t i = 0; i < count32; i++)
					lcg.nextUInt();
				break;
			case 2:
				for (uint32_t i = 0; i < count32; i++)
					msm.nextUInt();
				break;
			case 3:
				for (uint32_t i = 0; i < count32; i++)
					lfg.nextUInt();
				break;
			case 4:
				for (uint32_t i = 0; i < count32; i++)
					awc.nextUInt();
				break;
			case 5:
				for (uint32_t i = 0; i < count32; i++)
					xorshift.nextUInt();
				break;
			case 6:
				for (uint32_t i = 0; i < count32; i++)
					lehmer.nextUInt();
				break;
			case 7:
				for (uint32_t i = 0; i < count32; i++)
					mwc.nextUInt();
				break;
			}

			auto end = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			duration32[i] = (uint32_t)elapsed.count();
		}	
	}

	void calculate64() {
		highest64 = 0;

		//64bit random number generators

		for (uint32_t i = 0; i < 4; i++) {
			auto start = std::chrono::system_clock::now();

			switch (i)
			{
			case 0:
				for (uint32_t i = 0; i < count64; i++)
					mt64.nextUInt64();
				break;
			case 1:
				for (uint32_t i = 0; i < count64; i++)
					lfg64.nextUInt64();
				break;
			case 2:
				for (uint32_t i = 0; i < count64; i++)
					xorshift64.nextUInt64();
				break;
			case 3:
				for (uint32_t i = 0; i < count64; i++)
					lehmer64.nextUInt64();
				break;
			}

			auto end = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			duration64[i] = (uint32_t)elapsed.count();
		}
	}

	void calculateHighest() {
		highest32 = 0;
		highest64 = 0;

		for (uint32_t i = 0; i < 8; i++) {
			if (i == 2 && excludeMSM)
				continue;
			if (duration32[i] > highest32)
				highest32 = duration32[i];
		}

		for (uint32_t i = 0; i < 4; i++) {
			if (duration64[i] > highest64)
				highest64 = duration64[i];
		}
	}

};
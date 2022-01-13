#pragma once
#include "Scene/Scene.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "RandomNumberGenerators.h"
#include <vector>

class Cryptography : public Scene {
private:

	GLFWwindow* window;
	std::vector<Random*> RNG;
	char* plainText = nullptr;
	char* cipherText = nullptr;
	uint32_t size = 1024;
	uint32_t seed = 123456;

	int chosen = 0;
public:
	Cryptography(std::string name, GLFWwindow* window) : Scene(name), window(window) {
		plainText = new char[2048]{
			"Two roads diverged in a yellow wood,\n"
			"And sorry I could not travel both\n"
			"And be one traveler, long I stood\n"
			"And looked down one as far as I could\n"
			"To where it bent in the undergrowth;\n"
		};
		cipherText = new char[1024]{""};
	}

	~Cryptography() {
		delete plainText;
		delete cipherText;
	}

	void onCreate() override {
		RNG.push_back(new MersenneTwister());
		RNG.push_back(new LinearCongruentialGenerator());
		RNG.push_back(new LaggedFibonacciGenerator());
		RNG.push_back(new AddWithCarry());
		RNG.push_back(new Xorshift());
		RNG.push_back(new LehmerGenerator());
		RNG.push_back(new MultiplyWithCarry());

		encrypt();
	}

	void onUpdate(float delta) override {
		Panel1();
		Panel2();
		Panel3();
	}

	void Panel1() {
		ImGui::Begin("Panel 1");
		
		const char* prngs[] = {
			"Mersenne Twister",
			"Linear Cogruential Generator",
			"Lagged Fibonacci Generator",
			"Add With Carry",
			"Xorshift",
			"Lehmer Generator",
			"Multiply With Carry" };
		static int item_current_idx = 0; // Here we store our selection data as an index.
		const char* combo_preview_value = prngs[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
		static ImGuiComboFlags flags = 0;
		if (ImGui::BeginCombo("Algorithm", combo_preview_value, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(prngs); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(prngs[n], is_selected)) {
					item_current_idx = n;
					chosen = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::NewLine();
		ImGui::InputInt("Seed", (int*)&seed);

		ImGui::NewLine();
		if (ImGui::Button("Encrypt",ImVec2(70,40))) {
			encrypt();
		}

		ImGui::End();
	}

	void Panel2() {
		ImGui::Begin("Input");
		static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
		ImGui::InputTextMultiline("##source", plainText, size, ImGui::GetContentRegionAvail(),flags);
		ImGui::End();
	}

	void Panel3() {
		ImGui::Begin("Encrypted Output");
		ImGui::TextWrapped("%s",cipherText, size,ImGui::GetContentRegionAvail());
		ImGui::End();
	}

	void encrypt() {
		uint32_t i = 0;
		uint32_t diff = 126 - 32 + 1;
		uint32_t length = strlen(plainText);
		uint32_t addition = 41;

		RNG[chosen]->setSeed(seed);
		while (i < size && i < length) {
			if (plainText[i] == ' ' || plainText[i] == '\n') {
				cipherText[i] = plainText[i];
			}
			else {
				uint32_t random = RNG[chosen]->nextUInt();
				cipherText[i] = plainText[i];
				unsigned int character = (unsigned int)cipherText[i] + random;
				if (character > 126) {
					character = (character + addition) % diff + 32;
					cipherText[i] = character;
				}
			}
			i++;
		}
	}

	void onSuspend() override {
	}

	void onResume() override {
	}

	void onDestroy() override {
	}
};
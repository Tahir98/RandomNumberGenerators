#pragma once

#include "Scene/Scene.h"
#include "ImGui/ImGuiLayer.h"
#include "RandomNumberGenerators.h"
#include <vector>
#include <sstream>

class RNG : public Scene {
private:

	enum DataType {
		UINT = 0, INT = 1, FLOAT = 2, DOUBLE = 3
	};

	GLFWwindow* window;
	std::vector<Random*> rng;

	uint32_t seed = 123456789;
	uint32_t chosen = 0;
	bool newLine = false;
	int count = 1;
	std::vector<std::string> texts;
	int currenCount = 0;
	bool range = false;
	int r[2] = { 0,99 };

	bool appendUInts = false;
	bool isGenClicked = false;

	DataType type = DataType::UINT;
public:
	RNG(std::string name, GLFWwindow* window) : Scene(name), window(window) {
		texts.push_back(std::string());
	}

	~RNG() {
	}

	void onCreate() override {
		rng.push_back(new MersenneTwister(seed));
		rng.push_back(new LinearCongruentialGenerator(seed));
		rng.push_back(new LaggedFibonacciGenerator(seed));
		rng.push_back(new AddWithCarry(seed));
		rng.push_back(new Xorshift(seed));
		rng.push_back(new LehmerGenerator(seed));
		rng.push_back(new MultiplyWithCarry(seed));
	}

	void onUpdate(float delta) override {
		ImGui::Begin("Properties");
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

		const char* DATA_TYPES[] = {
			"unsigned int",
			"int",
			"float",
			"double"
		};
		static int current_type = 0; // Here we store our selection data as an index.
		const char* combo_preview_type = DATA_TYPES[current_type];  // Pass in the preview value visible before opening the combo (it could be anything)
		static ImGuiComboFlags t_flags = 0;
		if (ImGui::BeginCombo("Type", combo_preview_type, t_flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(DATA_TYPES); n++)
			{
				const bool is_selected = (current_type == n);
				if (ImGui::Selectable(DATA_TYPES[n], is_selected)) {
					current_type = n;
					type = (DataType)n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::InputInt("Count", (int*)&count);

		ImGui::InputInt("Seed",(int*)&seed,1);
		if (ImGui::Button("Reseed")) {
			for (auto random : rng)
				random->setSeed(seed);
		}

		ImGui::NewLine();
		ImGui::Checkbox("range",&range);
			
		ImGui::SameLine();
		ImGui::DragInt2("min-max",r);

		ImGui::Checkbox("Append unsigned integers", &appendUInts);

		ImGui::Text("\n");
		if (ImGui::Button("Generate")) {
			isGenClicked = true;
		}
		else  isGenClicked = false;

		ImGui::Text("\n");

		if (ImGui::Button("Clear")) {
			currenCount = 0;
			texts.clear();
			texts.shrink_to_fit();
			texts.push_back(std::string());
		}
		ImGui::Text("\n");



		ImGui::End();

		if (isGenClicked) {
			generateRandomNumbers();
		}

		ImGui::Begin("Output");
		for (auto text : texts) {
			ImGui::TextWrapped("%s", text.c_str());
		}
			
	
		ImGui::End();
	}

	void generateRandomNumbers() {
		std::stringstream ss;

		for (int i = 0; i < (count <= 10000 ? count : 10000); i++) {
			if (type == DataType::UINT) {
				uint32_t random1 = range ? rng[chosen]->nextUInt(r[0], r[1]) : rng[chosen]->nextUInt();
				if (appendUInts && type == DataType::UINT) {
					texts[texts.size() - 1] += std::to_string(random1);
				}
				else {
					texts[texts.size() - 1] += std::to_string(random1) + std::string("\n");
				}
				currenCount++;
				if (currenCount > 1000) {
					currenCount = 0;
					texts.push_back(std::string());
				}
			}
			else if (type == DataType::INT) {
				int random2 = range ? rng[chosen]->nextInt(r[0], r[1]) : rng[chosen]->nextInt();
				texts[texts.size() - 1] += std::to_string(random2) + std::string("\n");

				currenCount++;
				if (currenCount > 1000) {
					currenCount = 0;
					texts.push_back(std::string());
				}
			}
			else if (type == DataType::FLOAT) {
				float random3 = rng[chosen]->nextFloat();
				ss.precision(8);
				ss << random3;
				std::string a;
				ss >> a;
				ss.clear();
				texts[texts.size() - 1] += a + std::string("\n");

				currenCount++;
				if (currenCount > 1000) {
					currenCount = 0;
					texts.push_back(std::string());
				}
			}
			else if (type == DataType::DOUBLE) {
				long double random4 = rng[chosen]->nextDouble();
				ss.precision(17);
				ss << random4;
				std::string a;
				ss >> a;
				ss.clear();
				texts[texts.size() - 1] += a + std::string("\n");

				currenCount++;
				if (currenCount > 1000) {
					currenCount = 0;
					texts.push_back(std::string());
				}
			}	
		}
		
	}

	void onSuspend() override {
	}

	void onResume() override {
	}

	void onDestroy() override {
	}
};
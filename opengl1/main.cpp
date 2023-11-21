#include <iostream>
#include <GL/glew.h>

#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

#include "painter.h"
#include "painter_state.h";

using namespace sf;

int main() {
	sf::RenderWindow window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);

	auto painter = Painter();
	painter.Init();
	if (!ImGui::SFML::Init(window)) return -1;

	sf::Clock deltaClock;		
	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(window, event);

			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized) {
				glViewport(
					event.size.width/2 - 300, 
					event.size.height/2 - 300, 
					600,
					600
				);
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Lab 10");
		
		ImGui::ListBox("Label", &painter.state.figure, PainterState::figureLabels, 3);

		ImGui::RadioButton(label(FShader::Const), &painter.state.fshader, FShader::Const);
		ImGui::RadioButton(label(FShader::Uniform), &painter.state.fshader, FShader::Uniform);
		if (ImGui::RadioButton(label(FShader::Gradient), &painter.state.fshader, FShader::Gradient)) {
			painter.CreateColorVBOs();
		}
		
		if (painter.state.fshader == FShader::Uniform) {
			ImGui::ColorPicker3("",painter.state.color);
		}

		window.clear();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		painter.Draw();
		ImGui::SFML::Render(window);
	    

		window.display();
	}

	ImGui::SFML::Shutdown();
	painter.Release();
	return 0;
}
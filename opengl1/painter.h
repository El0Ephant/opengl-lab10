#pragma once
#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "painter_state.h"

using namespace sf;

class Painter {

	struct MyVertex {
		GLfloat x;
		GLfloat y;
	};


	// ID шейдерной программы
	GLuint Programs[3];

	// ID атрибута
	GLint Attrib_vertex[3];

	// ID Vertex Buffer Object
	GLuint VBO;

	const char* VertexShaderSource = R"(
#version 330 core
in vec2 coord;
void main() {
gl_Position = vec4(coord, 0.0, 1.0);
}
)";

	const char* FragShaderSources[3] = { R"(
#version 330 core
out vec4 color;
void main() {
color = vec4(1, 0, 0, 1);
}
)",

R"(
#version 330 core
out vec4 color;
uniform vec4 uniform_color;
void main() {
color = uniform_color;
}
)",

R"(
#version 330 core
out vec4 color;
void main() {
color = vec4(0, 0, 1, 1);
}
)",


	};


	void ShaderLog(unsigned int shader)
	{
		int infologLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
		if (infologLen > 1)
		{
			int charsWritten = 0;
			std::vector<char> infoLog(infologLen);
			glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
			std::cout << "InfoLog: " << infoLog.data() << std::endl;
		}
	}



	void InitVBO() {
		glGenBuffers(1, &VBO);
		// Вершины нашего треугольника
		MyVertex triangle[3] = {
		{ -1.0f, -1.0f },
		{ 0.0f, 1.0f },
		{ 1.0f, -1.0f }
		};
		// Передаем вершины в буфер
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
		// Проверка ошибок OpenGL, если есть, то вывод в консоль тип ошибки
	}

	void InitShader() {
		// Создаем вершинный шейдер
		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		// Передаем исходный код
		glShaderSource(vShader, 1, &VertexShaderSource, NULL);
		// Компилируем шейдер
		glCompileShader(vShader);
		std::cout << "vertex shader \n";
		// Функция печати лога шейдера
		ShaderLog(vShader); //Пример функции есть в лабораторной


		GLuint fShaders[3];

		for (int i = 0; i < 3; i++) {
			// Создаем фрагментный шейдер
			fShaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
			// Передаем исходный код
			glShaderSource(fShaders[i], 1, &(FragShaderSources[i]), NULL);
			// Компилируем шейдер
			glCompileShader(fShaders[i]);
			std::cout << "fragment shader" << i << std::endl;
			ShaderLog(fShaders[i]); //Пример функции есть в лабораторной
		}

		
		for (int i = 0; i < 3; i++) {
			// Создаем программу и прикрепляем шейдеры к ней
			Programs[i] = glCreateProgram();
			glAttachShader(Programs[i], vShader);
			glAttachShader(Programs[i], fShaders[i]);
			// Линкуем шейдерную программу
			glLinkProgram(Programs[i]);
			// Проверяем статус сборки
			int link_ok;
			glGetProgramiv(Programs[i], GL_LINK_STATUS, &link_ok);
			if (!link_ok) {
				std::cout << "error attach shaders \n";
				return;
			}
			// Вытягиваем ID атрибута из собранной программы
			const char* attr_name = "coord"; //имя в шейдере
			Attrib_vertex[0] = glGetAttribLocation(Programs[i], attr_name);
			if (Attrib_vertex[0] == -1) {
				std::cout << "could not bind attrib " << attr_name << std::endl;
				return;
			}
		}

	}

	// Освобождение шейдеров
	void ReleaseShader() {
		// Передавая ноль, мы отключаем шейдерную программу
		glUseProgram(0);
		// Удаляем шейдерную программу
		for (int i = 0; i < 3; i++) {
			glDeleteProgram(Programs[i]);
		}
	}

	// Освобождение буфера
	void ReleaseVBO() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VBO);
	}

public:
	PainterState state;

	void Draw() {
		GLint currentAttrib;
		glUseProgram(Programs[state.fshader]);
		
		if (state.fshader == FShader::Uniform) {
			GLuint uniformColorLocation = glGetUniformLocation(Programs[1], "uniform_color");
			glUniform4f(uniformColorLocation, state.color[0], state.color[1], state.color[2], 1.0);
		}

		currentAttrib = Attrib_vertex[state.fshader];
		glEnableVertexAttribArray(currentAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Подключаем VBO
		// Указывая pointer 0 при подключенном буфере, мы указываем что данные в VBO
		glVertexAttribPointer(currentAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Отключаем VBO
		glDrawArrays(GL_TRIANGLES, 0, 3); // Передаем данные на видеокарту(рисуем)
		glDisableVertexAttribArray(currentAttrib); // Отключаем массив атрибутов
		glUseProgram(0); // Отключаем шейдерную программу
	}

	void Init() {
		glewInit();
		// Шейдеры
		InitShader();
		// Вершинный буфер
		InitVBO();
	}

	void Release() {
		// Шейдеры
		ReleaseShader();
		// Вершинный буфер
		ReleaseVBO();
	}

};

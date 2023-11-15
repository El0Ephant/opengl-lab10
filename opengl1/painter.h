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


	// ID ��������� ���������
	GLuint Programs[3];

	// ID ��������
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
		// ������� ������ ������������
		MyVertex triangle[3] = {
		{ -1.0f, -1.0f },
		{ 0.0f, 1.0f },
		{ 1.0f, -1.0f }
		};
		// �������� ������� � �����
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
		// �������� ������ OpenGL, ���� ����, �� ����� � ������� ��� ������
	}

	void InitShader() {
		// ������� ��������� ������
		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		// �������� �������� ���
		glShaderSource(vShader, 1, &VertexShaderSource, NULL);
		// ����������� ������
		glCompileShader(vShader);
		std::cout << "vertex shader \n";
		// ������� ������ ���� �������
		ShaderLog(vShader); //������ ������� ���� � ������������


		GLuint fShaders[3];

		for (int i = 0; i < 3; i++) {
			// ������� ����������� ������
			fShaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
			// �������� �������� ���
			glShaderSource(fShaders[i], 1, &(FragShaderSources[i]), NULL);
			// ����������� ������
			glCompileShader(fShaders[i]);
			std::cout << "fragment shader" << i << std::endl;
			ShaderLog(fShaders[i]); //������ ������� ���� � ������������
		}

		
		for (int i = 0; i < 3; i++) {
			// ������� ��������� � ����������� ������� � ���
			Programs[i] = glCreateProgram();
			glAttachShader(Programs[i], vShader);
			glAttachShader(Programs[i], fShaders[i]);
			// ������� ��������� ���������
			glLinkProgram(Programs[i]);
			// ��������� ������ ������
			int link_ok;
			glGetProgramiv(Programs[i], GL_LINK_STATUS, &link_ok);
			if (!link_ok) {
				std::cout << "error attach shaders \n";
				return;
			}
			// ���������� ID �������� �� ��������� ���������
			const char* attr_name = "coord"; //��� � �������
			Attrib_vertex[0] = glGetAttribLocation(Programs[i], attr_name);
			if (Attrib_vertex[0] == -1) {
				std::cout << "could not bind attrib " << attr_name << std::endl;
				return;
			}
		}

	}

	// ������������ ��������
	void ReleaseShader() {
		// ��������� ����, �� ��������� ��������� ���������
		glUseProgram(0);
		// ������� ��������� ���������
		for (int i = 0; i < 3; i++) {
			glDeleteProgram(Programs[i]);
		}
	}

	// ������������ ������
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
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // ���������� VBO
		// �������� pointer 0 ��� ������������ ������, �� ��������� ��� ������ � VBO
		glVertexAttribPointer(currentAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // ��������� VBO
		glDrawArrays(GL_TRIANGLES, 0, 3); // �������� ������ �� ����������(������)
		glDisableVertexAttribArray(currentAttrib); // ��������� ������ ���������
		glUseProgram(0); // ��������� ��������� ���������
	}

	void Init() {
		glewInit();
		// �������
		InitShader();
		// ��������� �����
		InitVBO();
	}

	void Release() {
		// �������
		ReleaseShader();
		// ��������� �����
		ReleaseVBO();
	}

};

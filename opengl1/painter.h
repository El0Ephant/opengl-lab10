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
	GLuint quadVBO, fanVBO, pentagonVBO;

	GLuint quadVAO, fanVAO, pentagonVAO;

	const char* VertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 coord;
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

	float randF(float start, float end) {
		return (float)((rand() % (int)((end - start) * 10)) / 10.0 + start);
	}

	const double PI = 3.14159265358979323846;
	int fanVertexCount;

	void InitVBO() {
		srand(time(0));

		// QUAD
		glGenBuffers(1, &quadVBO);
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);
		MyVertex *quadVert = new MyVertex[4];
		quadVert[0] = { -randF(0.45, 0.75), -randF(0.45, 0.75) };
		quadVert[1] = { -randF(0.45, 0.75), randF(0.45, 0.75) };
		quadVert[2] = { randF(0.45, 0.75), randF(0.45, 0.75) };
		quadVert[3] = { randF(0.45, 0.75), -randF(0.45, 0.75) };
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVert) * 4, quadVert, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// FAN
		glGenBuffers(1, &fanVBO);
		fanVertexCount = rand() % 10 + 8;
		glGenVertexArrays(1, &fanVAO);
		glBindVertexArray(fanVAO);
		MyVertex* fanVert = new MyVertex[fanVertexCount];
		fanVert[0] = { 0.0f, -0.5f };
		int i = 1;
		for (float angle = 0.15; angle <= PI - 0.15 + 1e-4; angle += (PI - 0.3) / (fanVertexCount - 2), i++) {
			fanVert[i] = { (float)(1 * cos(angle)), (float)(1 * sin(angle) - 0.5)};
		}
		glBindBuffer(GL_ARRAY_BUFFER, fanVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fanVert) * (fanVertexCount), fanVert, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// PENTAGON
		glGenBuffers(1, &pentagonVBO);
		glGenVertexArrays(1, &pentagonVAO);
		glBindVertexArray(pentagonVAO);
		MyVertex* pentagonVert = new MyVertex[5];
		for (int i = 0; i < 5; i++) {
			pentagonVert[i] = { (float)(0.7 * cos(PI/2 + i * 2 * PI / 5)), (float)(0.7 * sin(PI / 2 + i * 2 * PI / 5)) };
		}
		glBindBuffer(GL_ARRAY_BUFFER, pentagonVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pentagonVert) * 5, pentagonVert, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
		glDeleteBuffers(1, &quadVBO);
		glDeleteBuffers(1, &fanVBO);
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

		switch (state.figure)
		{
		case Figure::Quad:
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_QUADS, 0, 4);
			glBindVertexArray(0);
			break;
		case Figure::Fan:
			glBindVertexArray(fanVAO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, fanVertexCount);
			glBindVertexArray(0);
			break;
		case Figure::Pentagon:
			glBindVertexArray(pentagonVAO);
			glDrawArrays(GL_POLYGON, 0, 5);
			glBindVertexArray(0);
			break;
		default:
			break;
		}
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

#pragma once
#include <GL/glew.h>

enum Figure : int {
	Quad,
	Fan,
	Pentagon
};

enum FShader {
	Const,
	Uniform,
	Gradient,
};

const char* label(FShader shader);

class PainterState {
public:
	int figure = Figure::Quad;
	int fshader = FShader::Const;
	GLfloat color[3] = {1.0f, 1.0f, 1.0f};

	static const char* figureLabels[3];
	static const char* shadeLabels[3];
};

#include <iostream>
#include "painter_state.h";

const char* PainterState::figureLabels[3] = {"Quadrilateral", "Fan", "Regular pentagon"};
const char* PainterState::shadeLabels[3] = { "Const", "Uniform variable", "Gradient" };

const char* label(FShader fshader) {
	return PainterState::shadeLabels[fshader];
}

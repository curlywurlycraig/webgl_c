// Some from
// https://lazyfoo.net/tutorials/SDL/50_SDL_and_opengl_2/index.php
#include <emscripten.h>
#include <emscripten/html5.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>
#include <stdio.h>
#include "shaders.c"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

GLuint programObject;
SDL_Surface* screen;

GLfloat vVertices[] = {
   	0.0f, 0.5f, 1.0f,
	-0.5f, -0.5f, -1.0f,
	0.5f, -0.5f, 0.0f
};

GLint uniformOriginX, uniformOriginY, uniformZoom;

int init() {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_OPENGL);
		if (screen == NULL) {
            printf("Failed to init SDL: %s", SDL_GetError());
			return 0;
		}
	} else {
        printf("Failed to init SDL: %s", SDL_GetError());
		return 0;
	}

	const char vertexShaderSource[] =
		"attribute vec4 vPosition;		                     \n"
		"uniform float originX, originY;                     \n"
		"uniform float zoom;                                 \n"
		"varying vec3 color;                                 \n"
		"void main()                                         \n"
		"{                                                   \n"
		"   gl_Position = vPosition;                         \n"
		"   gl_Position.x = (originX + gl_Position.x) * zoom;\n"
		"   gl_Position.y = (originY + gl_Position.y) * zoom;\n"
		"   color = gl_Position.xyz + vec3(0.5);             \n"
		"}                                                   \n";

	const char fragmentShaderSource[] =
		"precision mediump float;                     \n"
		"varying vec3 color;                          \n"
		"void main()                                  \n"
		"{                                            \n"
		"  gl_FragColor = vec4 ( color, 1.0 );        \n"
		"}                                            \n";

	// load vertex and fragment shaders
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	programObject = buildProgram(vertexShader, fragmentShader, "vPosition");

	// save location of uniform variables
	uniformOriginX = glGetUniformLocation(programObject, "originX");
	uniformOriginY = glGetUniformLocation(programObject, "originY");
	uniformZoom = glGetUniformLocation(programObject, "zoom");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_GL_SwapBuffers();
	return 1;
}

EM_BOOL iter(double time, void* userData) {
    // fill the screen with the clear color
	glClear(GL_COLOR_BUFFER_BIT);

	// enable our shader program
	glUseProgram(programObject);
    glUniform1f(uniformOriginX, 0);
	glUniform1f(uniformOriginY, 0);
	glUniform1f(uniformZoom, 1);

	// set up the vertices array
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
	glEnableVertexAttribArray(0);
	// draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// swap buffer to make whatever we've drawn to backbuffer appear on the screen
	SDL_GL_SwapBuffers();

    return EM_TRUE;
}

int main() {
    init();

    emscripten_request_animation_frame_loop(iter, 0);

    return 0;
}

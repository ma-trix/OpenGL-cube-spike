#pragma once

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <forward_list>
#include <chrono>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec2 position;"
"in vec3 color;"
"in vec2 texcoord;"
"out vec3 Color;"
"out vec2 Texcoord;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 proj;"
"void main() {"
"	Color = color;"
"	Texcoord = texcoord;"
"	gl_Position = proj * view * model * vec4(position, 0.0, 1.0);"
"}";
const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 Color;"
"in vec2 Texcoord;"
"out vec4 outColor;"
"uniform sampler2D texKitten;"
"uniform sampler2D texPuppy;"
"void main() {"
"	outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);"
"}";

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	glewExperimental = GL_TRUE;
	glewInit();
	SDL_Event windowEvent;

	// Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	glGenBuffers(1, &vbo);
	
	GLfloat vertices[] = {
		//  Position   Color             Texcoords
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an Element Arrya
	GLuint ebo;
	glGenBuffers(1, &ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	
	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	
	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	GLint colorAttrib = glGetAttribLocation(shaderProgram, "color");
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(colorAttrib);
	
	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(5 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);

	// Load textures	
	GLuint textures[2];
	int width, height;
	unsigned char*  image;
	glGenTextures(2, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	image = SOIL_load_image("Textures/sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	
	image = SOIL_load_image("Textures/sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::mat4 trans;
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 result = trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	GLuint uniTrans = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

	glm::mat4 view = glm::lookAt(
		glm::vec3(1.2f, 1.2f, 1.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	auto t_start = std::chrono::high_resolution_clock::now();
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) 
				break;
			if (windowEvent.type == SDL_KEYUP &&
				windowEvent.key.keysym.sym == SDLK_ESCAPE) 
				break;
		}
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Calculate transformation
		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		glm::mat4 trans;
		trans = glm::rotate(
			trans,
			time * glm::radians(180.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
			);
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

		SDL_GL_SwapWindow(window);
	}
	glDeleteTextures(2, textures);

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}
/*
ZJ Wood CPE 471 Lab 3 base code - I. Dunn class re-write
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "WindowManager.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Buffer {
public:
	static float toRadians(int x) {
		const float PI = 3.14159f;
		//world range 0 - 80
		//radian range 0 - 2PI

		return ((x * 2 * PI) / 80);
	}

	static void buildVertexBuffer(GLfloat vertexBuffer[]) {
		float offSet = 0.25;
		float position;
		vertexBuffer[0] = vertexBuffer[1] = vertexBuffer[2] = 0;

		for (int i = 3; i < 243; i += 3) {
			position = toRadians(int(i / 3));
			if (i % 2 == 0) { offSet = .75; }

			else { offSet = 0.25; }

			vertexBuffer[i] = cos(position)*offSet;
			vertexBuffer[i + 1] = sin(position)*offSet;
			vertexBuffer[i + 2] = 0;
			//printf("%f \n", vertexBuffer[i]);
			//printf("%f \n", vertexBuffer[i+1]);
		}
	}

	static void buildIndexBuffer(GLuint indexBuffer[]) {
		int prevIndex = 0;
		for (int i = 0; i < 237; i += 3) {
			indexBuffer[i] = 0;
			indexBuffer[i + 1] = prevIndex + 1;
			indexBuffer[i + 2] = prevIndex + 2;
			prevIndex++;
		}

		indexBuffer[237] = 0;
		indexBuffer[238] = 80;
		indexBuffer[239] = 1;
	}
};


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> prog2;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;
	GLuint IndexBufferID;

	GLuint OtherVertexArrayID;
	GLuint OtherVertexBufferID;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
		}
	}


	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		//vertex buffer declaration
		GLfloat g_vertex_buffer_data[243] = { 0 };

		Buffer::buildVertexBuffer(g_vertex_buffer_data);
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create and bind IBO
		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);

		//index budffer declaration
		GLuint g_index_buffer_data[240] = { 0 };

		Buffer::buildIndexBuffer(g_index_buffer_data);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_index_buffer_data), g_index_buffer_data, GL_DYNAMIC_DRAW);

		glBindVertexArray(0);

		glGenVertexArrays(1, &OtherVertexArrayID);
		glBindVertexArray(OtherVertexArrayID);
		glGenBuffers(1, &OtherVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, OtherVertexBufferID);
		
		static const GLfloat g_other_vertex_buffer_data[] =
		{
			0.0f, 0.0f, 0.0f,
			-width, height, 0.0f,
			width, height, 0.0f,

			0.0f, 0.0f, 0.0f,
			width, height, 0.0f,
			width, -height, 0.0f,

			0.0f, 0.0f, 0.0f,
			width, -height, 0.0f,
			-width, -height, 0.0f,

			0.0f, 0.0f, 0.0f,
			-width, -height, 0.0f,
			-width, height, 0.0f			
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(g_other_vertex_buffer_data), g_other_vertex_buffer_data, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindVertexArray(0);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.9f, 0.2f, 0.0f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		//Program 1
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/sun_vert33.glsl", resourceDirectory + "/sun_frag33.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("MV");
		prog->addUniform("uWindowSize");
		prog->addUniform("uTime");
		prog->addAttribute("vertPos");

		//Program 2
		prog2 = std::make_shared<Program>();
		prog2->setVerbose(true);
		prog2->setShaderNames(resourceDirectory + "/background_vert33.glsl", resourceDirectory + "/background_frag33.glsl");
		prog2->init();
		prog2->addUniform("P");
		prog2->addUniform("MV");
		prog2->addUniform("uWindowSize");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		auto P = std::make_shared<MatrixStack>();
		auto MV = std::make_shared<MatrixStack>();
		// Apply orthographic projection.
		P->pushMatrix();
		if (width > height)
		{
			P->ortho(-1*aspect, 1*aspect, -1, 1, -2, 100.0f);
		}
		else
		{
			P->ortho(-1, 1, -1*1/aspect, 1*1/aspect, -2, 100.0f);
		}
		MV->pushMatrix();

		// Draw the triangle using GLSL.
		prog->bind();

		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniform2f(prog->getUniform("uWindowSize"), (float)width, (float)height);
		glUniform1f(prog->getUniform("uTime"), (float)glfwGetTime());

		glBindVertexArray(VertexArrayID);

		//draw triangles
		glDrawElements(GL_TRIANGLES, 240, GL_UNSIGNED_INT, nullptr);
		//glDrawElements(GL_LINE_STRIP, 240, GL_UNSIGNED_INT, nullptr);
		prog->unbind();

		glBindVertexArray(OtherVertexArrayID);
		
		prog2->bind();
		glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniform2f(prog2->getUniform("uWindowSize"), (float)width, (float)height);

		glDrawArrays(GL_TRIANGLES, 0, 12);
		prog2->unbind();

		glBindVertexArray(0);

		prog->unbind();

		// Pop matrix stacks.
		MV->popMatrix();
		P->popMatrix();
	}

};

int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}

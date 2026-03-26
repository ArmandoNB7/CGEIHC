/*
Practica 5: Optimizacion y Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Model Goddard_M;

//Declarar mis modelos del blender 
//Model Goddard_Cuerpo;
//Model Goddard_Mandibula;
//Model Pata_derechaGoddard;
//Model Pata_izqGoddard;
//Model PataTrasera_derechaGoddard;
//Model PataTrasera_izqGoddard;
// Modelos del Coche
Model MICARRO;
Model capo;
Model rueda_der_frente;
Model rueda_der_trasera;
Model rueda_izq_frente;
Model rueda_izq_trasera;

// Ángulos de rotación para las patas (inician en 0)
//float rotPataDelDer = 0.0f;
//float rotPataDelIzq = 0.0f;
//float rotPataTrasDer = 0.0f;
//float rotPataTrasIzq = 0.0f;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";





void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 1.0f);

	Goddard_M = Model();
	Goddard_M.LoadModel("Models/goddard_base.obj");

	//Aqui agrego mis nuevas variables para mis modelos recientes 
	//el cuerpo del goddard
	//Goddard_Cuerpo = Model();
	//Goddard_Cuerpo.LoadModel("Models/Goddard.obj");

	////la mandibula del goddard
	//Goddard_Mandibula = Model();
	//Goddard_Mandibula.LoadModel("Models/MandibulaGoddard.obj");

	////las patas del goddard
	//Pata_derechaGoddard = Model();
	//Pata_derechaGoddard.LoadModel("Models/Pata_derechaGoddard.obj");

	//Pata_izqGoddard = Model();
	//Pata_izqGoddard.LoadModel("Models/Pata_izqGoddard.obj");

	//PataTrasera_derechaGoddard = Model();
	//PataTrasera_derechaGoddard.LoadModel("Models/PataTrasera_derechaGoddard.obj");

	//PataTrasera_izqGoddard = Model();
	//PataTrasera_izqGoddard.LoadModel("Models/PataTrasera_izqGoddard.obj");
	//// hasta aqui estan las patas de goddard 

	///todo lo del carro por partes 
	MICARRO = Model();
	MICARRO.LoadModel("Models/MICARRO.obj");

	capo = Model();
	capo.LoadModel("Models/capo.obj");

	rueda_der_frente = Model();
	rueda_der_frente.LoadModel("Models/rueda_der_frente.obj");

	rueda_der_trasera = Model();
	rueda_der_trasera.LoadModel("Models/rueda_der_trasera.obj");

	rueda_izq_frente = Model();
	rueda_izq_frente.LoadModel("Models/rueda_izq_frente.obj");

	rueda_izq_trasera = Model();
	rueda_izq_trasera.LoadModel("Models/rueda_izq_trasera.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh();

		////------------*INICIA DIBUJO DE NUESTROS DEM�S OBJETOS-------------------*
		////Goddard
		//color = glm::vec3(0.0f, 0.0f, 0.0f); //modelo de goddard de color negro
		//
		////modelaux = model;
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		////Goddard_M.RenderModel();//modificar por el modelo sin las 4 patas y sin cola
		//color = glm::vec3(0.0f, 0.0f, 1.0f);
		////En sesi�n se separara una parte del modelo de Goddard y se unir� por jeraqu�a al cuerpo
		//modelaux = model;
		////Cola


		//Siguientes modelos
		/* Ejercicio:
		1.- Separar las 4 patas de Goddard del modelo del cuerpo, unir por medio de jerarqu�a cada pata al cuerpo de Goddard
		2.- Hacer que al presionar una tecla cada pata pueda rotar un m�ximo de 45� "hacia adelante y hacia atr�s"
		*/

		
		//color = glm::vec3(0.74f, 0.74f, 0.74f); //
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));


		//// cuerppo goddard
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 1.5f, -1.5f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Goddard_Cuerpo.RenderModel();

		//// la mandibula
		//modelaux = model;
		//modelaux = glm::translate(modelaux, glm::vec3(0.8f, 0.5f, 1.2f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		//Goddard_Mandibula.RenderModel();

		////pata delantera der
		//modelaux = model;
	
		//modelaux = glm::translate(modelaux, glm::vec3(-0.4f, -0.8f, 1.5f));
		//modelaux = glm::rotate(modelaux, rotPataDelIzq * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//modelaux = glm::scale(modelaux, glm::vec3(0.65f, 0.65f, 0.65f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		//Pata_izqGoddard.RenderModel();

		////pata delantera izq
		//modelaux = model;
		//modelaux = glm::translate(modelaux, glm::vec3(-0.8f, -0.6f, 0.3f));
		//modelaux = glm::rotate(modelaux, rotPataDelDer * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//modelaux = glm::scale(modelaux, glm::vec3(1.2f, 1.2f, 1.2f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		//Pata_derechaGoddard.RenderModel();

		////esta es la pata izq trasera del goddard
		//modelaux = model;
		//
		//modelaux = glm::translate(modelaux, glm::vec3(-1.9f, -1.2f, 2.4f));
		//modelaux = glm::rotate(modelaux, rotPataTrasIzq * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		//PataTrasera_izqGoddard.RenderModel();

		////esta es la pata derecha trasera
		//modelaux = model;
		//
		//modelaux = glm::translate(modelaux, glm::vec3(-1.9f, -1.4f, 1.0f));
		//modelaux = glm::rotate(modelaux, rotPataTrasDer * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		//PataTrasera_derechaGoddard.RenderModel();''


		//cuerpo o chasis 
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getPosCoche(), 0.5f, -1.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MICARRO.RenderModel();

		//capo (Este rota con las teclas Y y H)
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-1.67f,0.19f, 0.05f));
		modelaux = glm::rotate(modelaux, mainWindow.getRotCofre() * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		capo.RenderModel();

		//llantas( estas rotan con las teclas T y G)
		// Llanta Delantera Derecha
		color = glm::vec3(0.0f, 1.0f, 0.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-2.0f, -0.35f, -1.0f));
		modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_der_frente.RenderModel();

		// Llanta Delantera Izquierda 
		color = glm::vec3(1.0f, 1.0f, 0.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-2.0f, -0.35f, 1.1f));
		modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_izq_frente.RenderModel();

		// Llanta Trasera Derecha
		color = glm::vec3(0.0f, 1.0f, 1.0f); //cyan
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(2.0f, -0.35f, 1.1f));
		modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_der_trasera.RenderModel();

		// Llanta Trasera Izquierda //magenta
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(2.0f, -0.6f, -1.3f));
		modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_izq_trasera.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

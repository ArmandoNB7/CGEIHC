/*
Práctica 6: Texturizado
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

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;

Model Kitt_M;
Model Llanta_M;
Model Dado_M;
//micarro exportado
Model MICARRO;
Model capo;
Model rueda_der_frente;
Model rueda_der_trasera;
Model rueda_izq_frente;
Model rueda_izq_trasera;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";




//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}



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

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);



	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.35f,  0.76f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.67f,	0.76f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.67f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.35f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3

		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.34f,  0.51f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.66f,	0.51f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.66f,	0.75f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.34f,	0.75f,		-1.0f,	0.0f,	0.0f,

		// back
		-0.5f, -0.5f, -0.5f,	0.65f,  0.26f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.34f,	0.26f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.34f,	0.5f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.56f,	0.5f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.34f,  0.01f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.65f,	0.01f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.65f,	0.25f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.34f,	0.25f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.5f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.0f,	0.73f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.33f,	0.73f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.33f,	0.5f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.65f,  0.75f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.65f,	0.5f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	0.5f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	1.0f,	0.75f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

	

}

void CrearDado8Caras()//dadito 8 caras
{
	unsigned int dado8_indices[] = {
		//  (Mitad superior)
		0, 1, 2,    // Cara 1: Top-Front-Right
		3, 4, 5,    // Cara 2: Top-Right-Back
		6, 7, 8,    // Cara 3: Top-Back-Left
		9, 10, 11,  // Cara 4: Top-Left-Front

		//  (Mitad inferior)
		12, 13, 14, // Cara 5: Bottom-Right-Front
		15, 16, 17, // Cara 6: Bottom-Back-Right
		18, 19, 20, // Cara 7: Bottom-Left-Back
		21, 22, 23  // Cara 8: Bottom-Front-Left
	};

	GLfloat dado8_vertices[] = {
		// x      y      z         S(U)    T(V)       nx    ny    nz

		// --- MITAD SUPERIOR ---
		// Cara 1 - 1
		0.0f,  1.0f,  0.0f,      0.5f,  0.73f,     0.0f, 0.0f, 0.0f, //top l
		0.0f,  0.0f,  1.0f,      0.26f,  0.5f,     0.0f, 0.0f, 0.0f,// bot
		1.0f,  0.0f,  0.0f,      0.75f,  0.5f,     0.0f, 0.0f, 0.0f, //top r

		// Cara 7 
		0.0f,  1.0f,  0.0f,      0.5f,  0.74f,     0.0f, 0.0f, 0.0f, // Top-L
		1.0f,  0.0f,  0.0f,      0.75f,  0.5f,     0.0f, 0.0f, 0.0f, // Bot
		0.0f,  0.0f, -1.0f,      1.0f,  0.74f,     0.0f, 0.0f, 0.0f, // Top-R

		// Cara 5
		0.0f,  1.0f,  0.0f,      0.5f,  0.74f,     0.0f, 0.0f, 0.0f, // Bot-L
		0.0f,  0.0f, -1.0f,      0.99f,  0.74f,     0.0f, 0.0f, 0.0f, // Bot-R
	   -1.0f,  0.0f,  0.0f,      0.74f,  1.0f,     0.0f, 0.0f, 0.0f, // Top

	   // Cara 3
	   0.0f,  1.0f,  0.0f,      0.49f,  0.72f,     0.0f, 0.0f, 0.0f, // Top-R
	  -1.0f,  0.0f,  0.0f,      0.03f,  0.72f,     0.0f, 0.0f, 0.0f, // Top-L
	   0.0f,  0.0f,  1.0f,      0.27f,  0.5f,     0.0f, 0.0f, 0.0f, // Bot

	   // Cara 4
	   0.0f, -1.0f,  0.0f,      0.50f,  0.25f,     0.0f, 0.0f, 0.0f, // Bot
	   1.0f,  0.0f,  0.0f,      0.27f,  0.49f,     0.0f, 0.0f, 0.0f, // Top-R
	   0.0f,  0.0f,  1.0f,      0.75f,  0.49f,     0.0f, 0.0f, 0.0f, // Top-L

	   // Cara 6:
	   0.0f, -1.0f,  0.0f,      0.51f,  0.25f,     0.0f, 0.0f, 0.0f, // Bot-L
	   0.0f,  0.0f, -1.0f,      0.75f,  0.48f,     0.0f, 0.0f, 0.0f, // Bot-R
	   1.0f,  0.0f,  0.0f,      1.0f,  0.25f,     0.0f, 0.0f, 0.0f, // Top

	   // Cara 8
	   0.0f, -1.0f,  0.0f,      0.5f,  0.25f,     0.0f, 0.0f, 0.0f, // Bot
	  -1.0f,  0.0f,  0.0f,      0.99f,  0.26f,     0.0f, 0.0f, 0.0f, // Top-L
	   0.0f,  0.0f, -1.0f,      0.75f,  0.0f,     0.0f, 0.0f, 0.0f, // Top-R

	   // Cara 2
	   0.0f, -1.0f,  0.0f,      0.5f,  0.25f,     0.0f, 0.0f, 0.0f, // Bot-R
	   0.0f,  0.0f,  1.0f,      0.27f,  0.49f,     0.0f, 0.0f, 0.0f, // Top
	  -1.0f,  0.0f,  0.0f,      0.0f,  0.25f,     0.0f, 0.0f, 0.0f  // Bot-L
	};
	
	calcAverageNormals(dado8_indices, 24, dado8_vertices, 192, 8, 5);

	Mesh* dado8 = new Mesh();
	dado8->CreateMesh(dado8_vertices, dado8_indices, 192, 24);
	meshList.push_back(dado8);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CrearDado8Caras();// dadito 8 caras
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	//dadoTexture = Texture("Textures/dado_emociones_v2.jpg");// cambiar textura aqui 
	//dadoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado8carasv5.jpg");// cambiar textura aqui 
	dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Dado_M = Model();
	Dado_M.LoadModel("Models/dado_emociones_v2.obj");

	///todo lo del carro por partes 
	MICARRO = Model();
	MICARRO.LoadModel("Models/MICARRO.obj");
	 
	capo = Model();
	capo.LoadModel("Models/capoTexturizadov3.obj");

	rueda_der_frente = Model();
	rueda_der_frente.LoadModel("Models/rueda_der_frente1.obj");

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
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();


		////Dado de Opengl
		////Ejercicio 1: Texturizar su cubo con la imagen dado_animales ya optimizada por ustedes
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//dadoTexture.UseTexture();
		//meshList[4]->RenderMesh();

		////Ejercicio 2:Importar el cubo texturizado en el programa de modelado con 
		////la imagen dado_animales ya optimizada por ustedes

		////Dado importado
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-4.0f, 2.0f, -2.0f));
		//model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Dado_M.RenderModel();




		/*Reporte de práctica :
		Ejercicio 1: Crear un dado de 8 caras y texturizarlo por medio de código
		Ejercicio 2: Importar el modelo de su coche con sus 4 llantas acomodadas
		y tener texturizadas las 4 llantas (diferenciar caucho y rin)  y
		texturizar el logo de la Facultad de ingeniería en el cofre de su propio modelo de coche

		//*/
		////Instancia del coche 
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), -0.5f, -3.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Kitt_M.RenderModel();

		////Llanta delantera izquierda
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		//color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta_M.RenderModel();

		////Llanta trasera izquierda
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta_M.RenderModel();

		////Llanta delantera derecha
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta_M.RenderModel();

		////Llanta trasera derecha
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta_M.RenderModel();

		
	
		// Ejercicio 1 reporte: Crear dado de 8 caras
	
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 6.5f, -2.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(1.0f, 1.0f, 1.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		dadoTexture.UseTexture();
		meshList[5]->RenderMesh();


		// Ejercicio 2: Carro y Llantas

		// cuerpo o chasis 
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f + mainWindow.getPosCoche(), 0.5f, -1.5f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MICARRO.RenderModel();

		// capo (Este rota con las teclas Y y H)
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-1.67f, 0.19f, 0.05f));
		//modelaux = glm::rotate(modelaux, mainWindow.getRotCofre() * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		capo.RenderModel();

		// Llanta Delantera Derecha
		color = glm::vec3(1.0f, 1.0f, 1.0f); //verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-2.0f, -0.35f, -1.0f));
		//modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_der_frente.RenderModel();

		// Llanta Delantera Izquierda 
		color = glm::vec3(1.0f, 1.0f, 1.0f); //amarilla
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-2.0f, -0.35f, 1.1f));
		//modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_izq_frente.RenderModel();

		// Llanta Trasera Derecha
		color = glm::vec3(1.0f, 1.0f, 1.0f); //cyan
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(2.0f, -0.35f, 1.1f));
		//modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_der_trasera.RenderModel();

		// Llanta Trasera Izquierda //magenta
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(2.0f, -0.6f, -1.3f));
		//modelaux = glm::rotate(modelaux, mainWindow.getRotLlantas() * toRadians, glm::vec3(0.0f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		rueda_izq_trasera.RenderModel();
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/
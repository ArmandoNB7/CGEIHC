/*
Pr�ctica 7: Iluminaci�n 1 
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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

//MICARRO
Model MICARRO;
Model capo;
Model rueda_der_frente;
Model rueda_der_trasera;
Model rueda_izq_frente;
Model rueda_izq_trasera;

//AQUI AGREGO MI LAMPARA
Model Mi_lamparita;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//funci�n de calculo de normales por promedio de v�rtices 
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
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
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

	};	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	-1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	-1.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	-1.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	-1.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
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
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");



	//MI CARRO 
	//todo lo del carro por partes 
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

	//Aqui agrego mi lampara 
	Mi_lamparita = Model();
	Mi_lamparita.LoadModel("Models/Mi_lamparita.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,//sol es toda la luz general (a(radiacion de la luz, que tanto ilumina), b (este es el sol, que tanta luz se percibe en general))
		0.0f, 0.0f, -1.0f); //Este indica desde donde iluminamos 
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual // ESTO ES LA INTENSIDAD DIFUSA
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, //que tan intenso es la luz puntual, en este caso es la luz roja, el primer valor(a, b), el a es el rango de extension de iluminacion, y el b es la intensidad
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f); //abre el cono spotlight
	spotLightCount++;
	
	//se crean mas luces puntuales y spotlight 
	//Aqui esta la luz del carro 
	spotLights[2] = SpotLight(0.0f, 0.0f, 0.777f,  // Color Azul 
		0.8f, 3.0f,                              
		0.0f, 0.0f, 0.0f,                         
		-1.0f, -0.2f, 0.0f,                       
		1.0f, 0.4f, 0.012f,                        
		15.0f);                                  // Ángulo de apertura del cono
	spotLightCount++;                          

	// LUZ del helicptero
	spotLights[3] = SpotLight(1.0f, 0.8f, 0.0f,  
		1.0f, 5.0f,                              
		0.0f, 0.0f, 0.0f,                     
		0.0f, -1.0f, 0.0f,                       // En -Y pq apunta directo hacia abajo 
		1.0f, 0.04f, 0.002f,                     // Atenuación (Valores bajos porque el heli se supone q vuela alto)
		40.0f);                                  
	spotLightCount++;                            

	//mi lamparita
	glm::vec3 posicionFarola = glm::vec3(10.0f, -1.0f, -10.0f); //aqui asigne la posicion desde aca para q este ligada a la lampara de una vez 

	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 5.0f,
		posicionFarola.x + 2.4f,                      
		posicionFarola.y + 7.6f,                        
		posicionFarola.z - 2.4f,                         
		1.0f, 0.05f, 0.012f);
		pointLightCount++;
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
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
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		//sirve para que en tiempo de ejecuci�n (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//aqui implementamos nuestro faro 
		glm::mat4 modelLuzCoche = glm::mat4(1.0);
		modelLuzCoche = glm::translate(modelLuzCoche, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
		modelLuzCoche = glm::scale(modelLuzCoche, glm::vec3(1.5f, 1.5f, 1.5f));

		glm::vec4 posFaroLocal = glm::vec4(-2.1f, 0.2f, 1.0f, 1.0f);

		glm::vec4 dirFaroLocal = glm::vec4(-1.0f, -0.1f, 0.0f, 0.0f);
		glm::vec4 posFaroMundo = modelLuzCoche * posFaroLocal;
		glm::vec4 dirFaroMundo = modelLuzCoche * dirFaroLocal;
		spotLights[2].SetFlash(glm::vec3(posFaroMundo), glm::vec3(dirFaroMundo));

		//Aqui se implementa la luz pero directamente al helicptero
		glm::mat4 modelLuzHeli = glm::mat4(1.0);
		modelLuzHeli = glm::translate(modelLuzHeli, glm::vec3(0.0f + mainWindow.getmuevexHelix(), 0.5f, -3.0f));
		modelLuzHeli = glm::translate(modelLuzHeli, glm::vec3(0.0f, 5.0f, 6.0));

		glm::vec4 posReflectorLocal = glm::vec4(0.0f, -0.5f, 0.0f, 1.0f);
		glm::vec4 posReflectorMundo = modelLuzHeli * posReflectorLocal;
		glm::vec3 dirReflectorMundo = glm::vec3(0.0f, -1.0f, 0.0f);

		spotLights[3].SetFlash(glm::vec3(posReflectorMundo), dirReflectorMundo);

		//spotLights[1].SetPos(poscoche + glm::vec(x, y, cofre));

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		////Instancia del coche 
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
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
	

		model = glm::mat4(1.0);
		//EJERCICIO 1; RP7
		// //ESTO ES DEL helicopeto
		//Aqui mandamos a llamar al nuevo muevex diferente con R y T para que nuestro helicoptero se mueva independiente al carro
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevexHelix(), 0.5f, -3.0f));
		model = glm::translate(model, glm::vec3(0.0f, 8.0f, 6.0));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		meshList[4]->RenderMesh();

		//Mi farola
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = glm::mat4(1.0);
		model = glm::translate(model, posicionFarola);
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Mi_lamparita.RenderModel();


		//CARRO MIO 
		// Ejercicio 2: Carro y Llantas

		// cuerpo o chasis 
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f + mainWindow.getPosCoche(), 0.5f, -1.5f));
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
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


		//Agave �qu� sucede si lo renderizan antes del coche y el helic�ptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

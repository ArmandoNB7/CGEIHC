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
Texture dadoTexture;
Texture cristalTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
//Model Dado_M;


//MICARRO
Model MICARRO;
Model capo;
Model rueda_der_frente;
Model rueda_der_trasera;
Model rueda_izq_frente;
Model rueda_izq_trasera;

//AQUI AGREGO MI LAMPARA
Model Mi_lamparita;

//Aqui agregamos nave
Model nave;

//AQUI AGREGARE MI PECERA
Model Pecera;

//AQUI AGREGARE MI PEZ ABISAL
Model PezCuerpo;
Model PezAntena;
Model PezBulbo;

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
		// //1.- Agregar su dado de 8 caras y editar sus normales para que las caras del dado sean iluminadas correctamente.
		// Cara 1 - 1
		0.0f,  1.0f,  0.0f,      0.5f,  0.73f,     -1.0f,	-1.0f,	-1.0f, //top l
		0.0f,  0.0f,  1.0f,      0.26f,  0.5f,     -1.0f,	-1.0f,	-1.0f,// bot  //YA
		1.0f,  0.0f,  0.0f,      0.75f,  0.5f,     -1.0f,	-1.0f,	-1.0f, //top r

		// Cara 7 
		0.0f,  1.0f,  0.0f,      0.5f,  0.75f,     -1.0f, -1.0f, 1.0f, // Top-L
		1.0f,  0.0f,  0.0f,      0.75f,  0.51f,     -1.0f, -1.0f, 1.0f, // Bot  //YA
		0.0f,  0.0f, -1.0f,      0.99f,  0.76f,     -1.0f, -1.0f, 1.0f, // Top-R

		// Cara 5
		0.0f,  1.0f,  0.0f,      0.5f,  0.74f,     1.0f, -1.0f, 1.0f, // Bot-L
		0.0f,  0.0f, -1.0f,      0.99f,  0.74f,     1.0f, -1.0f, 1.0f, // Bot-R   YA
	   -1.0f,  0.0f,  0.0f,      0.74f,  1.0f,     1.0f, -1.0f, 1.0f, // Top

	   // Cara 3
	   0.0f,  1.0f,  0.0f,      0.49f,  0.75f,     1.0f, -1.0f, -1.0f, // Top-R
	  -1.0f,  0.0f,  0.0f,      0.00f,  0.75f,     1.0f, -1.0f, -1.0f, // Top-L  YA
	   0.0f,  0.0f,  1.0f,      0.25f,  0.5f,      1.0f, -1.0f, -1.0f, // Bot

	   // Cara 4
	   0.0f, -1.0f,  0.0f,      0.50f,  0.25f,     -1.0f, 1.0f, -1.0f, // Bot
	   1.0f,  0.0f,  0.0f,      0.75f,  0.49f,     -1.0f, 1.0f, -1.0f, // Top-R YA
	   0.0f,  0.0f,  1.0f,      0.27f,  0.49f,     -1.0f, 1.0f, -1.0f, // Top-L   

	   // Cara 6:
	   0.0f, -1.0f,  0.0f,      0.5f,  0.25f,     -1.0f, 1.0f, 1.0f, // Bot-L
	   0.0f,  0.0f, -1.0f,      1.0f,  0.25f,     -1.0f, 1.0f, 1.0f, // Bot-R
	   1.0f,  0.0f,  0.0f,      0.75f,  0.5f,     -1.0f, 1.0f, 1.0f, // Top 1.0f,  0.25f,

	   // Cara 8
	   0.0f, -1.0f,  0.0f,      0.5f,  0.25f,      1.0f, 1.0f, 1.0f, // Bot
	  -1.0f,  0.0f,  0.0f,      0.99f,  0.26f,     1.0f, 1.0f, 1.0f, // Top-L
	   0.0f,  0.0f, -1.0f,      0.75f,  0.0f,      1.0f, 1.0f, 1.0f, // Top-R

	   // Cara 2
	   0.0f, -1.0f,  0.0f,      0.5f,  0.25f,      1.0f, 1.0f, -1.0f, // Bot-R
	   0.0f,  0.0f,  1.0f,      0.27f,  0.49f,     1.0f, 1.0f, -1.0f, // Top
	  -1.0f,  0.0f,  0.0f,      0.0f,  0.25f,      1.0f, 1.0f, -1.0f  // Bot-L
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
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado8carasv5.jpg");// cambiar textura aqui 
	dadoTexture.LoadTextureA();

	cristalTexture = Texture("Textures/textura_pecera.png");
	cristalTexture.LoadTextureA(); 



	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	//Dado_M = Model();
	//Dado_M.LoadModel("Models/dado_emociones_v2.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	nave = Model();
	nave.LoadModel("Models/nave.obj");
	Pecera = Model();
	Pecera.LoadModel("Models/Pecera.obj");


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

	//AQUI MI PESCADITO
	PezCuerpo = Model();
	PezCuerpo.LoadModel("Models/cuerpo_pez.obj");

	PezAntena = Model();
	PezAntena.LoadModel("Models/antena_pez.obj");

	PezBulbo = Model();
	PezBulbo.LoadModel("Models/bulbo.obj");
	

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

	//Ejericio 1; P08 Luces de la nave ==========================================================================================
	// Reflector  de delante de la navecita cyan
	spotLights[4] = SpotLight(0.0f, 1.0f, 1.0f,
		0.0f, 3.0f,                         
		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.012f,
		20.0f);
	spotLightCount++;

	// Reflector atras de la anavecita naranja
	spotLights[5] = SpotLight(1.0f, 0.5f, 0.0f,
		0.0f, 0.0f, // Empieza apagado (Intensidad 0)                         
		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.012f,
		20.0f);
	spotLightCount++;
	//  ==========================================================================================

	// Luz para el pez abisal
	spotLights[6] = SpotLight(1.0f, 0.0f, 1.0f,  
		0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,                     
		1.0f, 0.05f, 0.012f,
		15.0f);                               
	spotLightCount = 7; 

	//mi lamparita
	glm::vec3 posicionFarola = glm::vec3(10.0f, -1.0f, -10.0f); //aqui asigne la posicion desde aca para q este ligada a la lampara de una vez 

	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 5.0f,
		posicionFarola.x + 2.4f,                      
		posicionFarola.y + 7.6f,                        
		posicionFarola.z - 2.4f,                         
		5.0f, 0.05f, 0.012f); //Valores por defecto para que no se me olviden: 1.0f, 0.05f, 0.012f
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

			//aqui el ejriccio 2 
		static int arregloActivo = 1;
		if (mainWindow.getsKeys()[GLFW_KEY_1]) {
			arregloActivo = 1;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_2]) {
			arregloActivo = 2;
		}

		glm::vec3 p1(-15.0f, 2.0f, -8.0f);
		glm::vec3 p2(-10.0f, 2.0f, -8.0f);
		glm::vec3 p3(-5.0f, 2.0f, -8.0f);
		glm::vec3 p4(0.0f, 2.0f, -8.0f);

		if (arregloActivo == 1) {
			// ARREGLO 1: Verde, Azul, Roja, Blanca 
			pointLights[2] = PointLight(0.0f, 1.0f, 0.0f, 0.0f, 1.5f, p1.x, p1.y, p1.z, 1.0f, 0.2f, 0.1f);
			pointLights[3] = PointLight(0.0f, 0.0f, 1.0f, 0.0f, 1.5f, p2.x, p2.y, p2.z, 1.0f, 0.2f, 0.1f);
			pointLights[4] = PointLight(1.0f, 0.0f, 0.0f, 0.0f, 1.5f, p3.x, p3.y, p3.z, 1.0f, 0.2f, 0.1f);
			pointLights[5] = PointLight(1.0f, 1.0f, 1.0f, 0.0f, 1.5f, p4.x, p4.y, p4.z, 1.0f, 0.2f, 0.1f);
		}
		else {
			// ARREGLO 2: Blanca, Verde, Azul, Roja 
			pointLights[2] = PointLight(1.0f, 1.0f, 1.0f, 0.0f, 1.5f, p1.x, p1.y, p1.z, 1.0f, 0.2f, 0.1f);
			pointLights[3] = PointLight(0.0f, 1.0f, 0.0f, 0.0f, 1.5f, p2.x, p2.y, p2.z, 1.0f, 0.2f, 0.1f);
			pointLights[4] = PointLight(0.0f, 0.0f, 1.0f, 0.0f, 1.5f, p3.x, p3.y, p3.z, 1.0f, 0.2f, 0.1f);
			pointLights[5] = PointLight(1.0f, 0.0f, 0.0f, 0.0f, 1.5f, p4.x, p4.y, p4.z, 1.0f, 0.2f, 0.1f);
		}
		pointLightCount = 7;// aqui me di cuenta del error de pasar de 8 a 6 


		//2.- Apagar con teclado la luz 
		//Boton OFF/ON para mi lamparita'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
		static bool lamparaEncendida = true;  
		static bool teclaOPresionada = false; 

		
		if (mainWindow.getsKeys()[GLFW_KEY_O]) {
			if (!teclaOPresionada) { 
				lamparaEncendida = !lamparaEncendida; 
				teclaOPresionada = true; 
			}
		}
		else {
			teclaOPresionada = false; // se abre el candado cuando se suelta la tecla para q detecte que estamos presionando la tecla O 
		}

		if (lamparaEncendida) {
			// encendida con la ambiental en 5.0
			pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
				0.0f, 5.0f,
				posicionFarola.x + 2.4f,
				posicionFarola.y + 7.6f,
				posicionFarola.z - 2.4f,
				5.0f, 0.05f, 0.012f);
		}
		else {
			// apagadda cuadno la intensidad difusa y abiental a 0.0f
			pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
				0.0f, 0.0f, 
				posicionFarola.x + 2.4f,
				posicionFarola.y + 7.6f,
				posicionFarola.z - 2.4f,
				5.0f, 0.05f, 0.012f);
		}
		// ''''''''''''''''''''''''''''''''''''''''''''''''''''''''

		//**************************************************************************
		//3.- Boton C para Hacer que al presionar una tecla, la luz del faro del coche cicle de color entre rojo, amarillo, verde, cian, azul, magenta
		static int colorFaro = 0; 
		static bool teclaCPresionada = false;

		
		if (mainWindow.getsKeys()[GLFW_KEY_C]) {
			if (!teclaCPresionada) {
				colorFaro++; // Pasamos al siguiente color
				if (colorFaro > 5) {
					colorFaro = 0; 
				}
				teclaCPresionada = true;
			}
		}
		else {
			teclaCPresionada = false; 
		}

		// 2. Definir los valores RGB según el estado actual
		GLfloat r = 1.0f, g = 1.0f, b = 1.0f;
		switch (colorFaro) {
		case 0: r = 1.0f; g = 0.0f; b = 0.0f; break; // Rojo
		case 1: r = 1.0f; g = 1.0f; b = 0.0f; break; // Amarillo
		case 2: r = 0.0f; g = 1.0f; b = 0.0f; break; // Verde
		case 3: r = 0.0f; g = 1.0f; b = 1.0f; break; // Cian
		case 4: r = 0.0f; g = 0.0f; b = 1.0f; break; // Azul
		case 5: r = 1.0f; g = 0.0f; b = 1.0f; break; // Magenta
		}
		spotLights[2] = SpotLight(r, g, b,
			0.0f, 3.0f,                             
			0.0f, 0.0f, 0.0f,                     
			-1.0f, -0.2f, 0.0f,                      
			1.0f, 0.4f, 0.012f,                      
			15.0f);              
		// ******************************************************************

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

	

		//Aqui se implementa la luz pero directamente al helicptero pero para la PRACTICA 08 SE COMENTA 
		//glm::mat4 modelLuzHeli = glm::mat4(1.0);
		//modelLuzHeli = glm::translate(modelLuzHeli, glm::vec3(0.0f + mainWindow.getmuevexHelix(), 0.5f, -3.0f));
		//modelLuzHeli = glm::translate(modelLuzHeli, glm::vec3(0.0f, 5.0f, 6.0));

		//glm::vec4 posReflectorLocal = glm::vec4(0.0f, -0.5f, 0.0f, 1.0f);
		//glm::vec4 posReflectorMundo = modelLuzHeli * posReflectorLocal;
		//glm::vec3 dirReflectorMundo = glm::vec3(0.0f, -1.0f, 0.0f);

		//spotLights[3].SetFlash(glm::vec3(posReflectorMundo), dirReflectorMundo);

		// --- EJ1 P08 asignar la logica de las luces ===========================================================================

	
		static GLfloat lastNaveX = mainWindow.getmuevexHelix();
		GLfloat currentNaveX = mainWindow.getmuevexHelix();
		static bool naveAvanzando = true; 

		if (currentNaveX < lastNaveX) {
			naveAvanzando = true;  
		}
		else if (currentNaveX > lastNaveX) {
			naveAvanzando = false;
		}
		lastNaveX = currentNaveX; 

		// Prender y apagar focos para q al hacer el cambio se apaguen 
		GLfloat intAdelante = naveAvanzando ? 3.0f : 0.0f;
		GLfloat intAtras = !naveAvanzando ? 3.0f : 0.0f;

		
		spotLights[4] = SpotLight(0.0f, 1.0f, 1.0f, 0.0f, intAdelante, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.05f, 0.012f, 20.0f);
		spotLights[5] = SpotLight(1.0f, 0.5f, 0.0f, 0.0f, intAtras, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.05f, 0.012f, 20.0f);

		// le doy la posicion de la nave 
		glm::mat4 modelNaveLuz = glm::mat4(1.0);
		modelNaveLuz = glm::translate(modelNaveLuz, glm::vec3(0.0f + mainWindow.getmuevexHelix(), 0.5f, -3.0f));
		modelNaveLuz = glm::translate(modelNaveLuz, glm::vec3(0.0f, 1.0f, 6.0));

		glm::vec4 posAdelanteLocal = glm::vec4(-2.0f, 0.0f, 0.0f, 1.0f); 
		glm::vec4 dirAdelanteLocal = glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f); 

		glm::vec4 posAtrasLocal = glm::vec4(2.0f, 0.0f, 0.0f, 1.0f);  
		glm::vec4 dirAtrasLocal = glm::vec4(1.0f, -1.0f, 0.0f, 0.0f);  

		spotLights[4].SetFlash(glm::vec3(modelNaveLuz * posAdelanteLocal), glm::vec3(modelNaveLuz * dirAdelanteLocal));
		spotLights[5].SetFlash(glm::vec3(modelNaveLuz * posAtrasLocal), glm::vec3(modelNaveLuz * dirAtrasLocal));
		// ===================================================================



		//spotLights[1].SetPos(poscoche + glm::vec(x, y, cofre));

	//Mov de mi pez y luz ======================================================

		static glm::vec3 posPez = glm::vec3(40.0f, 5.0f, 40.0f);
		float velPez = 0.5f * deltaTime;

		float limiteMinY = 3.5f, limiteMaxY = 12.0f;
		float limiteMinZ = 30.0f, limiteMaxZ = 50.0f;

		if (mainWindow.getsKeys()[GLFW_KEY_Z]) {
			posPez.y += velPez; // Sube
			posPez.z -= velPez; // Nada hacia el frente
		}
		if (mainWindow.getsKeys()[GLFW_KEY_X]) {
			posPez.y -= velPez; // Baja
			posPez.z += velPez; // Retrocede
		}

		if (posPez.y > limiteMaxY) posPez.y = limiteMaxY;
		if (posPez.y < limiteMinY) posPez.y = limiteMinY;
		if (posPez.z > limiteMaxZ) posPez.z = limiteMaxZ;
		if (posPez.z < limiteMinZ) posPez.z = limiteMinZ;

		// luz del bulbo para el pezc
		// Cle paso su posicion 
		glm::mat4 modelPezLuz = glm::mat4(1.0);
		modelPezLuz = glm::translate(modelPezLuz, posPez);
		modelPezLuz = glm::scale(modelPezLuz, glm::vec3(0.5f, 0.5f, 0.5f));
		glm::vec4 posBulboLocal = glm::vec4(-0.2f, 10.4f, 7.7f, 1.0f);
		glm::vec4 posBulboMundo = modelPezLuz * posBulboLocal;

		// Con la P de pez
		static bool bulboEncendido = true;
		static bool teclaPPresionada = false;
		if (mainWindow.getsKeys()[GLFW_KEY_P]) {
			if (!teclaPPresionada) {
				bulboEncendido = !bulboEncendido;
				teclaPPresionada = true;
			}
		}
		else {
			teclaPPresionada = false;
		}

		GLfloat intBulbo = bulboEncendido ? 2.0f : 0.0f;
		pointLights[6] = PointLight(0.0f, 0.0f, 1.0f,
			0.0f, intBulbo,
			posBulboMundo.x, posBulboMundo.y, posBulboMundo.z,
			0.3f, 0.2f, 0.1f);

		static glm::vec3 dirReflectorPez = glm::vec3(0.0f, -1.0f, 0.0f);
		float velRot = 1.0f * deltaTime;

		
		if (mainWindow.getsKeys()[GLFW_KEY_J]) dirReflectorPez.x -= velRot;//j
		if (mainWindow.getsKeys()[GLFW_KEY_L]) dirReflectorPez.x += velRot;//L
		if (mainWindow.getsKeys()[GLFW_KEY_I]) dirReflectorPez.y += velRot;//I
		if (mainWindow.getsKeys()[GLFW_KEY_K]) dirReflectorPez.y -= velRot;//K
		if (mainWindow.getsKeys()[GLFW_KEY_N]) dirReflectorPez.z -= velRot;//N
		if (mainWindow.getsKeys()[GLFW_KEY_M]) dirReflectorPez.z += velRot;//M

		
		if (glm::length(dirReflectorPez) > 0.1f) {
			dirReflectorPez = glm::normalize(dirReflectorPez);
		}

		spotLights[6].SetFlash(glm::vec3(posBulboMundo), dirReflectorPez);

		// ========================================================================

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


		//DADITO 8 CARAS 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 6.5f, -2.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		
		dadoTexture.UseTexture();
		meshList[5]->RenderMesh();


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



		//pecesitoi abisal
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glm::mat4 modelPezBase = glm::mat4(1.0);
		modelPezBase = glm::translate(modelPezBase, posPez);
		modelPezBase = glm::scale(modelPezBase, glm::vec3(0.5f, 0.5f, 0.5f)); 

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		// Cuerpo
		model = modelPezBase; 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PezCuerpo.RenderModel();

		// Antena
		model = modelPezBase;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PezAntena.RenderModel();

		//  Bulbo
		model = modelPezBase;
		model = glm::translate(model, glm::vec3(-0.2f, 10.4f, 7.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PezBulbo.RenderModel();

		//mi pecera
		// Esto es para la transparencia 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//cristalTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess); 
		Pecera.RenderModel();

		//Agave �qu� sucede si lo renderizan antes del coche y el helic�ptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		
		//Mi navecita 
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevexHelix(), 0.5f, -3.0f));
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 6.0));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nave.RenderModel();

		

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

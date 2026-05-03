

//Práctica 9: Animación
//Ing.Jose Roque RG
//•
//11 : 31 a.m.
//No se adjuntaron archivos proporcionados anteriormente, solo nuevos archivos o modificados.
//Manual Práctica 9 Animación JRRG.pdf
//PDF
//
//Practica 9 sesion C.zip
//Archivo comprimido
//
//numeros.zip
//Archivo comprimido
//
//practica9_c.cpp
//Texto
//
//flechas.tga
//Desconocido
//
//Comentarios de la clase
//
//Agrega un comentario de clase…

/*
Animación:
- Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
-Compleja: Por medio de funciones y algoritmos.
-Textura Animada
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

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;



Model Kitt_M;
Model Llanta_M;
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Aeolipile_base_M;
Model Aeolipile_M;

//MICARRO
Model MICARRO;
Model capo;
Model rueda_der_frente;
Model rueda_der_trasera;
Model rueda_izq_frente;
Model rueda_izq_trasera;

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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

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


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	Ala_M = Model();
	Ala_M.LoadModel("Models/ala.obj");
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile.obj");


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


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
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
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
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

	// Luces de la nave ==========================================================================================
	// Reflector  de delante de la navecita cyan
	spotLights[3] = SpotLight(0.0f, 1.0f, 1.0f,
		0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.012f,
		20.0f);
	spotLightCount++;

	// Reflector atras de la anavecita naranja
	spotLights[4] = SpotLight(1.0f, 0.5f, 0.0f,
		0.0f, 0.0f,                          
		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.012f,
		20.0f);
	spotLightCount++;
	//  ==========================================================================================

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.3f;
	rotllanta = 1.0f;
	rotllantaOffset = 10.0f;

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	
	struct PuntoRuta {
		float x, y, z;       
		float giroY;         
		float giroZ;         
	};

	PuntoRuta rutaCoche[] = {
		//    X (Avance)   Y (Altura)   Z (Lados)   giroY(Volante)  giroZ(Rampa)

		// recto inicio
		{    0.0f,       0.0f,       4.0f,        0.0f,         0.0f }, // P0: Salida
		{  -40.0f,       0.0f,       4.0f,        0.0f,         0.0f }, // P1: para enderezarse

		// primera curva izquierda
		{  -45.0f,       0.0f,       10.0f,       35.0f,         0.0f }, // P2: Vértice curva izquierda
		{  -55.0f,       1.0f,      10.0f,        0.0f,         0.0f }, // P3: enderza x2
		// curva derecha
		{  -60.0f,      2.0f,      6.0f,     -55.0f,         0.0f }, // P5: Vértice curva derecha

		// curva derecha antes de subir la rampa
		{  -74.0f,       3.5f,       -6.0f,      -65.0f,         -5.0f }, // P4: Vértice curva derecha
		{  -76.0f,       6.5f,       -6.0f,        0.0f,         -10.0f }, // P5: endereza x3 antes de la curva

		// rampa
		{  -100.0f,       12.5f,       -5.0f,      -15.0f,        -25.0f }, 
		{ -110.0f,        22.0f,      -4.0f,        0.0f,        -25.0f }, 
		{ - 125.0f,       25.0f,       -8.0f,        -15.0f,         0.0f}, 
	    { -170.0f,        27.0f,       -15.0f,        -25.0f,         0.0f },  
		{ -185.0f,        24.0f,       -25.0f,        -20.0f,         5.0f }  
	};

	
	int numPuntosPista = 12;
	float indiceRuta = 0.0f;
	float velRuta = 0.007f;
	bool animacionCocheActiva = true;

	
	PuntoRuta rutaNave[] = {
		//   X(Avance)  Y(Altura)  Z(Lado)  giroY(Volante)  giroZ(Cabeceo)

		{ -185.0f,      35.0f,     -15.0f,     160.0f,        0.0f }, //inico del vuelo
		{ -125.0f,      30.0f,      -4.0f,     180.0f,       -5.0f },
		{  -70.0f,      20.0f,      -6.0f,     180.0f,      -10.0f }, // sobrevuela la rampa
		{  -30.0f,      10.0f,      -8.0f,     180.0f,      -15.0f }, // empieza a aterrixar
		{    0.0f,       0.5f,     -10.0f,     180.0f,        0.0f }  //qterriza
	};

	int numPuntosNave = 5;
	float indiceRutaNave = 0.0f;
	float velRutaNave = 0.005f; // Velocidad de vuelo
	bool animacionNaveActiva = false; 
	bool naveYaVolo = false;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		angulovaria += 0.5f * deltaTime;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		

		// --- Lógica del Coche 
		if (mainWindow.getsKeys()[GLFW_KEY_R]) {
			indiceRuta = 0.0f;
			animacionCocheActiva = true;
		}

		if (mainWindow.getsKeys()[GLFW_KEY_V]) {
			if (!naveYaVolo) {
				animacionNaveActiva = true; 
				naveYaVolo = true;          
			}
		}

		glm::vec3 posCocheCalculada(0.0f);
		float yawCoche = 0.0f;
		float pitchCoche = 0.0f;

		if (animacionCocheActiva) {
			indiceRuta += velRuta * deltaTime;
			if (indiceRuta >= numPuntosPista - 1) {
				indiceRuta = numPuntosPista - 1.001f;
				animacionCocheActiva = false;
			}
			else {
				rotllanta -= rotllantaOffset * deltaTime;
			}
		}

		int pActual = (int)indiceRuta;
		int pSiguiente = pActual + 1;
		float t = indiceRuta - pActual;

		posCocheCalculada.x = rutaCoche[pActual].x + t * (rutaCoche[pSiguiente].x - rutaCoche[pActual].x);
		posCocheCalculada.y = rutaCoche[pActual].y + t * (rutaCoche[pSiguiente].y - rutaCoche[pActual].y);
		posCocheCalculada.z = rutaCoche[pActual].z + t * (rutaCoche[pSiguiente].z - rutaCoche[pActual].z);
		yawCoche = rutaCoche[pActual].giroY + t * (rutaCoche[pSiguiente].giroY - rutaCoche[pActual].giroY);
		pitchCoche = rutaCoche[pActual].giroZ + t * (rutaCoche[pSiguiente].giroZ - rutaCoche[pActual].giroZ);

		// --- Lógica de la Nave ---
		

		glm::vec3 posNaveCalculada(0.0f);
		float yawNave = 0.0f, pitchNave = 0.0f;
		static float rotAla = 0.0f;
		static float rotHelice = 0.0f;
		static bool bajaAla = true;


		if (animacionNaveActiva) {
			indiceRutaNave += velRutaNave * deltaTime;



			
			if (indiceRutaNave >= numPuntosNave - 1) {
				indiceRutaNave = numPuntosNave - 1.001f;
				animacionNaveActiva = false;
			}
			else {
				
				rotHelice += 500.0f * deltaTime; 

				if (bajaAla) {
					rotAla -= 50.0f * deltaTime;
					if (rotAla <= -45.0f) bajaAla = false;
				}
				else {
					rotAla += 50.0f * deltaTime;
					if (rotAla >= 45.0f) bajaAla = true;
				}
			}
		}

		int pActNave = (int)indiceRutaNave;
		int pSigNave = pActNave + 1;
		float tNave = indiceRutaNave - pActNave;

		posNaveCalculada.x = rutaNave[pActNave].x + tNave * (rutaNave[pSigNave].x - rutaNave[pActNave].x);
		posNaveCalculada.y = rutaNave[pActNave].y + tNave * (rutaNave[pSigNave].y - rutaNave[pActNave].y);
		posNaveCalculada.z = rutaNave[pActNave].z + tNave * (rutaNave[pSigNave].z - rutaNave[pActNave].z);
		yawNave = rutaNave[pActNave].giroY + tNave * (rutaNave[pSigNave].giroY - rutaNave[pActNave].giroY);
		pitchNave = rutaNave[pActNave].giroZ + tNave * (rutaNave[pSigNave].giroZ - rutaNave[pActNave].giroZ);

		//  Faro coch3
		static int colorFaro = 0;
		static bool teclaCPresionada = false;

		if (mainWindow.getsKeys()[GLFW_KEY_C]) {
			if (!teclaCPresionada) {
				colorFaro++;
				if (colorFaro > 5) colorFaro = 0;
				teclaCPresionada = true;
			}
		}
		else {
			teclaCPresionada = false;
		}

		GLfloat r = 1.0f, g = 1.0f, b = 1.0f;
		switch (colorFaro) {
		case 0: r = 1.0f; g = 0.0f; b = 0.0f; break; // Rojo
		case 1: r = 1.0f; g = 1.0f; b = 0.0f; break; // Amarillo
		case 2: r = 0.0f; g = 1.0f; b = 0.0f; break; // Verde
		case 3: r = 0.0f; g = 1.0f; b = 1.0f; break; // Cian
		case 4: r = 0.0f; g = 0.0f; b = 1.0f; break; // Azul
		case 5: r = 1.0f; g = 0.0f; b = 1.0f; break; // Magenta
		}
		spotLights[2] = SpotLight(r, g, b, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, -1.0f, -0.2f, 0.0f, 1.0f, 0.4f, 0.012f, 15.0f);


		

		// Matriz Madre Coche
		glm::mat4 modelCocheBase = glm::mat4(1.0);
		modelCocheBase = glm::translate(modelCocheBase, posCocheCalculada);
		modelCocheBase = glm::rotate(modelCocheBase, yawCoche * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelCocheBase = glm::rotate(modelCocheBase, pitchCoche * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		modelCocheBase = glm::rotate(modelCocheBase, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelCocheBase = glm::scale(modelCocheBase, glm::vec3(0.8f, 0.8f, 0.8f));
		modelCocheBase = glm::rotate(modelCocheBase, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		// Luz Faro Coche
		glm::vec4 posFaroLocal = glm::vec4(-2.1f, 0.2f, 1.0f, 1.0f);
		glm::vec4 dirFaroLocal = glm::vec4(-1.0f, -0.1f, 0.0f, 0.0f);
		spotLights[2].SetFlash(glm::vec3(modelCocheBase * posFaroLocal), glm::vec3(modelCocheBase * dirFaroLocal));

		// Matriz Madre Nave
		glm::mat4 modelNaveBase = glm::mat4(1.0);
		modelNaveBase = glm::translate(modelNaveBase, posNaveCalculada);
		modelNaveBase = glm::rotate(modelNaveBase, yawNave * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelNaveBase = glm::rotate(modelNaveBase, pitchNave * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelNaveBase = glm::scale(modelNaveBase, glm::vec3(5.0f, 5.0f, 5.0f)); // Escala general

		// Luces Nave 
		GLfloat luzNavePrendida = animacionNaveActiva ? 3.0f : 0.0f; 
		spotLights[3] = SpotLight(0.0f, 1.0f, 1.0f, 0.0f, luzNavePrendida, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.05f, 0.012f, 30.0f);

		glm::vec4 posFaroNaveLocal = glm::vec4(0.0f, -0.5f, 0.0f, 1.0f); // Debajo de la panza
		glm::vec4 dirFaroNaveLocal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f); // Apuntando estricto a -Y
		spotLights[3].SetFlash(glm::vec3(modelNaveBase* posFaroNaveLocal), glm::vec3(modelNaveBase* dirFaroNaveLocal));


		// Luz Linterna Cámara
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		
		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		// piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.1f, 2.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista_M.RenderModel();

		// cochexito
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = modelCocheBase;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MICARRO.RenderModel();

		model = modelCocheBase;
		model = glm::translate(model, glm::vec3(-1.67f, 0.19f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		capo.RenderModel();

		model = modelCocheBase;
		model = glm::translate(model, glm::vec3(-2.0f, -0.35f, -1.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rueda_der_frente.RenderModel();

		model = modelCocheBase;
		model = glm::translate(model, glm::vec3(-2.0f, -0.35f, 1.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rueda_izq_frente.RenderModel();

		model = modelCocheBase;
		model = glm::translate(model, glm::vec3(2.0f, -0.35f, 1.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rueda_der_trasera.RenderModel();

		model = modelCocheBase;
		model = glm::translate(model, glm::vec3(2.0f, -0.6f, -1.3f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rueda_izq_trasera.RenderModel();

		// ==========================================================
		// RENDER NAVE: JERARQUÍA COMPLETA
		// ==========================================================
		// 1. CUERPO DE LA NAVE
		model = modelNaveBase;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave_M.RenderModel();

		// 2. ALA IZQUIERDA (Hija de la Nave)
		glm::mat4 modelAlaIzq = modelNaveBase;
		modelAlaIzq = glm::translate(modelAlaIzq, glm::vec3(0.0f, 0.0f, -0.3f));
		modelAlaIzq = glm::rotate(modelAlaIzq, rotAla * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = modelAlaIzq;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		// 2.1 HÉLICE IZQUIERDA (Hija del Ala Izquierda)
		glm::mat4 modelHeliceIzq = modelAlaIzq;
		modelHeliceIzq = glm::translate(modelHeliceIzq, glm::vec3(-0.5f, 0.0f, 0.0f));

		// ¡EL MATA-BOLAS GIGANTES! Escala la hélice para hacerla chiquita
		modelHeliceIzq = glm::scale(modelHeliceIzq, glm::vec3(0.15f, 0.15f, 0.15f));

		modelHeliceIzq = glm::rotate(modelHeliceIzq, rotHelice * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = modelHeliceIzq;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		// (OJO: Cuando tengas tu .obj de hélice real, ponlo aquí en lugar de Aeolipile_M)
		Aeolipile_M.RenderModel();

		// 3. ALA DERECHA (Hija de la Nave)
		glm::mat4 modelAlaDer = modelNaveBase;
		modelAlaDer = glm::translate(modelAlaDer, glm::vec3(0.0f, 0.0f, 0.3f));
		modelAlaDer = glm::rotate(modelAlaDer, -rotAla * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelAlaDer = glm::scale(modelAlaDer, glm::vec3(1.0f, 1.0f, -1.0f));
		model = modelAlaDer;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		// 3.1 HÉLICE DERECHA (Hija del Ala Derecha)
		glm::mat4 modelHeliceDer = modelAlaDer;
		modelHeliceDer = glm::translate(modelHeliceDer, glm::vec3(-0.5f, 0.0f, 0.0f));

		// ¡EL MATA-BOLAS GIGANTES 2! 
		modelHeliceDer = glm::scale(modelHeliceDer, glm::vec3(0.15f, 0.15f, 0.15f));

		modelHeliceDer = glm::rotate(modelHeliceDer, rotHelice * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = modelHeliceDer;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_M.RenderModel();


		// ala
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -0.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_base_M.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_M.RenderModel();
		//agave
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// --- Texturas con movimiento (Flechas y Números) ---
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		if (toffsetflechau > 1.0) toffsetflechau = 0.0;
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[6]->RenderMesh();

		for (int i = 1; i < 4; i++) {
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[6]->RenderMesh();
		}

		for (int j = 1; j < 5; j++) {
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[6]->RenderMesh();
		}

		toffsetnumerocambiau += 0.25;
		if (toffsetnumerocambiau > 1.0) toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[6]->RenderMesh();

		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Numero1Texture.UseTexture();
		meshList[5]->RenderMesh();

		glDisable(GL_BLEND);
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
// SettingUpOpenGL.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "stdlib.h"
#include <iostream>
#include "SettingUpOpenGL.h"
#include "GL\glut.h"
#include "GL\freeglut.h"
#include <ctime>
#include <shellapi.h> // ?
#include <cmath>
#include <math.h> 

#define M_E 2.71828182845904523536
#define M_LOG2E 1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#define M_1_PI 0.318309886183790671538
#define M_2_PI 0.636619772367581343076
#define M_1_SQRTPI 0.564189583547756286948
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT_2 0.707106781186547524401

//#define _USE_MATH_DEFINES
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

												//OpenGL structs and vars
int g_iWindowWidth = 512;
int g_iWindowHeight = 512;
int g_iGLUTWindowHandle = 0;
float g_fRotate1 = 0.0f;
float g_fRotate2 = 0.0f;
float g_fRotate3 = 0.0f;

//Time variables (ticks)
std::clock_t g_PreviousTicks;
std::clock_t g_CurrentTicks;

struct float2
{
	float2(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}

	float x;
	float y;
};

struct float3
{
	float3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

	float x;
	float y;
	float z;
};

enum ESceneType
{
	ST_Scene1 = 0,
	ST_Scene2,
	ST_Scene3,
	ST_Scene4,
	ST_NumScenes
};
ESceneType g_eCurrentScene = ST_Scene1;

typedef struct OpenGLData {
	HGLRC Rc;	//render context
	GLuint glTexture;	//texture
	GLfloat xrot;	//X rotation 
	GLfloat yrot;	//Y rotation
} GLDATABASE;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//GL functions
void InitGL(int argc, char* argv[]);
void DisplayGL();
void IdleGL();
void KeyboardGL(unsigned char c, int x, int y);
void MouseGL(int button, int state, int x, int y);
void MotionGL(int x, int y);
void ReshapeGL(int w, int h);

//Primitives creation functions
void DrawRectangle(float width, float height);
void DrawCircle(float radius, int numSides = 8);
void DrawTriangle(float2 p1, float2 p2, float2 p3);

void DrawCube(float width, float height, float depth);
void DrawSphere(float radius);
void DrawPyramid(float scale = 1.0f);

// Render a simple scene with 2D primitives
void RenderScene1();
// Render a slightly more complex scene using different colors
void RenderScene2();
// Render a scene with animated transformations
void RenderScene3();
// Render a scene with 3D objects that perform rotations on all 3 axis.
void RenderScene4();

//Cleaning
void Cleanup(int exitCode, bool bExit = true);


void InitGL(int argc, char* argv[])
{
	//std::cout << "Initialise OpenGL..." << std::endl;

	glutInit(&argc, argv);
	int iScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int iScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition((iScreenWidth - g_iWindowWidth) / 2,
		(iScreenHeight - g_iWindowHeight) / 2);
	glutInitWindowSize(g_iWindowWidth, g_iWindowHeight);

	g_iGLUTWindowHandle = glutCreateWindow("OpenGL");

	// Register GLUT callbacks
	glutDisplayFunc(DisplayGL);
	glutIdleFunc(IdleGL);
	glutMouseFunc(MouseGL);
	glutMotionFunc(MotionGL);
	glutKeyboardFunc(KeyboardGL);
	glutReshapeFunc(ReshapeGL);

	// Setup initial GL State
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);
	//std::cout << "Initialise OpenGL: Success!" << std::endl;
}

void Cleanup(int errorCode, bool bExit)
{
	if (g_iGLUTWindowHandle != 0)
	{
		glutDestroyWindow(g_iGLUTWindowHandle);
		g_iGLUTWindowHandle = 0;
	}

	if (bExit)
	{
		exit(errorCode);
	}
}

void DisplayGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (g_eCurrentScene)
	{
	case ST_Scene1:
	{
		RenderScene1();
	}
	break;
	case ST_Scene2:
	{
		RenderScene2();
	}
	break;
	case ST_Scene3:
	{
		RenderScene3();
	}
	break;
	case ST_Scene4:
	{
		RenderScene4();
	}
	break;
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void IdleGL()
{
	// Update our simulation
	g_CurrentTicks = std::clock();
	float deltaTicks = (g_CurrentTicks - g_PreviousTicks);
	g_PreviousTicks = g_CurrentTicks;

	float fDeltaTime = deltaTicks / (float)CLOCKS_PER_SEC;

	// Rate of rotation in (degrees) per second
	const float fRotationRate = 50.0f;

	// Update our rotation parameters
	g_fRotate1 += (fRotationRate * fDeltaTime);
	g_fRotate1 = fmodf(g_fRotate1, 360.0f);

	g_fRotate2 += (fRotationRate * fDeltaTime);
	g_fRotate2 = fmodf(g_fRotate2, 360.0f);

	g_fRotate3 += (fRotationRate * fDeltaTime);
	g_fRotate3 = fmodf(g_fRotate3, 360.0f);

	glutPostRedisplay();
}

void KeyboardGL(unsigned char c, int x, int y)
{
	// Store the current scene so we can test if it has changed later.
	ESceneType currentScene = g_eCurrentScene;

	switch (c)
	{
	case '1':
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);                         // White background
		g_eCurrentScene = ST_Scene1;
	}
	break;
	case '2':
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                         // Black background
		g_eCurrentScene = ST_Scene2;
	}
	break;
	case '3':
	{
		glClearColor(0.27f, 0.27f, 0.27f, 1.0f);                      // Dark-Gray background
		g_eCurrentScene = ST_Scene3;
	}
	break;
	case '4':
	{
		glClearColor(0.73f, 0.73f, 0.73f, 1.0f);                      // Light-Gray background
		g_eCurrentScene = ST_Scene4;
	}
	break;
	case 's':
	case 'S':
	{
		std::cout << "Shade Model: GL_SMOOTH" << std::endl;
		// Switch to smooth shading model
		glShadeModel(GL_SMOOTH);
	}
	break;
	case 'f':
	case 'F':
	{
		std::cout << "Shade Model: GL_FLAT" << std::endl;
		// Switch to flat shading model
		glShadeModel(GL_FLAT);
	}
	break;
	case 'r':
	case 'R':
	{
		std::cout << "Reset Parameters..." << std::endl;
		g_fRotate1 = g_fRotate2 = g_fRotate3 = 0.0f;
	}
	break;
	case '\033': // escape quits
	case '\015': // Enter quits
	case 'Q':    // Q quits
	case 'q':    // q (or escape) quits
	{
		// Cleanup up and quit
		Cleanup(0);
	}
	break;
	}

	if (currentScene != g_eCurrentScene)
	{
		std::cout << "Changed Render Scene: " << (g_eCurrentScene + 1) << std::endl;
	}

	glutPostRedisplay();
}

void MouseGL(int button, int state, int x, int y)
{

}

void MotionGL(int x, int y)
{

}

void ReshapeGL(int w, int h)
{
	std::cout << "ReshapGL( " << w << ", " << h << " );" << std::endl;

	if (h == 0)                                        // Prevent a divide-by-zero error
	{
		h = 1;                                      // Making Height Equal One
	}

	g_iWindowWidth = w;
	g_iWindowHeight = h;

	glViewport(0, 0, g_iWindowWidth, g_iWindowHeight);

	// Setup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)g_iWindowWidth / (GLdouble)g_iWindowHeight, 0.1, 100.0);

	glutPostRedisplay();
}

void DrawTriangle(float2 p1, float2 p2, float2 p3)
{
	glBegin(GL_TRIANGLES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glVertex2f(p3.x, p3.y);
	glEnd();
}

void DrawRectangle(float width, float height)
{
	const float w = width / 2.0f;
	const float h = height / 2.0f;

	glBegin(GL_QUADS);
	glVertex2f(-w, h);   // Top-Left
	glVertex2f(w, h);   // Top-Right
	glVertex2f(w, -h);   // Bottom-Right
	glVertex2f(-w, -h);   // Bottom-Left
	glEnd();

}

void DrawCircle(float radius, int numSides /* = 8 */)
{
	const float step = M_PI / numSides;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0f, 0.0f);
	for (float angle = 0.0f; angle < (2.0f * M_PI); angle += step)
	{
		glVertex2f(radius * sinf(angle), radius * cosf(angle));
	}
	glVertex2f(0.0f, radius); // One more vertex to close the circle
	glEnd();
}

void RenderScene1()
{
	glMatrixMode(GL_MODELVIEW);                                           // Switch to modelview matrix mode
	glLoadIdentity();                                                       // Load the identity matrix

	glTranslatef(-1.5f, 1.0f, -6.0f);                                     // Translate our view matrix back and a bit to the left.
	glColor3f(1.0f, 0.0f, 0.0f);                                          // Set Color to red
	DrawTriangle(float2(0.0f, 1.0f), float2(-1.0f, -1.0f), float2(1.0f, -1.0f));

	glTranslatef(3.0f, 0.0f, 0.0f);                                       // Shift view 3 units to the right
	glColor3f(0.0f, 0.0f, 1.0f);                                          // Set Color to blue
	DrawRectangle(2.0f, 2.0f);

	glTranslatef(-1.5f, -3.0f, 0.0f);                                     // Back to center and lower screen
	glColor3f(1.0f, 1.0f, 0.0f);                                          // Set color to yellow
	DrawCircle(1.0f, 16);
}

void RenderScene2()
{
	glMatrixMode(GL_MODELVIEW);                                           // Switch to modelview matrix mode
	glLoadIdentity();                                                       // Load the identity matrix

	glTranslatef(-1.5f, 1.0f, -6.0f);                                     // Translate back and to the left
																		  // Draw a triangle with different colors on each vertex
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex2f(0.0f, 1.0f);                                           // Top-Center

	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex2f(-1.0f, -1.0f);                                         // Bottom-Left

	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex2f(1.0f, -1.0f);                                          // Bottom-Right
	glEnd();

	glTranslatef(3.0f, 0.0f, 0.0f);                                       // Translate right
																		  // Draw a rectangle with different colors on each vertex
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex2f(-1.0f, 1.0f);                                          // Top-Left

	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex2f(1.0f, 1.0f);                                           // Top-Right

	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex2f(1.0f, -1.0f);                                          // Bottom-Right

	glColor3f(1.0f, 1.0f, 1.0f);                                      // White
	glVertex2f(-1.0f, -1.0f);                                         // Bottom-Left
	glEnd();

	glTranslatef(-1.5f, -3.0f, 0.0f);                                     // Back to center and lower screen

																		  // Draw a circle with blended red/blue vertices.
	const float step = M_PI / 16;
	const float radius = 1.0f;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	for (float angle = 0.0f; angle < (2.0f * M_PI); angle += step)
	{
		float fSin = sinf(angle);
		float fCos = cosf(angle);

		glColor3f((fCos + 1.0f) * 0.5f, (fSin + 1.0f) * 0.5f, 0.0f);
		glVertex2f(radius * fSin, radius * fCos);
	}
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, radius); // One more vertex to close the circle
	glEnd();
}

void RenderScene3()
{
	glMatrixMode(GL_MODELVIEW);                                           // Switch to modelview matrix mode
	glLoadIdentity();                                                       // Load the identity matrix

	glTranslatef(-1.5f, 1.0f, -6.0f);                                     // Translate back and to the left
	glPushMatrix();                                                         // Push the current transformation onto the matrix stack
	glRotatef(g_fRotate1, 0.0f, 0.0f, 1.0f);
	// Draw a triangle with different colors on each vertex
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex2f(0.0f, 1.0f);                                           // Top-Center

	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex2f(-1.0f, -1.0f);                                         // Bottom-Left

	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex2f(1.0f, -1.0f);                                          // Bottom-Right
	glEnd();
	glPopMatrix();

	glTranslatef(3.0f, 0.0f, 0.0f);                                       // Translate right
	glPushMatrix();
	glRotatef(g_fRotate2, 0.0f, 0.0f, 1.0f);
	// Draw a rectangle with different colors on each vertex
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex2f(-1.0f, 1.0f);                                          // Top-Left

	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex2f(1.0f, 1.0f);                                           // Top-Right

	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex2f(1.0f, -1.0f);                                          // Bottom-Right

	glColor3f(1.0f, 1.0f, 1.0f);                                      // White
	glVertex2f(-1.0f, -1.0f);                                         // Bottom-Left
	glEnd();
	glPopMatrix();

	glTranslatef(-1.5f, -3.0f, 0.0f);                                     // Back to center and lower screen
	glPushMatrix();
	glRotatef(g_fRotate3, 0.0f, 0.0f, 1.0f);
	// Draw a circle with blended red/blue vertecies
	const float step = M_PI / 16;
	const float radius = 1.0f;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	for (float angle = 0.0f; angle < (2.0f * M_PI); angle += step)
	{
		float fSin = sinf(angle);
		float fCos = cosf(angle);

		glColor3f((fCos + 1.0f) * 0.5f, (fSin + 1.0f) * 0.5f, 0.0f);
		glVertex2f(radius * fSin, radius * fCos);
	}
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, radius); // One more vertex to close the circle
	glEnd();
	glPopMatrix();
}

void RenderScene4()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	glTranslatef(-1.5f, 1.0f, -6.0f);                                     // Translate back and to the left

	glPushMatrix();                                                         // Push the current modelview matrix on the matrix stack
	glRotatef(g_fRotate1, 1.0f, 1.0f, 1.0f);                               // Rotate on all 3 axis
	glBegin(GL_TRIANGLES);                                                // Draw a pyramid
	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex3f(0.0f, 1.0f, 0.0f);                                     // Top of front face
	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex3f(-1.0f, -1.0f, 1.0f);                                   // Left of front face
	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex3f(1.0f, -1.0f, 1.0f);                                    // Right of front face

	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex3f(0.0f, 1.0f, 0.0f);                                     // Top of right face
	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex3f(1.0f, -1.0f, 1.0f);                                    // Left of right face
	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex3f(1.0f, -1.0f, -1.0f);                                   // Right of right face

	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex3f(0.0f, 1.0f, 0.0f);                                     // Top of back face
	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex3f(1.0f, -1.0f, -1.0f);                                   // Left of back face
	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex3f(-1.0f, -1.0f, -1.0f);                                  // Right of back face

	glColor3f(1.0f, 0.0f, 0.0f);                                      // Red
	glVertex3f(0.0f, 1.0f, 0.0f);                                     // Top of left face
	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex3f(-1.0f, -1.0f, -1.0f);                                  // Left of left face
	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex3f(-1.0f, -1.0f, 1.0f);                                   // Right of left face
	glEnd();

	// Render a quad for the bottom of our pyramid
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex3f(-1.0f, -1.0f, 1.0f);                                   // Left/right of front/left face
	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex3f(1.0f, -1.0f, 1.0f);                                    // Right/left of front/right face
	glColor3f(0.0f, 1.0f, 0.0f);                                      // Green
	glVertex3f(1.0f, -1.0f, -1.0f);                                   // Right/left of right/back face
	glColor3f(0.0f, 0.0f, 1.0f);                                      // Blue
	glVertex3f(-1.0f, -1.0f, -1.0f);                                  // Left/right of right/back face
	glEnd();
	glPopMatrix();

	glTranslatef(3.0f, 0.0f, 0.0f);                                        // Translate right
	glPushMatrix();                                                         // Push the current modelview matrix on the matrix stack
	glRotatef(g_fRotate2, 1.0f, 1.0f, 1.0f);                              // Rotate the primitive on all 3 axis
	glBegin(GL_QUADS);
	// Top face
	glColor3f(0.0f, 1.0f, 0.0f);                                   // Green
	glVertex3f(1.0f, 1.0f, -1.0f);                                   // Top-right of top face
	glVertex3f(-1.0f, 1.0f, -1.0f);                                   // Top-left of top face
	glVertex3f(-1.0f, 1.0f, 1.0f);                                   // Bottom-left of top face
	glVertex3f(1.0f, 1.0f, 1.0f);                                   // Bottom-right of top face

																	// Bottom face
	glColor3f(1.0f, 0.5f, 0.0f);                                  // Orange
	glVertex3f(1.0f, -1.0f, -1.0f);                                  // Top-right of bottom face
	glVertex3f(-1.0f, -1.0f, -1.0f);                                  // Top-left of bottom face
	glVertex3f(-1.0f, -1.0f, 1.0f);                                  // Bottom-left of bottom face
	glVertex3f(1.0f, -1.0f, 1.0f);                                  // Bottom-right of bottom face

																	// Front face
	glColor3f(1.0f, 0.0f, 0.0f);                                  // Red
	glVertex3f(1.0f, 1.0f, 1.0f);                                  // Top-Right of front face
	glVertex3f(-1.0f, 1.0f, 1.0f);                                  // Top-left of front face
	glVertex3f(-1.0f, -1.0f, 1.0f);                                  // Bottom-left of front face
	glVertex3f(1.0f, -1.0f, 1.0f);                                  // Bottom-right of front face

																	// Back face
	glColor3f(1.0f, 1.0f, 0.0f);                                 // Yellow
	glVertex3f(1.0f, -1.0f, -1.0f);                                 // Bottom-Left of back face
	glVertex3f(-1.0f, -1.0f, -1.0f);                                 // Bottom-Right of back face
	glVertex3f(-1.0f, 1.0f, -1.0f);                                 // Top-Right of back face
	glVertex3f(1.0f, 1.0f, -1.0f);                                 // Top-Left of back face

																   // Left face
	glColor3f(0.0f, 0.0f, 1.0f);                                   // Blue
	glVertex3f(-1.0f, 1.0f, 1.0f);                                   // Top-Right of left face
	glVertex3f(-1.0f, 1.0f, -1.0f);                                   // Top-Left of left face
	glVertex3f(-1.0f, -1.0f, -1.0f);                                   // Bottom-Left of left face
	glVertex3f(-1.0f, -1.0f, 1.0f);                                   // Bottom-Right of left face

																	  // Right face
	glColor3f(1.0f, 0.0f, 1.0f);                                   // Violet
	glVertex3f(1.0f, 1.0f, 1.0f);                                   // Top-Right of left face
	glVertex3f(1.0f, 1.0f, -1.0f);                                   // Top-Left of left face
	glVertex3f(1.0f, -1.0f, -1.0f);                                   // Bottom-Left of left face
	glVertex3f(1.0f, -1.0f, 1.0f);                                   // Bottom-Right of left face
	glEnd();
	glPopMatrix();

	glTranslatef(-1.5f, -3.0f, 0.0f);                                     // Back to center and lower screen
	glPushMatrix();
	glRotatef(g_fRotate3, 1.0f, 1.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 0.0f);                                          // Yellow
	glutSolidSphere(1.0f, 16, 16);                                        // Use GLUT to draw a solid sphere
	glScalef(1.01f, 1.01f, 1.01f);
	glColor3f(1.0f, 0.0f, 0.0f);                                          // Red
	glutWireSphere(1.0f, 16, 16);                                         // Use GLUT to draw a wireframe sphere
	glPopMatrix();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	//LPWSTR *argv;
	//int argc;

	//argv = CommandLineToArgvW(lpCmdLine, &argc);

    // TODO: Place code here.

	// Capture the previous time to calculate the delta time on the next frame
	g_PreviousTicks = std::clock();

	InitGL(__argc, __argv);
	glutMainLoop();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SETTINGUPOPENGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SETTINGUPOPENGL));
                                       // Zero y rotation
	MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SETTINGUPOPENGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SETTINGUPOPENGL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

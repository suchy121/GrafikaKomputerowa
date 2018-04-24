#include <GL/glut.h>
#include "glext.h"
#ifndef WIN32
#define GLX_GLXEXT_LEGACY
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif
#include <stdlib.h>
#include <stdio.h>
#include "colors.h"
#include "targa.h"

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

enum
{
	PERSPECTIVE_CORRECTION_FASTEST,
	PERSPECTIVE_CORRECTION_DONT_CARE,
	PERSPECTIVE_CORRECTION_NICEST,
	GENERATE_MIPMAP_FASTEST,
	GENERATE_MIPMAP_DONT_CARE,
	GENERATE_MIPMAP_NICEST,
	FULL_WINDOW,
	ASPECT_1_1,
	EXIT
};

int aspect = FULL_WINDOW;

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;
int button_state = GLUT_UP;
int button_x, button_y;
GLfloat scale = 1.5;
GLuint GROUND, WOOD, ROOF, WINDOW;
GLint GROUND_LIST, WOOD_LIST, ROOF_LIST, WINDOW_LIST;
GLint min_filter = GL_LINEAR_MIPMAP_LINEAR;
GLint perspective_correction_hint = GL_DONT_CARE;
GLint mipmap_generation_hint = GL_DONT_CARE;

void DrawString(GLint x, GLint y, char *string)
{
	glWindowPos2i(x, y);
	int len = strlen(string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
}

void DisplayScene()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -(near + far) / 2);
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);
	glScalef(scale, scale, scale);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, perspective_correction_hint);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, GROUND);
	glPushMatrix();
	glTranslatef(0.0, -1.0, 0.0);
	glCallList(GROUND_LIST);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, WOOD);
	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glCallList(WOOD_LIST);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, ROOF);
	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glCallList(ROOF_LIST);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, WINDOW);
	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glCallList(WINDOW_LIST);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	char string[200];
	GLint var;
	glColor3fv(Black);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &var);
	switch (var)
	{
	case GL_NEAREST_MIPMAP_NEAREST:
		sprintf_s(string, "GL_TEXTURE_MIN_FILTER = GL_NEAREST_MIPMAP_NEAREST");
		break;
	case GL_NEAREST_MIPMAP_LINEAR:
		sprintf_s(string, "GL_TEXTURE_MIN_FILTER = GL_NEAREST_MIPMAP_LINEAR");
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		sprintf_s(string, "GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_NEAREST");
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		sprintf_s(string, "GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR");
		break;
	}
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 17, string);
	glGetIntegerv(GL_PERSPECTIVE_CORRECTION_HINT, &var);
	switch (var)
	{
	case GL_FASTEST:
		sprintf_s(string, "GL_PERSPECTIVE_CORRECTION_HINT = GL_FASTEST");
		break;
	case GL_DONT_CARE:
		sprintf_s(string, "GL_PERSPECTIVE_CORRECTION_HINT = GL_DONT_CARE");
		break;
	case GL_NICEST:
		sprintf_s(string, "GL_PERSPECTIVE_CORRECTION_HINT = GL_NICEST");
		break;
	}
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 33, string);
	glGetIntegerv(GL_GENERATE_MIPMAP_HINT, &var);
	switch (var)
	{
	case GL_FASTEST:
		sprintf_s(string, "GL_GENERATE_MIPMAP_HINT = GL_FASTEST");
		break;
	case GL_DONT_CARE:
		sprintf_s(string, "GL_GENERATE_MIPMAP_HINT = GL_DONT_CARE");
		break;
	case GL_NICEST:
		sprintf_s(string, "GL_GENERATE_MIPMAP_HINT = GL_NICEST");
		break;
	}
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 49, string);
	glFlush();
	glutSwapBuffers();
}

void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (aspect == ASPECT_1_1)
	{
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);
	DisplayScene();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == '+')
		scale += 0.05;
	else
		if (key == '-' && scale > 0.05)
			scale -= 0.05;
	DisplayScene();
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		button_state = state;
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void GenerateTextures()
{
	GLsizei width, height;
	GLenum format, type;
	GLvoid *pixels;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glHint(GL_GENERATE_MIPMAP_HINT, mipmap_generation_hint);

	GLboolean error = load_targa("ground1-2.tga", width, height, format, type, pixels);
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku ground1-2.tga");
		exit(0);
	}
	glGenTextures(1, &GROUND);
	glBindTexture(GL_TEXTURE_2D, GROUND);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);
	delete[](unsigned char*)pixels;

	error = load_targa("wall_wood_verti_color.tga", width, height, format, type, pixels);
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku wall_wood_verti_color.tga");
		exit(0);
	}
	glGenTextures(1, &WOOD);
	glBindTexture(GL_TEXTURE_2D, WOOD);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);
	delete[](unsigned char*)pixels;

	error = load_targa("roof_old_rectangle_color.tga", width, height, format, type, pixels);
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku roof_old_rectangle_color.tga");
		exit(0);
	}
	glGenTextures(1, &ROOF);
	glBindTexture(GL_TEXTURE_2D, ROOF);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);
	delete[](unsigned char*)pixels;

	error = load_targa("okno.tga", width, height, format, type, pixels);
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku okno.tga");
		exit(0);
	}
	glGenTextures(1, &WINDOW);
	glBindTexture(GL_TEXTURE_2D, WINDOW);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);
	delete[](unsigned char*)pixels;
}

void Menu(int value)
{
	switch (value)
	{
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_LINEAR:
		min_filter = value;
		DisplayScene();
		break;
	case PERSPECTIVE_CORRECTION_FASTEST:
		perspective_correction_hint = GL_FASTEST;
		DisplayScene();
		break;
	case PERSPECTIVE_CORRECTION_DONT_CARE:
		perspective_correction_hint = GL_DONT_CARE;
		DisplayScene();
		break;
	case PERSPECTIVE_CORRECTION_NICEST:
		perspective_correction_hint = GL_NICEST;
		DisplayScene();
		break;
	case GENERATE_MIPMAP_FASTEST:
		GenerateTextures();
		mipmap_generation_hint = GL_FASTEST;
		DisplayScene();
		break;
	case GENERATE_MIPMAP_DONT_CARE:
		GenerateTextures();
		mipmap_generation_hint = GL_DONT_CARE;
		DisplayScene();
		break;
	case GENERATE_MIPMAP_NICEST:
		GenerateTextures();
		mipmap_generation_hint = GL_NICEST;
		DisplayScene();
		break;
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;
	case EXIT:
		exit(0);
	}
}

void GenerateDisplayLists()
{
	GROUND_LIST = glGenLists(1);

	glNewList(GROUND_LIST, GL_COMPILE);

	glBegin(GL_QUADS);
	glTexCoord2f(16.0, 0.0);
	glVertex3f(-8.0, 0.0, -8.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-8.0, 0.0, 8.0);
	glTexCoord2f(0.0, 16.0);
	glVertex3f(8.0, 0.0, 8.0);
	glTexCoord2f(16.0, 16.0);
	glVertex3f(8.0, 0.0, -8.0);
	glEnd();

	glEndList();

	WOOD_LIST = glGenLists(1);

	glNewList(WOOD_LIST, GL_COMPILE);

	glBegin(GL_TRIANGLES);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(1.0, -1.0, -1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(-1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, -1.0, -1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1.0, 2.0);
	glVertex3f(0.0, 2.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(1.0, 2.0);
	glVertex3f(0.0, 2.0, -1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(1.0, 1.0, -1.0);
	glEnd();

	glEndList();

	ROOF_LIST = glGenLists(1);

	glNewList(ROOF_LIST, GL_COMPILE);

	glBegin(GL_QUADS);

	glTexCoord2f(2.0, 2.0);
	glVertex3f(0.0, 2.0, 1.2);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(0.0, 2.0, -1.2);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, 1.0, -1.2);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-1.0, 1.0, 1.2);

	glTexCoord2f(2.0, 2.0);
	glVertex3f(0.0, 2.0, -1.2);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(0.0, 2.0, 1.2);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, 1.0, 1.2);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, -1.2);

	glEnd();

	glEndList();

	WINDOW_LIST = glGenLists(1);

	glNewList(WINDOW_LIST, GL_COMPILE);

	glBegin(GL_QUADS);
	/////////////////////////////////////////////////////////
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.9, 0.9, 1.01);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1, 0.9, 1.01);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.1, 1.01);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.9, 0.1, 1.01);
	/////////////////////////////////////////////////////////
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.9, 0.9, 1.01);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.9, 1.01);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.1, 1.01);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.9, 0.1, 1.01);
	/////////////////////////////////////////////////////////
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.01, 0.9, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.01, 0.9, -0.3);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.01, 0.1, -0.3);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(1.01, 0.1, 0.5);
	/////////////////////////////////////////////////////////
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.9, 0.9, -1.01);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1, 0.9, -1.01);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.1, -1.01);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.9, 0.1, -1.01);
	/////////////////////////////////////////////////////////
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.9, 0.9, -1.01);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.9, -1.01);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.1, -1.01);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.9, 0.1, -1.01);
	/////////////////////////////////////////////////////////
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.01, 0.9, -0.9);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-1.01, 0.9, -0.1);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-1.01, 0.1, -0.1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.01, 0.1, -0.9);
	/////////////////////////////////////////////////////////
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.01, 0.9, 0.9);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-1.01, 0.9, 0.1);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-1.01, 0.1, 0.1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.01, 0.1, 0.9);

	glEnd();

	glEndList();
}

void ExtensionSetup()
{
	const char *version = (char*)glGetString(GL_VERSION);
	int major = 0, minor = 0;
	if (sscanf_s(version, "%d.%d", &major, &minor) != 2)
	{
#ifdef WIN32
		printf("Błędny format wersji OpenGL\n");
#else
		printf("Bledny format wersji OpenGL\n");
#endif
		exit(0);
	}
	if (!(major > 1 || minor >= 4) &&
		!glutExtensionSupported("GL_SGIS_generate_mipmap"))
	{
		printf("Brak rozszerzenia GL_SGIS_generate_mipmap!\n");
		exit(0);
	}
	if (major > 1 || minor >= 4)
	{
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
	}
	else
		if (glutExtensionSupported("GL_ARB_window_pos"))
		{
			glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress
			("glWindowPos2iARB");
		}
		else
		{
			printf("Brak rozszerzenia ARB_window_pos!\n");
			exit(0);
		}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Mipmapy 2D");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	int MenuMinFilter = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR);
	int PerspectiveCorrectionHint = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_FASTEST", PERSPECTIVE_CORRECTION_FASTEST);
	glutAddMenuEntry("GL_DONT_CARE", PERSPECTIVE_CORRECTION_DONT_CARE);
	glutAddMenuEntry("GL_NICEST", PERSPECTIVE_CORRECTION_NICEST);
	int GenerateMipmapHint = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_FASTEST", GENERATE_MIPMAP_FASTEST);
	glutAddMenuEntry("GL_DONT_CARE", GENERATE_MIPMAP_DONT_CARE);
	glutAddMenuEntry("GL_NICEST", GENERATE_MIPMAP_NICEST);
	int MenuAspect = glutCreateMenu(Menu);
#ifdef WIN32
	glutAddMenuEntry("Aspekt obrazu - całe okno", FULL_WINDOW);
#else
	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
#endif
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutCreateMenu(Menu);
#ifdef WIN32
	glutAddSubMenu("Filtr pomniejszający", MenuMinFilter);
	glutAddSubMenu("GL_PERSPECTIVE_CORRECTION_HINT", PerspectiveCorrectionHint);
	glutAddSubMenu("GL_GENERATE_MIPMAP_HINT", GenerateMipmapHint);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjście", EXIT);
#else
	glutAddSubMenu("Filtr pomniejszajacy", MenuMinFilter);
	glutAddSubMenu("GL_PERSPECTIVE_CORRECTION_HINT", PerspectiveCorrectionHint);
	glutAddSubMenu("GL_GENERATE_MIPMAP_HINT", GenerateMipmapHint);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	GenerateTextures();
	ExtensionSetup();
	GenerateDisplayLists();
	glutMainLoop();
	return 0;
}
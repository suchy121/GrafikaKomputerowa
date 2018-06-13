
#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"
#include "gltools_extracted.h"
#include "VectorMath.cpp"
#include <array>
#include <vector>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>

#define GL_PI 3.14159265359
#define NUM_PARTICLES 1000 // Number of particles 
#define NUM_DEBRIS 70 // Number of debris
// GLUT menu entries
#define PAUSE 0
#define NORMALIZE_SPEED 1
#define QUIT 2

// A particle

struct particleData
{
	float position[3];
	float speed[3];
	float color[3];
};

typedef struct particleData particleData;

// A piece of debris

struct debrisData
{
	float position[3];
	float speed[3];
	float orientation[3]; // Rotation angles around x, y, and z axes
	float orientationSpeed[3];
	float color[3];
	float scale[3];
};

typedef struct debrisData debrisData;

// Globals

particleData particles[NUM_PARTICLES];
debrisData debris[NUM_DEBRIS];

int fuel = 0; // "fuel" of the explosion
float angle = 0.0; // camera rotation angle

				   // Light sources and material

GLfloat light0Amb[4] = { 1.0, 0.6, 0.2, 1.0 };
GLfloat light0Dif[4] = { 1.0, 0.6, 0.2, 1.0 };
GLfloat light0Spec[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light0Pos[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light1Amb[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light1Dif[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light1Spec[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light1Pos[4] = { 0.0, 5.0, 5.0, 0.0 };
GLfloat materialAmb[4] = { 0.25, 0.22, 0.26, 1.0 };
GLfloat materialDif[4] = { 0.63, 0.57, 0.60, 1.0 };
GLfloat materialSpec[4] = { 0.99, 0.91, 0.81, 1.0 };
GLfloat materialShininess = 27.8;

int wantNormalize = 0; // Speed vector normalization flag
int wantPause = 0; // Pause flag

void newSpeed(float dest[3])
{
	float x;
	float y;
	float z;
	float len;

	x = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
	y = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
	z = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;

	//Normalizing the speed vectors gives a "fireball" effect

	if (wantNormalize)
	{
		len = sqrt(x * x + y * y + z * z);
		if (len)
		{
			x = x / len;
			y = y / len;
			z = z / len;
		}
	}

	dest[0] = x;
	dest[1] = y;
	dest[2] = z;
}

///Create a new explosion.

void newExplosion(void)
{
	for (size_t i = 0; i < NUM_PARTICLES; i++)
	{
		particles[i].position[0] = 0.0;
		particles[i].position[1] = 0.0;
		particles[i].position[2] = 0.0;

		particles[i].color[0] = 1.0;
		particles[i].color[1] = 1.0;
		particles[i].color[2] = 0.5;
		newSpeed(particles[i].speed);
	}

	for (size_t i = 0; i < NUM_DEBRIS; i++)
	{
		debris[i].position[0] = 0.0;
		debris[i].position[1] = 0.0;
		debris[i].position[2] = 0.0;

		debris[i].orientation[0] = 0.0;
		debris[i].orientation[1] = 0.0;
		debris[i].orientation[2] = 0.0;

		debris[i].color[0] = 0.7;
		debris[i].color[1] = 0.7;
		debris[i].color[2] = 0.7;

		debris[i].scale[0] = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
		debris[i].scale[1] = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
		debris[i].scale[2] = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;

		newSpeed(debris[i].speed);
		newSpeed(debris[i].orientationSpeed);
	}
	fuel = 100;
}

///Create object

void createObject()
{
	GLTVector3 centralVertex{ 0.0f, -1.0f, 0.0f };
	GLTVector3 topVertex{ 0.0f, 2.0f, 0.0f };
	GLTVector3 bottomVertices[6];

	for (size_t i = 0; i < 6; i++)
	{
		GLfloat angle = 2.0 * GL_PI * i / 6.0f;
		GLfloat x = 2 * sin(angle);
		GLfloat y = 2 * cos(angle);

		bottomVertices[i][0] = x;
		bottomVertices[i][1] = -1.0f;
		bottomVertices[i][2] = y;
	}

	glBegin(GL_POLYGON);
	//Podstawa
	for (size_t i = 0; i <6; i++)
	{
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(centralVertex);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(bottomVertices[(i + 1) % 6]);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(bottomVertices[i]);
	}
	glEnd();
	glBegin(GL_TRIANGLES);
	//Sciany boczne
	for (size_t i = 0; i <6; i++)
	{
		GLTVector3 normal;
		gltGetNormalVector(bottomVertices[i], bottomVertices[(i + 1) % 6], topVertex, normal);
		glNormal3fv(normal);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(bottomVertices[i]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(bottomVertices[(i + 1) % 6]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(topVertex);
	}

	glEnd();
}

GLfloat fog_density = 0.5;
GLint fog_hint = GL_DONT_CARE;
GLfloat fog_mode = GL_EXP;

void createFog()
{
	glEnable(GL_FOG);
	glHint(GL_FOG_HINT, fog_hint);
	glFogfv(GL_FOG_COLOR, Gray);
	glFogf(GL_FOG_DENSITY, fog_density);
	glFogf(GL_FOG_MODE, fog_mode);
}

///Display object

void display(void)
{
	createFog();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//Place the camera
	glTranslatef(0.0, 0.0, -10.0);
	glRotatef(angle, 0.0, 1.0, 0.0);
	//If no explosion, draw cube
	if (fuel == 0)
	{
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glutSolidCone(1, 2, 6, 6);
		//createObject();
	}

	if (fuel > 0)
	{
		glPushMatrix();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_POINTS);
		for (size_t i = 0; i < NUM_PARTICLES; i++)
		{
			glColor3fv(particles[i].color);
			glVertex3fv(particles[i].position);
		}

		glEnd();
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glNormal3f(0.0, 0.0, 1.0);

		for (size_t i = 0; i < NUM_DEBRIS; i++)
		{
			glColor3fv(debris[i].color);
			glPushMatrix();
			glTranslatef(debris[i].position[0], debris[i].position[1], debris[i].position[2]);
			glRotatef(debris[i].orientation[0], 1.0, 0.0, 0.0);
			glRotatef(debris[i].orientation[1], 0.0, 1.0, 0.0);
			glRotatef(debris[i].orientation[2], 0.0, 0.0, 1.0);
			glScalef(debris[i].scale[0], debris[i].scale[1], debris[i].scale[2]);
			glBegin(GL_TRIANGLES);
			glVertex3f(0.0, 0.5, 0.0);
			glVertex3f(-0.25, 0.0, 0.0);
			glVertex3f(0.25, 0.0, 0.0);
			glEnd();
			glPopMatrix();
		}
	}

	glutSwapBuffers();

}

///Keyboard callback.

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '+':
		fog_density += 0.01;
		break;
	case '-':
		fog_density -= 0.01;
		break;
	case ' ':
		newExplosion();
		break;

	case 27:
		exit(0);
		break;

	case 'p':
		wantPause = 1 - wantPause;
		break;
	}
}

///Update animation variables.

void idle(void)
{
	if (!wantPause)
	{
		if (fuel > 0)
		{
			for (size_t i = 0; i < NUM_PARTICLES; i++)
			{
				particles[i].position[0] += particles[i].speed[0] * 0.2;
				particles[i].position[1] += particles[i].speed[1] * 0.2;
				particles[i].position[2] += particles[i].speed[2] * 0.2;

				particles[i].color[0] -= 1.0 / 500.0;
				if (particles[i].color[0] < 0.0)
				{
					particles[i].color[0] = 0.0;
				}
				particles[i].color[1] -= 1.0 / 100.0;
				if (particles[i].color[1] < 0.0)
				{
					particles[i].color[1] = 0.0;
				}

				particles[i].color[2] -= 1.0 / 50.0;
				if (particles[i].color[2] < 0.0)
				{
					particles[i].color[2] = 0.0;
				}
			}

			for (size_t i = 0; i < NUM_DEBRIS; i++)
			{
				debris[i].position[0] += debris[i].speed[0] * 0.1;
				debris[i].position[1] += debris[i].speed[1] * 0.1;
				debris[i].position[2] += debris[i].speed[2] * 0.1;

				debris[i].orientation[0] += debris[i].orientationSpeed[0] * 10;
				debris[i].orientation[1] += debris[i].orientationSpeed[1] * 10;
				debris[i].orientation[2] += debris[i].orientationSpeed[2] * 10;
			}
			--fuel;
		}
		angle += 0.3; //Always continue to rotate the camera
	}
	glutPostRedisplay();
}

///Window reshape callback.

void reshape(int w, int h)
{
	glViewport(0.0, 0.0, (GLfloat)w, (GLfloat)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

///Menu selection callback.
void menuSelect(int value)
{
	switch (value)
	{
	case PAUSE:
		wantPause = 1 - wantPause;
		break;

	case NORMALIZE_SPEED:
		wantNormalize = 1 - wantNormalize;
		break;

	case QUIT:
		exit(0);
		break;
	}
}

/// Main function
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Explosion demo");

	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	srand(time(NULL));
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Spec);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1Amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Dif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1Spec);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

	glEnable(GL_NORMALIZE);
	glutCreateMenu(menuSelect);
	glutAddMenuEntry("Pause", PAUSE);
	glutAddMenuEntry("Toggle normalized speed vectors", NORMALIZE_SPEED);
	glutAddMenuEntry("Quit", QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();

	return 0;
}
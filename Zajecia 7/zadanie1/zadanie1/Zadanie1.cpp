#include <GL/glut.h>
#include <cstdlib>
#include "colors.h"
#include "gltools_extracted.h"
#include "VectorMath.cpp"
#include <array>
#include <vector>

const GLint N = 10;

// rozmiary bryły obcinania
const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 2.0;
const GLdouble far_ = 7.0;
// stałe do obsługi menu podręcznego
enum
{
	CUTTING_PLANE, // płaszczyzna przekroju
	POLYGON_OFFSET, // przesunięcie wartości głębi
	EXIT // wyjście
};
// kąt obrotu kuli
GLfloat angle = 0.0;
// kąty obrotu sześcianu
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;
// wskaŸnik rysowania płaszczyzna przekroju
bool cutting_plane = true;
// wskaŸnik przesunięcia wartości głębi
bool polygon_offset = true;
// wskaŸnik naciśnięcia lewego przycisku myszki
int button_state = GLUT_UP;
// położenie kursora myszki
int button_x, button_y;


void drawPyramid(GLuint n, GLfloat size, GLenum type, bool drawBase)
{
	GLTVector3 topVertice{ 0.0f, +size, 0.0f };
	GLTVector3 baseCenter{ 0.0f, -size / 2.0, 0.0f };

	std::vector<GLTVector3> bottomVertices{ n };

	for (int i = 0; i < n; ++i)
	{
		const GLfloat angle = (GLfloat)i / n * 3.1415926 * 2.0;

		bottomVertices[i][0] = size * sin(angle);
		bottomVertices[i][1] = -size / 2;
		bottomVertices[i][2] = size * cos(angle);
	}

	if (drawBase)
	{
		for (int i = 0; i <= n; ++i)
		{
			glBegin(type);
			glNormal3f(0.0f, -1.0f, 0.0f);
			glVertex3fv(baseCenter);
			glVertex3fv(bottomVertices[(i + 1) % n]);
			glVertex3fv(bottomVertices[i % n]);
			glEnd();
		}
	}
	for (int i = 0; i <= n; ++i)
	{
		glBegin(type);
		GLTVector3 normal;
		gltGetNormalVector(bottomVertices[i % n], bottomVertices[(i + 1) % n], topVertice, normal);
		glNormal3fv(normal);

		glVertex3fv(bottomVertices[i % n]);
		glVertex3fv(bottomVertices[(i + 1) % n]);
		glVertex3fv(topVertice);

		glEnd();
	}
}

void drawSolidPyramid(GLint n, GLdouble size)
{
	drawPyramid(n, size, GL_TRIANGLES, true);
}

void drawWirePyramid(GLint n, GLdouble size)
{
	drawPyramid(n, size, GL_LINE_LOOP, false);
}

// funkcja generująca scenę 3D
void Display()
{
	// kolor tła - zawartośæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);
	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);
	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();
	// przesunięcie układu współrzędnych sześcianu do środka bryły odcinania
	glTranslatef(0, 0, -(near_ + far_) / 2);
	// obroty sześcianu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);
	// niewielkie powiększenie sześcianu
	//glScalef(1.15, 1.15, 1.15);
	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);
	// najpierw rysujemy kulę obracającą się wewnątrz sześcianu;
	// z uwagi na celowy brak efektów oświetlenia, obrót kuli
	// podkreśla druga kula w wersji "szkieletowej"
	glPushMatrix();
	angle += 0.2;
	glRotatef(angle, 1.0, 1.0, 0.0);
	glColor3fv(Yellow);
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	glutSolidSphere(0.5, 10, 10);
	glColor3fv(Black);
	glutWireSphere(0.5, 10, 10);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);
	glPopMatrix();


	const GLfloat a = 0.5;

	// w drugiej kolejności rysujemy wnętrze sześcianu;
	// rysowane są tylko przednie strony wewnętrznych ścian
	// no i nie jest rysowana ściana przednia, w której będzie otwór
	glEnable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glColor3fv(Red);
	glVertex3f(-a, -2 * a, -a);
	glVertex3f(a, -2 * a, -a);
	glVertex3f(a, a, -a);
	glVertex3f(-a, a, -a);
	glColor3fv(Magenta);
	glVertex3f(-a, a, a);
	glVertex3f(a, a, a);
	glVertex3f(a, -2 * a, a);
	glVertex3f(-a, -2 * a, a);
	glColor3fv(Lime);
	glVertex3f(-a, a, a);
	glVertex3f(-a, -2 * a, a);
	glVertex3f(-a, -2 * a, -a);
	glVertex3f(-a, a, -a);
	glColor3fv(Cyan);
	glVertex3f(a, a, a);
	glVertex3f(-a, a, a);
	glVertex3f(-a, a, -a);
	glVertex3f(a, a, -a);
	glColor3fv(Green);
	glVertex3f(a, -2 * a, a);
	glVertex3f(a, a, a);
	glVertex3f(a, a, -a);
	glVertex3f(a, -2 * a, -a);
	glColor3fv(Cyan);
	glVertex3f(-a, -2 * a, a);
	glVertex3f(a, -2 * a, a);
	glVertex3f(a, -2 * a, -a);
	glVertex3f(-a, -2 * a, -a);
	glEnd();
	glDisable(GL_CULL_FACE);
	// rysowanie płaszczyzny otworu w sześcianie
	if (cutting_plane)
	{
		// wyłączenie rysowania w buforze kolorów
		glDrawBuffer(GL_NONE);
		// rysowanie kwadratu częściowo odsłaniającego wnętrze sześcianu
		// (kwadrat jest położony o 0,001 jednostki nad bokiem sześcianu)
		glBegin(GL_QUADS);
		glVertex3f(-0.5, -1.001, -0.5);
		glVertex3f(0.5, -1.001, -0.5);
		glVertex3f(0.5, -1.001, 0.5);
		glVertex3f(-0.5, -1.001, 0.5);
		glEnd();
		// włączenie rysowania w buforze kolorów
		glDrawBuffer(GL_BACK);
	}
	// właściwy sześcian z obramowaniem, którego rysowanie wymusza brak oświetlenia
	glColor3fv(Blue);
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 1.0);
	drawSolidPyramid(N, 2.0);
	glColor3fv(Black);
	drawWirePyramid(N, 2.0);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);
	// skierowanie poleceñ do wykonania
	glFlush();
	// zamiana buforów koloru
	glutSwapBuffers();
}
// zmiana wielkości okna
void Reshape(int width, int height)
{
	// obszar renderingu - całe okno
	glViewport(0, 0, width, height);
	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);
	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();
	// parametry bryły obcinania
	glFrustum(left, right, bottom, top, near_, far_);
	// generowanie sceny 3D
	Display();
}
// obsługa klawiszy funkcyjnych i klawiszy kursora
void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		rotatey -= 1;
		break;
		// kursor w górę
	case GLUT_KEY_UP:
		rotatex -= 1;
		break;
		// kursor w prawo
	case GLUT_KEY_RIGHT:
		rotatey += 1;
		break;
		// kursor w dół
	case GLUT_KEY_DOWN:
		rotatex += 1;
		break;
	}
	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}
// obsługa przycisków myszki
void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiętanie stanu lewego przycisku myszki
		button_state = state;
		// zapamiętanie położenia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}
// obsługa ruchu kursora myszki
void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}
// obsługa menu podręcznego
void Menu(int value)
{
	switch (value)
	{
		// płaszczyzna przekroju
	case CUTTING_PLANE:
		cutting_plane = !cutting_plane;
		Display();
		break;
		// przesunięcie wartości głębi
	case POLYGON_OFFSET:
		polygon_offset = !polygon_offset;
		Display();
		break;
		// wyjście
	case EXIT:
		exit(0);
	}
}
int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);
	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);
	// utworzenie głównego okna programu
	glutCreateWindow("Z-bufor");
	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(Display);
	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);
	// dołączenie funkcji obsługi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);
	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);
	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);
	// utworzenie menu podręcznego
	glutCreateMenu(Menu);
	// menu główne
	glutCreateMenu(Menu);
#ifdef WIN32
	glutAddMenuEntry("Płaszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
	glutAddMenuEntry("Przesunięcie wartości głębi: włącz/wyłącz", POLYGON_OFFSET);
	glutAddMenuEntry("Wyjście", EXIT);
#else
	glutAddMenuEntry("Plaszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
	glutAddMenuEntry("Przesuniecie wartosci glebi: wlacz/wylacz", POLYGON_OFFSET);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif
	// określenie przycisku myszki obsługującej menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	// dołączenie funkcji wywoływanej podczas "bezczynności" systemu
	glutIdleFunc(Display);
	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();
	return 0;
}	
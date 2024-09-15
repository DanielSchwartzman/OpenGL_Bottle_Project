#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <iostream>
#include "Bubble.h"

const double PI = 3.14;
const int HEIGHT = 600;
const int WIDTH = 600;

double offset = 0;
double eyex = 0, eyey = 5, eyez = 20;
double pitch = 0;
bool slider_is_moving = false;
double oldY;
double colaLevel = 0;
int mofaim=0;
Bubble bubbles[100];

float glassAmb[4] = { 0.3,0.3,0.3,0.2 };
float glassDiff[4] = { 0.6,0.6,0.6,0.2 };
float glassSpec[4] = { 0.5,0.5,0.5,0.2 };

float colaAmb[4] = { 0.1,0.1,0.1,0.8 };
float colaDiff[4] = { 0.1,0.1,0.1,0.8 };
float colaSpec[4] = { 0.5,0.5,0.5,0.8 };

float bubAmb[4] = { 0.3,0.3,0.3,0.8 };
float bubDiff[4] = { 0.6,0.6,0.6,0.8 };
float bubSpec[4] = { 0.5,0.5,0.5,0.8 };

unsigned int letters;

void SetText()
{
	HDC hdc = wglGetCurrentDC();
	HFONT hf;
	GLYPHMETRICSFLOAT gm[128];
	LOGFONT lf;

	lf.lfHeight = 20;
	lf.lfWidth = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = true;
	lf.lfUnderline = false;
	lf.lfOrientation = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfPitchAndFamily = FF_MODERN;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfEscapement = 0;

	strcpy((char*)lf.lfFaceName, "Arial");

	hf = CreateFontIndirect(&lf);

	SelectObject(hdc, hf);

	letters = glGenLists(128);

	wglUseFontOutlines(hdc, 0, 128, letters,
		0, 0.3, WGL_FONT_POLYGONS, gm);
	DeleteObject(hf);
}

void DrawTexts(char* text)
{
	glListBase(letters);
	glCallLists(1, GL_UNSIGNED_BYTE, text);
}


void init()
{
	int i, j;

	srand(time(0));

	//  R   G    B
	glClearColor(0.7, 0.8, 1, 0);// color of window background

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	SetText();
}

void DrawAxes()
{
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(0, 0.1, 0);
	glVertex3d(15, 0.1, 0);

	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 8, 0);

	glColor3d(0, 0, 1);
	glVertex3d(0, 0.1, 0);
	glVertex3d(0, 0.1, 15);
	glEnd();
	glLineWidth(1);
}

void DrawCylinder(int num_pts, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / num_pts;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glNormal3d(topr * sin(alpha), bottomr * (bottomr - topr), topr * cos(alpha));
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // 1
		glNormal3d(topr * sin(alpha + teta), bottomr * (bottomr - topr), topr * cos(alpha + teta));
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // 2
		glNormal3d(bottomr * sin(alpha + teta), bottomr * (bottomr - topr), bottomr * cos(alpha + teta));
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // 3
		glNormal3d(bottomr * sin(alpha), bottomr * (bottomr - topr), bottomr * cos(alpha));
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // 4
		glEnd();
	}
}

void DrawSphere(int num_pts, int num_slices)
{
	double beta, delta = PI / num_slices;

	for (beta = -PI / 2; beta <= PI / 2; beta += delta)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCylinder(num_pts, cos(beta + delta), cos(beta));
		glPopMatrix();
	}
}

void DrawApple(int num_pts, int num_slices)
{
	double beta, delta = PI / num_slices;

	for (beta = 0; beta <= 2 * PI; beta += delta)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCylinder(num_pts, 1 + cos(beta + delta), 1 + cos(beta));
		glPopMatrix();
	}
}

void DrawPkak()
{
	double alpha, teta = 2 * PI / 30;
	glPushMatrix();
	glBegin(GL_POLYGON);
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glVertex3d(cos(alpha), 1, sin(alpha));
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	DrawCylinder(30, 1, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, -1, 0);
	glBegin(GL_POLYGON);
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glVertex3d(cos(alpha), 1, sin(alpha));
	}
	glEnd();
	glPopMatrix();
}

void DrawBottleUpperBody()
{
	glPushMatrix();
	DrawCylinder(30, 0.5, 1.4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, -2, 0);
	glScaled(1, 2, 1);
	DrawCylinder(30, 1.4, 1.6);
	glPopMatrix();
}

void DrawBottleLowerBody()
{
	double alpha, teta = 2 * PI / 30;

	glPushMatrix();
	glScaled(1, 2, 1);
	DrawCylinder(30, 1.6, 1.4);
	glPopMatrix();

	glPushMatrix();
	glScaled(1, -2, 1);
	DrawCylinder(30, 1.6, 1.4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, -3, 0);
	DrawCylinder(30, 1.6, 2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, -4, 0);
	glScaled(2, 1, 2);
	glBegin(GL_POLYGON);
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glVertex3d(cos(alpha), 1, sin(alpha));
	}
	glEnd();
	glPopMatrix();
}

void DrawBottleFeet()//asher maspik
{
	double alpha, teta = 2 * PI / 5;
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glPushMatrix();
		glTranslated(cos(alpha), 0, sin(alpha));
		glScaled(0.92, 0.92, 0.92);
		DrawSphere(30, 30);
		glPopMatrix();

	}
}

void DrawBakbuk()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colaAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colaDiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colaSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96);
	char text[100] = "Afeka";
	glPushMatrix();
	glRotated(120, 0, 1, 0);
	for (int i = 0; i < strlen(text); i++)
	{
		glPushMatrix();
		if (i <= 1)
			glRotated(30 * i, 0, 1, 0);
		else
			glRotated(25 * i, 0, 1, 0);
		glTranslated(0, -2, -1.5);
		glRotated(-180, 0, 1, 0);
		DrawTexts(text + i);
		glPopMatrix();
	}
	glPopMatrix();

	glColor3d(0.5, 0.5, 0.5);
	glPushMatrix();
	glTranslated(0, -5, 0);
	DrawBottleLowerBody();
	glPopMatrix();

	glColor3d(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslated(0, -8, 0);
	DrawBottleFeet();
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glassAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glassDiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96);
	glColor3d(0.8, 0.2, 0.2);
	glPushMatrix();
	glScaled(0.85, 0.85, 0.85);
	DrawPkak();
	glPopMatrix();

	glColor3d(0.6, 0.6, 0.6);
	glPushMatrix();
	glTranslated(0, -1, 0);
	DrawBottleUpperBody();
	glPopMatrix();
}

void drawBubbles()
{
	if ((mofaim++ % 25) == 0) 
	{
		double alpha, teta = 2 * PI / 100;
		int i = 0;
		for (alpha = 0; alpha < 2 * PI; alpha += teta)
		{
			bubbles[i].x = (rand() % 1800) / 1000.0 * cos(alpha);
			bubbles[i].z = (rand() % 18000) / 10000.0 * sin(alpha);
			if (colaLevel > 1) 
				bubbles[i++].y = -1 * (rand() % 10000) / 10000.0;
			else 
				bubbles[i++].y = 0;
		}
	}
	glColor3d(0, 0, 0);
	glTranslated(5, colaLevel-9.9, 0);
	for (int j = 0; j < 100; j++)
	{
		glPushMatrix();
		glTranslated(bubbles[j].x, bubbles[j].y, bubbles[j].z);
		glScaled(0.125, 0.125, 0.125);
		DrawSphere(30,30);
		glPopMatrix();
	}
}

void drawColaInGlass() 
{
	double alpha, teta = 2 * PI / 30;
	glColor4d(0, 0, 0,0.5);
	glPushMatrix();
	glTranslated(5, -10, 0);
	glScaled(1,colaLevel, 1);
	DrawCylinder(30, 1.9, 1.9);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(5, colaLevel-11, 0);
	glBegin(GL_POLYGON);
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glVertex3d(1.9 * cos(alpha), 1, 1.9 * sin(alpha));
	}
	glEnd();
	glPopMatrix();
}

void Drawkos() //emek
{
	double alpha, teta = 2 * PI / 30;
	glColor4d(0.5, 0.5, 0.5,0.2);
	glPushMatrix();
	glTranslated(5, -10, 0);
	glScaled(1, 4, 1);
	DrawCylinder(30, 2, 2);
	glPopMatrix();

	glPushMatrix();
	glColor4d(0.5, 0.5, 0.5,0.2);
	glTranslated(5, -11, 0);
	glBegin(GL_POLYGON);
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glVertex3d(2 * cos(alpha), 1, 2 * sin(alpha));
	}
	glEnd();
	glPopMatrix();
}

void DrawCola()
{
	glPushMatrix();
	glColor4d(0, 0, 0,0.75);
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	for (double i = 0; i < 5.75; i += 0.01)
	{
		double y = -0.3 * i * i ;
		glVertex3d(i, y, 0);
	}
	glEnd();
	glLineWidth(1);
	glPopMatrix();

	if(colaLevel < 3)
		colaLevel += 0.01;
}

void DrawSlider()
{
	glColor3d(1, 1, 0);
	// background
	glBegin(GL_POLYGON);
	glVertex2d(0, 0);
	glVertex2d(0, 100);
	glVertex2d(100, 100);
	glVertex2d(100, 0);
	glEnd();

	glLineWidth(3);
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(50, 0);
	glVertex2d(50, 100);
	glEnd();
	glLineWidth(1);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	glPushMatrix();
	glTranslated(0, pitch * 180 / PI -50, 0); // move in dgrees
	// slider button
	glColor3d(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glVertex2d(40, 40);
	glVertex2d(40, 60);
	glVertex2d(60, 60);
	glVertex2d(60, 40);
	glEnd();

	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(40, 50);
	glVertex2d(60, 50);
	glEnd();

	glPopMatrix();
}

void keyboard(unsigned char key, int x, int y)
{
	double s;
	double c;
	switch (key)
	{
	case 'w':
		eyey += 1;
		break;
	case 's':
		eyey -= 1;
		break;
	case 'a':
		s = sin(0.05);
		c = cos(0.05);
		eyex = eyex * c - eyez * s;
		eyez = eyex * s + eyez * c;
		break;
	case 'd':
		s = sin(-0.05);
		c = cos(-0.05);
		eyex = (eyex * c - eyez * s);
		eyez = (eyex * s + eyez * c);
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	double angle_in_degrees = pitch * 180 / PI;
	bool clickInSlider = x > 40 && x < 60 && HEIGHT - y >= angle_in_degrees - 10 && HEIGHT - y < angle_in_degrees + 10;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && clickInSlider) // drag slider
		slider_is_moving = true;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		slider_is_moving = false;
	if (button == GLUT_RIGHT_BUTTON)
		colaLevel = 0;
}

void mouseDrag(int x, int y)
{
	if (slider_is_moving && HEIGHT - y <= 100 && HEIGHT - y >= 0)
	{
		pitch = (HEIGHT - y) * PI / 180; // transform dy to radian
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z buffer
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here

	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(eyex, eyey, eyez, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (colaLevel > 0)
	{
		if (colaLevel > 0.5){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bubAmb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bubDiff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bubSpec);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96);
		glPushMatrix();
		drawBubbles();
		glPopMatrix();
		}

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colaAmb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colaDiff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colaSpec);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96);
		glPushMatrix();
		drawColaInGlass();
		glPopMatrix();
	}

	if (pitch * 180 / PI > 73)
	{
		DrawCola();
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glassAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glassDiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96);

	glPushMatrix();
	glRotated(-pitch * 180 / PI, 0, 0, 1);
	DrawBakbuk();
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glassAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glassDiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96);
	glPushMatrix();
	Drawkos();
	glPopMatrix();

	
	glDisable(GL_LIGHTING);
	// controls sub-window
	glViewport(0, 0, 100, 100);
	// Now we switch to Orthogonal projection, so we must change the projections matrix
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here
	glOrtho(0, 100, 0, 100, -1, 1);
	glDisable(GL_DEPTH_TEST);// return to principles of graphics 2D

	DrawSlider();

	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers(); // show all
}

void idle()
{
	glutPostRedisplay(); // calls indirectly display
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 50);
	glutCreateWindow("BABUK");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);

	init();

	glutMainLoop();
}
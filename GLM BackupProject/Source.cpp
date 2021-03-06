#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "imageloader.h"
#include "Falcon.h"

#define PI 3.141592

// variaveis
float x=0, y=0, z=0, rotX=0;
int fps=0, displayList=0;
bool displayFog = false;
bool displayMotionBlur = false;

// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging starts
int yDragStart = 0;
float deltaHeading = 0;
float deltaPitch = 0;
float distance = 10;
GLfloat lightPos[4] = {0.0, 2.0 ,0.0, 1.0};
GLfloat lightAmb[3] = {0.1, 0.1, 0.1};
GLfloat lightDiff[3] = {1.0, 1.0, 1.0};

Falcon myFalcon;

//GLMmodel* mymodel1;


GLuint texture1;

void keyboard(unsigned char key, int xx, int yy) {
	glutPostRedisplay();
	switch(key) {
		case 'a' : x-=2; break; 
		case 'd' : x+=2; break; 
		case 's' : z+=2; break; 
		case 'w' : myFalcon.moveForward(); break; 
		case '.' : y-=2; break; 
		case ',' : y+=2; break; 
		case 'k' : rotX-=2.0; break; 
		case 'i' : rotX+=2.0; break; 
		case 'm' : 
			if(displayMotionBlur){
				displayMotionBlur = false;
			}
			else {
				displayMotionBlur = true;
			}
			break; 
		case 'f' : 
			if(displayFog){
				displayFog = false;
			}
			else {
				displayFog = true;
			}
			break; 
		case 27  : exit(0);
	}
}
void mouseMove(int x, int y)
{
	if (isDragging) { // only when dragging
		// update the change in angle
		deltaHeading = ((x - xDragStart) * 0.02);
		deltaPitch = ((y - yDragStart) * 0.02);
		myFalcon.heading -= deltaHeading; // update camera turning angle
		myFalcon.pitch += deltaPitch;


		
	}
}
void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) { // left mouse button pressed
			isDragging = 1; // start dragging
			xDragStart = x; // save x where button first pressed
			yDragStart = y;	//save y where button first pressed
		}
		else  { /* (state = GLUT_UP) */
			myFalcon.heading -= deltaHeading; // update camera turning angle
			myFalcon.pitch += deltaPitch ;
			isDragging = 0; // no longer dragging
		}
	}
}


void changeSize(int w, int h) {
	if(h == 0) h = 1;
	float ratio = 1.0* w / h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);
	gluPerspective(25, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

//Draw Axes
void drawAxes()
{
	glPushMatrix();
		GLboolean isEnabled = false;
		glGetBooleanv(GL_LIGHTING, &isEnabled); /* To restore this lighting later */
		glDisable(GL_LIGHTING);
		glLineWidth(2);
		glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(2.0, 0.0, 0.0);


			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 2.0, 0.0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 2.0);
			glColor3f(1.0, 1.0, 1.0);
		glEnd();
		
		if(isEnabled)
			glEnable(GL_LIGHTING);
	glPopMatrix();
}

void motionBlur(){
	if(displayMotionBlur){
		glAccum(GL_MULT, 0.95);
		glAccum(GL_ACCUM, 1-0.95);
		glAccum(GL_RETURN, 1.0);
		glFlush();
	}
}

void fog(){
	if(displayFog){
		glEnable(GL_FOG);
		{
			GLfloat fogColor[4] = {1, 0.5, 0.5, 0.5};
			glFogi (GL_FOG_MODE, GL_LINEAR);
			glFogfv (GL_FOG_COLOR, fogColor);
			//glFogf (GL_FOG_DENSITY, 0.35);
			glHint (GL_FOG_HINT, GL_DONT_CARE);
			glFogf (GL_FOG_START, 1.0);
			glFogf (GL_FOG_END, 5.0);
		}
		glClearColor(1, 0.5, 0.5, 0.5);  /* fog color */
	}
	else
		glDisable(GL_FOG);
}

void terrain(){

	int height[88][88] ={

			//1
			{0,2,1,2,3,4,3,4,3,3,3,3,4,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1,1,1,0,1,
			2,2,2,1,2,3,4,3,2,2,3,4,4,5,5,5,5,4,4,3,0,1,1,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,5,6,7,7,8,7,6,4,5,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//2
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//3
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//4
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//5
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//6
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//7
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//8
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//9
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//10
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3},
			//11
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,
			1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1,1,3,2,3,3,4,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,
			1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1,1,2,1,2,2,2,2,1},
			{1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,
			1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2,1,2,2,3,3,3,3,2},
			{1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,
			1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3,1,2,2,3,4,4,3,3}
	};

	int MAP_SIZE = 88;

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(-44.0f,-2.0f,-44.0f);
    for (int x = 1; x < MAP_SIZE-1; x++) { 
      for (int z = 1; z < MAP_SIZE-1; z++) {
		glBegin(GL_QUADS);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(x,height[x][z],z);
			glVertex3f(x+1,height[x+1][z],z);
			glVertex3f(x+1,height[x+1][z+1],z + 1);
			glVertex3f(x,height[x][z+1],z+1);
        glEnd();
      }
    }
    glPopMatrix();
}

void updateCamera(){
	//look at Millenium falcon at all times
	float lookX = myFalcon.pos_x;
	float lookY = myFalcon.pos_y;
	float lookZ = myFalcon.pos_z;
	//camera position
	float eyeX;
	float eyeY;
	float eyeZ;


	gluLookAt(15.0, 15.0, 15.0,
		lookX, lookY, lookZ,
		0.0, 1.0, 0.0);
}

void renderScene(void) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	//gluLookAt(15.0, 15.0,	15.0,
	//		  0.0,	0.0,	0.0,
	//	      0.0f,	1.0f,	0.0f);	
	updateCamera();
	drawAxes();

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
	
	glTranslatef(x,y,z);
	glRotatef(rotX, 1.0, 0.0, 0.0);
	
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//Build the Height Map
	terrain();

	//Display Fog
	fog();

	myFalcon.draw();
	
	glutSwapBuffers();
}


void init(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Project");

	glEnable(GL_TEXTURE_2D);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);


	myFalcon = Falcon();

}




int main(int argc, char **argv) {
	init(argc, argv);
	
	
	// por registo de funcoes aqui
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboard);
	//glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMouseFunc(mouseButton); // process mouse button push/release
	glutMotionFunc(mouseMove); // process mouse dragging motion
	
	
	glutMainLoop();
	
	return 0;
}


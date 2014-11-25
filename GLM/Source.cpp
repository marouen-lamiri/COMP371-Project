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


/* perlin noise functions: */
inline double findnoise2(double x,double y) {
	int n=(int)x+(int)y*57;
	n=(n<<13)^n;
	int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
	return 1.0-((double)nn/1073741824.0);
}

inline double interpolate(double a,double b,double x) {
	double ft=x * 3.1415927;
	double f=(1.0-cos(ft))* 0.5;
	return a*(1.0-f)+b*f;
}
double noise(double x,double y) {
	double floorx=(double)((int)x);//This is kinda a cheap way to floor a double integer.
	double floory=(double)((int)y);
	double s,t,u,v;//Integer declaration
	s=findnoise2(floorx,floory);
	t=findnoise2(floorx+1,floory);
	u=findnoise2(floorx,floory+1);//Get the surrounding pixels to calculate the transition.
	v=findnoise2(floorx+1,floory+1);
	double int1=interpolate(s,t,x-floorx);//Interpolate between the values.
	double int2=interpolate(u,v,x-floorx);//Here we use x-floorx, to get 1st dimension. Don't mind the x-floorx thingie, it's part of the cosine formula.
	
	return interpolate(int1,int2,y-floory);//Here we use y-floory, to get the 2nd dimension.
}
/* perlin noise functions END */






void keyboard(unsigned char key, int xx, int yy) {
	glutPostRedisplay();
	switch(key) {
//<<<<<<< HEAD
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
//=======
//	case 'a' : x-=2; break; 
//	case 'd' : x+=2; break; 
//	case 's' : z+=2; break; 
//	case 'w' : z-=2; break; 
//	case '.' : y-=2; break; 
//	case ',' : y+=2; break; 
//	case 'k' : rotX-=2.0; break; 
//	case 'i' : rotX+=2.0; break; 
//	case 'm' : 
//		if(displayMotionBlur){
//			displayMotionBlur = false;
//		}
//		else {
//			displayMotionBlur = true;
//		}
//		break; 
//	case 'f' : 
//		if(displayFog){
//			displayFog = false;
//		}
//		else {
//			displayFog = true;
//		}
//		break; 
//	case 27  : exit(0);
//>>>>>>> c4271f11939f4239f54be03a68d0736b68fd4215
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
		glAccum(GL_MULT, 0.2);
		glAccum(GL_ACCUM, 1-0.2);
		glAccum(GL_RETURN, 1.0);
		glFlush();
	}
}

void fog(){
	if(displayFog){
		glEnable(GL_FOG);
		{
			GLfloat fogColor[4] = {0.894, 0.678, 0.525, 0.5};
			glFogi (GL_FOG_MODE, GL_LINEAR);
			glFogfv (GL_FOG_COLOR, fogColor);
			//glFogf (GL_FOG_DENSITY, 0.35);
			glHint (GL_FOG_HINT, GL_DONT_CARE);
			glFogf (GL_FOG_START, 1.0);
			glFogf (GL_FOG_END, 5.0);
		}
		glClearColor(0.894, 0.678, 0.525, 0.5);  /* fog color */
	}
	else
		glDisable(GL_FOG);
}

void terrain(){

	int height_[88][88] ={

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
	//int MAP_SIZE = 10;

	// get a noise map:
	int height[88][88];
	for (int x = 1; x < MAP_SIZE-1; x++) { 
		double getnoise =0;
		double zoom = 10.0;//0.000001;//1.0;
		double p = 0.05;//0.5;//P stands for persistence, this controls the roughness of the picture, i use 1/2
		double zoomPerlin = 40.0;//75;//75;
		int octaves=3;
		for (int z = 1; z < MAP_SIZE-1; z++) {
			for(int a=0;a<octaves-1;a++) { //This loops trough the octaves.
				double frequency = pow(2,a);//This increases the frequency with every loop of the octave.
				double amplitude = pow(p,a);//This decreases the amplitude with every loop of the octave.
				getnoise = noise(((double)x)*frequency/zoomPerlin,((double)z)/zoomPerlin*frequency)*amplitude;//This uses our perlin noise functions. It calculates all our zoom and frequency and amplitude
				
			}
			height[x][z] = getnoise * 100 ;
		}
	}
	// now draw the vertices:
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	GLfloat mat_specular[] = {4.0, 2.0, 2.0, 1.0};
	GLfloat mat_shininess[] = {50.0f};
	glShadeModel (GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	GLfloat mat_diffuse[] = {0.843, 0.514, 0.278, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glColor3f(0.2f, 0.2f, 1.0f);
	glTranslatef(-44.0f,-2.0f,-44.0f);
	for (int x = 1; x < MAP_SIZE-1; x++) { 
		for (int z = 1; z < MAP_SIZE-1; z++) {
			glBegin(GL_QUADS);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glColor3f(1.0f, 1.0f, 1.0f);
			glColor3f(0.5f, 0.0f, 0.5f);
			//glutSolidCube(height[x][z]);

			//draw vertices:
			glVertex3f(x,height[x][z],z);
			glVertex3f(x+1,height[x+1][z],z);
			glVertex3f(x+1,height[x+1][z+1],z + 1);
			glVertex3f(x,height[x][z+1],z+1);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

	eyeX = myFalcon.pos_x + myFalcon.forwardX * -distance;
	eyeY = myFalcon.pos_y + myFalcon.forwardY * -distance;
	eyeZ = myFalcon.pos_z + myFalcon.forwardZ * -distance;


	gluLookAt(eyeX, eyeY, eyeZ,
		lookX, lookY, lookZ,
		0.0, 1.0, 0.0);
}

void renderScene(void) {

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	//motion blur
	motionBlur();

	//1 way
	glCallList(displayList);

	glTranslatef(0, 4, 0);


	

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


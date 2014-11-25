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

int terrainTranslationConstant_X = 94;
int terrainTranslationConstant_Z = 44;

bool isInWireFrameMode = false;
bool isfirstPerson = true;
//float helicopterPositionX = 0;
//float helicopterPositionY = 0;
//float helicopterPositionZ = 0;
bool light0_isEnabled = true;
bool light1_isEnabled = true;//true;
bool light2_isEnabled = true;
bool isHighBeamMode = true;
int frameCounter = 0;
int isRotatingLeft;
int isRotatingRight;


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
	case't': isInWireFrameMode = !isInWireFrameMode; break;
	case '1': isfirstPerson = true; break;
	case '3': isfirstPerson = false; break;
	case 'h' : isHighBeamMode = !isHighBeamMode; break;
		//case 'p' : terrainTranslationConstant_X -= 5; break;
		//case 'o' : terrainTranslationConstant_Z -= 5; break;
		//case 'a' : x-=2; break; 
		//case 'd' : x+=2; break; 
		//case 's' : z+=2; break; 
		//case 'w' : myFalcon.moveForward(); break; 
		//case 'w' : z-=2; break; 
	case 'a' : myFalcon.pos_x += 1; break; 
	case 'd' : myFalcon.pos_x -= 1; break; 
	case 'w' : myFalcon.pos_z += 1; break; 
	case 's' : myFalcon.pos_z -= 1; break; 
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


void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.1f;

	switch (key) {
	case GLUT_KEY_F1:
		light0_isEnabled = ! light0_isEnabled;
		break;
	case GLUT_KEY_F2:
		light1_isEnabled = ! light1_isEnabled;
		break;
	case GLUT_KEY_F3:
		light2_isEnabled = ! light2_isEnabled;
		break;
		//case GLUT_KEY_F4:
		//	isMetalAppearance = ! isMetalAppearance;
		//	break;
		//case GLUT_KEY_F5:
		//	//texturesIsEnabled = ! texturesIsEnabled;
		//	isTextureNumber++;
		//	isTextureNumber = isTextureNumber % 5;
		//	break;
	case GLUT_KEY_LEFT :
		//terrainTranslationConstant_X -= 5;
		isRotatingLeft = 5;
		break;
	case GLUT_KEY_RIGHT :
		//terrainTranslationConstant_X += 5;
		isRotatingRight = 5;
		break;
	case GLUT_KEY_UP :
		myFalcon.pos_y += 1.0f;
		break;
	case GLUT_KEY_DOWN :
		myFalcon.pos_y -= 1.0f;
		break;
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


/* spotlight functions: */
void createSpotLight(float positionX, float positionY, float positionZ)
{
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess[] = {50.0};
	//myFalcon.pos_x , myFalcom.pos_y, myFalcon.pos_z;
	GLfloat position[] = {myFalcon.pos_x, myFalcon.pos_y, myFalcon.pos_z+5.0f, 1.0};
	GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};

	//GLfloat diffuse_blueLight[] = {1,1,1,1}; 
	//GLfloat ambient_blueLight[] = {1,1,1,1}; //{.5,0,0,1}; 
	//GLfloat specular_blueLight[] = {1,1,1,1}; 
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_blueLight); 
	//glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_blueLight); 
	//glLightfv(GL_LIGHT1, GL_SPECULAR, specular_blueLight); 

	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

	glLightfv(GL_LIGHT1,GL_SPECULAR,specular);//GL_LIGHT0
	glLightfv(GL_LIGHT1,GL_POSITION,position);
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,50.0f);
	glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,2.0f);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT1);
}

void spotlight() {

	// SPOTLIGHT 

	glPushMatrix();

	if(light1_isEnabled) {
		glEnable(GL_LIGHT1); //enable the light
	} else {
		glDisable(GL_LIGHT1);
	}
	// set last term to 0 for a spotlight (see chp 5 in ogl prog guide) 
	//myFalcon.pos_x , myFalcom.pos_y, myFalcon.pos_z;
	//GLfloat lightpos_blueLight[] = {mymodel1.pos_x+2.0f, 0, mymodel1.pos_z, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	//GLfloat lightpos_blueLight[] = {0+2.0f, 0, 0, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	GLfloat lightpos_blueLight[] = {myFalcon.pos_x, myFalcon.pos_y + 10.0f, myFalcon.pos_z, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	glLightfv(GL_LIGHT1,GL_POSITION, lightpos_blueLight); 

	if(isHighBeamMode){
		glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,90.0f);
		//GLfloat directionVector_blueLight[] = {3.0f, -10.0f, 0};
		GLfloat directionVector_blueLight[] = {myFalcon.pos_x - lightpos_blueLight[0], myFalcon.pos_y - lightpos_blueLight[1], myFalcon.pos_z - lightpos_blueLight[2]};
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, directionVector_blueLight);
	} else {
		glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,40.0f);
		//GLfloat directionVector_blueLight[] = {1.0f, -10.0f, 0};
		GLfloat directionVector_blueLight[] = {myFalcon.pos_x - lightpos_blueLight[0], myFalcon.pos_y - lightpos_blueLight[1] - 10.0f, myFalcon.pos_z - lightpos_blueLight[2]};
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, directionVector_blueLight);
	}

	glPopMatrix();
}
/* spotlight functions: */

void createRedLight(float positionX, float positionY, float positionZ)
{
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess[] = {50.0};
	//GLfloat position[] = {helicopterPositionX, helicopterPositionY, helicopterPositionZ+5.0f, 1.0};
	GLfloat position[] = {0, 20.0f, 0, 1.0};
	GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	//GLfloat diffuse[] = {0.0, 0.0, 0.0, 1.0};
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);

	GLfloat diffuse[] = {0,0,0,1};//{1,1,1,1}; //{1,1,1,1};//
	GLfloat ambient_[] = {0,0,0,1};//{1,1,1,1}; //{.5,0,0,1}; 
	GLfloat specular_[] = {1,1,1,1};
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse); 
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient_); 
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular_); 


	//glShadeModel (GL_SMOOTH);
	glLightfv(GL_LIGHT2,GL_SPECULAR,specular);//GL_LIGHT0
	glLightfv(GL_LIGHT2,GL_POSITION,position);
	glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,50.0f);
	glLightf(GL_LIGHT2,GL_SPOT_EXPONENT,2.0f);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, color);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT2);
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

	//int MAP_SIZE = 88;
	int MAP_SIZE = 88;
	//int MAP_SIZE = 10;

	// get a noise map for each translation of the terrain:
	int height[88][88];
	//int terrainTranslationConstant_X = 94; // now made global vars
	//int terrainTranslationConstant_Z = 44;
	double getnoise =0;

	// params:
	double zoom = 10.0;//0.000001;//1.0;
	double p = 0.05;//0.5;//P stands for persistence, this controls the roughness of the picture, i use 1/2
	double zoomPerlin = 40.0;//75;//75;
	int octaves=3;

	// movement:
	int speed = 1; // num of new rows of terrain vertices to create per frame
	//if(frameCounter > 1) {
	terrainTranslationConstant_Z -= speed; // this was incremented with o and p keys before, now the ship moves forward automatically.
	//frameCounter = 0;
	//}
	//frameCounter++;

	// handle ship roll rotation:
	float rotSpeed = 1.0f;
	float maxRotation = 36.0f;
	if(isRotatingLeft > 0) {
		isRotatingLeft--;

		// move terrain:
		terrainTranslationConstant_X -= 1;

		// rotate ship:
		if(myFalcon.roll < maxRotation) 
			myFalcon.roll += rotSpeed;
		//rotX += 1.0;
	} 
	else {
		if(myFalcon.roll <= 0) {
			//myFalcon.roll = 0;
		}
		else {
			myFalcon.roll -= rotSpeed;
			terrainTranslationConstant_X -= 1;
		}
	}
	if(isRotatingRight > 0) {
		isRotatingRight--;

		// move terrain:
		terrainTranslationConstant_X += 1;

		// rotate ship:
		if(myFalcon.roll > -maxRotation) 
			myFalcon.roll -= rotSpeed;
		//rotX += 1.0;

	} 
	else {
		if(myFalcon.roll >= 0) {
			//myFalcon.roll = 0;
		}
		else {
			myFalcon.roll += rotSpeed;
			terrainTranslationConstant_X += 1;
		}
	}
	//end ship roll rotation


	// terrain generation:
	int xTranslated;
	int zTranslated;
	for (int x = 1; x < MAP_SIZE-1; x++) { 
		xTranslated = x + terrainTranslationConstant_X;
		for (int z = 1; z < MAP_SIZE-1; z++) {
			zTranslated = z + terrainTranslationConstant_Z;
			for(int a=0;a<octaves-1;a++) { //This loops trough the octaves.
				double frequency = pow(2,a);//This increases the frequency with every loop of the octave.
				double amplitude = pow(p,a);//This decreases the amplitude with every loop of the octave.
				getnoise = noise(((double)xTranslated)*frequency/zoomPerlin,((double)zTranslated)/zoomPerlin*frequency)*amplitude;//This uses our perlin noise functions. It calculates all our zoom and frequency and amplitude

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

			//// place some pilars on the landscape:
			////if(x % 20 == 0) {
			//if(height[x+1][z] > 3.95f) {// && z % 20 == 0) {	// we never get landscape above 4.0f with current params	
			//	glPushMatrix();
			//	glTranslatef(x,height[x][z],z);
			//	glScalef(1.0f, 10.0f, 1.0f);
			//	glutSolidCube(1);
			//	glPopMatrix();
			//}
		}
	}
	glPopMatrix();
}

void updateCamera() {

	//look at Millenium falcon at all times
	float lookX = myFalcon.pos_x;
	float lookY = myFalcon.pos_y;
	float lookZ = myFalcon.pos_z;
	//camera position
	float eyeX;
	float eyeY;
	float eyeZ;

	eyeX = (myFalcon.pos_x + myFalcon.forwardX * -distance);
	eyeY = (myFalcon.pos_y + myFalcon.forwardY * -distance) + 1.0f;
	eyeZ = -(myFalcon.pos_z + myFalcon.forwardZ * -distance); // minus important so we are behind the ship.

	// Reset transformations
	//glLoadIdentity();

	// Set the camera
	if(isfirstPerson) {

		//cout<<"FIRST PERSON CAM";
		//glRotatef(90,0.0,1.0,0.0);		
		//gluLookAt(
		//	helicopterPositionX, helicopterPositionY, helicopterPositionZ+5.0f, //1.0f for ly before in example
		//	helicopterPositionX+2.0f, helicopterPositionY, helicopterPositionZ+2.0f,
		//	//x+lx, 1.0f,  z+lz,
		//	0.0f, 1.0f,  0.0f);
		//glRotatef(-90,0.0,1.0,0.0);	
		gluLookAt(
			eyeX, eyeY, eyeZ,
			lookX, lookY, lookZ,
			0.0, 1.0, 0.0);

	} else {

		//cout<<"THIRD PERSON CAM";
		//float lx = 10;
		//float ly = 50;
		//float lz = 10;
		//float lx = myFalcon.pos_x;
		//float ly = myFalcon.pos_y + 10.0f;
		//float lz = myFalcon.pos_z;
		float lx = eyeX + 70.0f;
		float ly = eyeY + 70.0f;
		float lz = eyeZ + 40.0f;
		gluLookAt(	
			lx, ly, lz, //1.0f for ly before in example
			//eyeX, eyeY + 10.0f, eyeZ,

			//0,0,0,
			lookX, lookY, lookZ,
			//x+lx, 1.0f,  z+lz,
			0.0f, 1.0f,  0.0f);


		// RED LIGHT:
		glPushMatrix();

		if(light2_isEnabled) {
			glEnable(GL_LIGHT2); //enable the light
		} else {
			glDisable(GL_LIGHT2);
		}

		// set last term to 0 for a spotlight (see chp 5 in ogl prog guide) 
		GLfloat lightpos[] = {lx,ly,lz,1.0f};//{lx*1.0f, ly*1.0f, lz*1.0f, 1.0f};//{3.0f, 15.0f, 0, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
		glLightfv(GL_LIGHT2, GL_POSITION, lightpos);

		//GLfloat directionVector[] = {-lx,-ly,-lz};//{0, -1.0f, 0}; // camera lookat point is the origin.
		GLfloat directionVector[] = {myFalcon.pos_x-lx,myFalcon.pos_y-ly,myFalcon.pos_z-lz};//{0, -1.0f, 0}; // camera lookat point is the origin.
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, directionVector);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF,180.0f);

		glPopMatrix();

		// END RED LIGHT.
	}
}

void renderScene(void) {
	glLoadIdentity();
	if(isInWireFrameMode) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
	terrain();
	spotlight();

	// main light:
	if(light0_isEnabled) {
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
	} else {
		glDisable(GL_LIGHT0);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	//gluLookAt(15.0, 15.0,	15.0,
	//	0.0,	0.0,	0.0,
	//	0.0f,	1.0f,	0.0f);	
	updateCamera();

	drawAxes();


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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);


	glEnable(GL_COLOR_MATERIAL);


	myFalcon = Falcon();


	createSpotLight(0,0,0);// create spotlight
	createRedLight(0.0f,1.0f,0.0f); // create main light

}






int main(int argc, char **argv) {
	init(argc, argv);


	// por registo de funcoes aqui
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	//glutIdleFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);


	glutMouseFunc(mouseButton); // process mouse button push/release
	glutMotionFunc(mouseMove); // process mouse dragging motion







	glutMainLoop();

	return 0;
}


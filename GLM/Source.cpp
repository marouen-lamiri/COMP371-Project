#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "imageloader.h"
#include "Falcon.h"
#include <iostream>
#include <math.h> // math definitions
#include <time.h>

#define PI 3.141592
#define MAX_PARTICLES   1000        // Number Of Particles To Create

// variaveis
float x=0, y=0, z=0, rotX=0;
int fps=0, displayList=0;
bool displayFog = false;
bool displayMotionBlur = false;
bool    rainbow = true;           // Rainbow Mode?    ( ADD )
time_t explosionStart;
time_t currentTime;

float   slowdown = 2.0f;          // Slow Down Particles
float   zoom = 0.0f;            // Used To Zoom Out
float   xspeed;             // Base X Speed (To Allow Keyboard Direction Of Tail)
float   yspeed;             // Base Y Speed (To Allow Keyboard Direction Of Tail)


// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging starts
int yDragStart = 0;
float deltaHeading = 0;
float deltaPitch = 0;
float distance = 20;

bool ambientLight = true;

Falcon myFalcon;

//GLMmodel* mymodel1;

GLuint texture1;
GLuint texture2;
GLuint texture3;
GLuint texture4;
GLuint texture5;
GLuint texture6;
GLuint texture7;
GLuint texture8;
GLuint texture9;

GLuint  loop;               // Misc Loop Variable
GLuint  col;                // Current Color Selection
GLuint  delay;              // Rainbow Effect Delay

int terrainTranslationConstant_X = 94;
int terrainTranslationConstant_Z = 44;

bool isInWireFrameMode = false;
bool isfirstPerson = true;
bool light0_isEnabled = true;
bool light1_isEnabled = true;//true;
bool light2_isEnabled = true;
bool isHighBeamMode = true;
int frameCounter = 0;
int isRotatingLeft;
int isRotatingRight;
int MAP_SIZE = 88;
bool Smoke = true;
int height[88][88];
bool sand = false;
float angleExplosion = 0;
int pilarsAreDrawn;
bool originHasPillar = false;
int pillarHeight = 10;


// textures:

int displayList2;

typedef struct                      // Create A Structure For Particle
{
	bool    active;                 // Active (Yes/No)
	float   life;                   // Particle Life
	float   fade;                   // Fade Speed

	float   r;                  // Red Value
	float   g;                  // Green Value
	float   b;                  // Blue Value

	float   x;                  // X Position
	float   y;                  // Y Position
	float   z;                  // Z Position

	float   xi;                 // X Direction
	float   yi;                 // Y Direction
	float   zi;                 // Z Direction

	float   xg;                 // X Gravity
	float   yg;                 // Y Gravity
	float   zg;                 // Z Gravity

}
particles;                      // Particles Structure
particles particle[MAX_PARTICLES];          // Particle Array

static GLfloat colors[12][3] =               // Rainbow Of Colors
{
	{ 1.0f, 0.5f, 0.5f }, { 1.0f, 0.75f, 0.5f }, { 1.0f, 1.0f, 0.5f }, { 0.75f, 1.0f, 0.5f },
	{ 0.5f, 1.0f, 0.5f }, { 0.5f, 1.0f, 0.75f }, { 0.5f, 1.0f, 1.0f }, { 0.5f, 0.75f, 1.0f },
	{ 0.5f, 0.5f, 1.0f }, { 0.75f, 0.5f, 1.0f }, { 1.0f, 0.5f, 1.0f }, { 1.0f, 0.5f, 0.75f }
};

// ----------------------------------- functions -------------------------------------------------



void smoke(){
	if(Smoke){

		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture8);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTranslatef(myFalcon.pos_x, myFalcon.pos_y, myFalcon.pos_z);// put smoke at rear of ship
		GLfloat mat_specular[] = {4.0, 2.0, 2.0, 1.0};
		GLfloat mat_shininess[] = {50.0f};
		glShadeModel (GL_SMOOTH);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		GLfloat mat_diffuse[] = {0.961f, 0.961f, 0.961f, 1.0f};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		for(float i = 0; i < 30; i+=0.01){
			glColor3f(1.0,0.0,0.0);
			glTranslatef(0.0, 0.0+i, 0.0);
			glutSolidCube(0.3);
			glTranslatef(0.1,0.1+i,0.1);
			glutSolidCube(0.3);
			glTranslatef(0.1,0.0+i,0.1);
			glutSolidCube(0.3);
			glTranslatef(0.1,0.1+i,0.0);
			glRotatef(angleExplosion++,0.0,1.0,0.0);
		}
		glPopMatrix();
	}
}
void explode(){
	if (difftime(currentTime, explosionStart) > 1){
		explosionStart = time(NULL);
		for (loop = 0; loop < MAX_PARTICLES; loop++)
		{
			particle[loop].active = true;                 // Make All The Particles Active
			particle[loop].life = 1.0f;                   // Give All The Particles Full Life
			particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;       // Random Fade Speed
			particle[loop].r = colors[loop*(12 / MAX_PARTICLES)][0];        // Select Red Rainbow Color
			particle[loop].g = colors[loop*(12 / MAX_PARTICLES)][1];        // Select Gree Rainbow Color
			particle[loop].b = colors[loop*(12 / MAX_PARTICLES)][2];        // Select Blue Rainbow Color
			particle[loop].x = myFalcon.pos_x;
			particle[loop].y = myFalcon.pos_y;
			particle[loop].z = myFalcon.pos_z;
			particle[loop].xi = float((rand() % 50) - 26.0f)*20.0f;       // Random Speed On X Axis
			particle[loop].yi = float((rand() % 50) - 25.0f)*20.0f;       // Random Speed On Y Axis
			particle[loop].zi = float((rand() % 50) - 25.0f)*20.0f;       // Random Speed On Z Axis
			particle[loop].xg = 0.0f;                     // Set Horizontal Pull To Zero
			particle[loop].yg = -0.0f;                    // Set Vertical Pull Downward
			particle[loop].zg = 0.0f;                     // Set Pull On Z Axis To Zero
		}
	}
}

void sandStorm()
{        
	if(sand){

		glPushMatrix();
		glTranslatef(myFalcon.pos_x, myFalcon.pos_y, myFalcon.pos_z);// put smoke at rear of ship
		GLfloat mat_specular[] = {4.0, 2.0, 2.0, 1.0};
		GLfloat mat_shininess[] = {50.0f};
		glShadeModel (GL_SMOOTH);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		GLfloat mat_diffuse[] = {1, 0.647, 0, 1.0f};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		for(float i = 0; i < 10; i+=0.1){
			for(float j = 0; j < 10; j+= 0.1){
				glTranslatef(j, i, 0.0);
				glutSolidCube(0.025);
				glTranslatef(-j,-i,0.1);
				glutSolidCube(0.025);
				glTranslatef(0.1,-i,j);
				glutSolidCube(0.025);
				glTranslatef(0.1,i,j);
				glRotatef(angleExplosion++,0.0,1.0,0.0);
			}
		}
		glPopMatrix();
	}
}

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
	case '2' : 
		if(sand){sand = false;}
		else {sand = true;}
		break; 
	case't': isInWireFrameMode = !isInWireFrameMode; break;
	case '1': isfirstPerson = true; break;
	case '3': isfirstPerson = false; break;
	case '4': ambientLight = !ambientLight;
	case 'h' : isHighBeamMode = !isHighBeamMode; break;
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
	case GLUT_KEY_LEFT :
		isRotatingLeft = 5;
		break;
	case GLUT_KEY_RIGHT :
		isRotatingRight = 5;
		break;
	case GLUT_KEY_UP :
		if(myFalcon.pos_y <= 10.0f) {
			myFalcon.pos_y += 1.0f; 
		}
		break;
	case GLUT_KEY_DOWN :
		if(myFalcon.pos_y >= -5.0f) {
			myFalcon.pos_y -= 1.0f;
		}
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


///* spotlight functions: */


void createSpotLight(float positionX, float positionY, float positionZ)
{
	//GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	//GLfloat shininess[] = {50.0};
	GLfloat position[] = {0, 0, 5.0f, 1.0};
	GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat diffuse_blueLight[] = {1,1,1,1}; 
	GLfloat ambient_blueLight[] = {1,1,1,1}; //{.5,0,0,1}; 
	GLfloat specular_blueLight[] = {1,1,1,1}; 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_blueLight); 
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_blueLight); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_blueLight); 

	//glLightfv(GL_LIGHT1,GL_SPECULAR,specular);//GL_LIGHT0
	glLightfv(GL_LIGHT1,GL_POSITION,position);
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,50.0f);
	glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,2.0f);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
}

void createSpotlight2Above() {
	//Select position of light and material characteristics
	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[] = {50.0};
	GLfloat light_position[] = {myFalcon.pos_x,myFalcon.pos_y + 10.0f, myFalcon.pos_z,1.0};
	GLfloat spotDir[] = {2.0 + myFalcon.pos_x,myFalcon.pos_y,1.0 + myFalcon.pos_z};
	GLfloat color[4] = { 0.0f, 1.0f, 1.0f, 1.0f};
	glShadeModel (GL_SMOOTH);
	glLightfv(GL_LIGHT1,GL_SPECULAR,mat_specular);
	glLightfv(GL_LIGHT1,GL_POSITION,light_position);
	// Definig spotlight attributes
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,45);
	glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,2.0f);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,spotDir);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void createDirectionalLight() {
	//GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	//GLfloat shininess[] = {50.0};
	GLfloat position[] = {0, 0, 5.0f, 0.0f};
	GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat diffuse_blueLight[] = {1,1,1,1}; 
	GLfloat ambient_blueLight[] = {1,1,1,1}; //{.5,0,0,1}; 
	GLfloat specular_blueLight[] = {1,1,1,1}; 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_blueLight); 
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_blueLight); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_blueLight); 

	//glLightfv(GL_LIGHT1,GL_SPECULAR,specular);//GL_LIGHT0
	glLightfv(GL_LIGHT1,GL_POSITION,position);
	//glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,180.0f);
	//glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,2.0f);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);
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
	//GLfloat lightpos_blueLight[] = {0+12.0f, 0, 0, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	//GLfloat lightpos_blueLight[] = {myFalcon.pos_x, myFalcon.pos_y + 10.0f, myFalcon.pos_z, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	GLfloat lightpos_blueLight[] = {myFalcon.pos_x, myFalcon.pos_y, myFalcon.pos_z+1.0f, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	glLightfv(GL_LIGHT1,GL_POSITION, lightpos_blueLight); 

	//float neg;
	//if(pilarsAreDrawn > 0) {
	//	neg = -1.0f;
	//}

	if(isHighBeamMode){
		glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,90.0f);
		//GLfloat directionVector_blueLight[] = {3.0f, -10.0f, 0};
		GLfloat directionVector_blueLight[] = {myFalcon.pos_x - lightpos_blueLight[0], myFalcon.pos_y - lightpos_blueLight[1], myFalcon.pos_z - lightpos_blueLight[2]};
		//GLfloat directionVector_blueLight[] = {(myFalcon.pos_x - lightpos_blueLight[0])*neg, myFalcon.pos_y - lightpos_blueLight[1], (myFalcon.pos_z - lightpos_blueLight[2])*neg};
		//GLfloat directionVector_blueLight[] = {(10.0f), -10.0f, 10.0f};
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, directionVector_blueLight);
	} else {
		glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,10.0f);
		//GLfloat directionVector_blueLight[] = {1.0f, -10.0f, 0};
		GLfloat directionVector_blueLight[] = {myFalcon.pos_x - lightpos_blueLight[0], myFalcon.pos_y - lightpos_blueLight[1], myFalcon.pos_z - lightpos_blueLight[2]};
		//GLfloat directionVector_blueLight[] = {(myFalcon.pos_x - lightpos_blueLight[0])*neg, myFalcon.pos_y - lightpos_blueLight[1] - 10.0f, (myFalcon.pos_z - lightpos_blueLight[2])*neg};
		//GLfloat directionVector_blueLight[] = {(10.0f), - 10.0f, 10.0f};
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, directionVector_blueLight);
	}

	glPopMatrix();

}

void spotlightAbove() {

	//---------------------------
	
	// SPOTLIGHT 
	glPushMatrix();
	//createSpotLight(0,0,0);
	if(light0_isEnabled) {//light0_isEnabled
		glEnable(GL_LIGHT0); //enable the light
	} else {
		glDisable(GL_LIGHT0);
	}
	// set last term to 0 for a spotlight (see chp 5 in ogl prog guide) 
	//GLfloat lightpos_blueLight[] = {myFalcon.pos_x+2.0f, myFalcon.pos_y + 10.0f, myFalcon.pos_z+2.0f, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	GLfloat lightpos_blueLight[] = {2.0f, 10.0f, 2.0f, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	glLightfv(GL_LIGHT0,GL_POSITION, lightpos_blueLight); 

	GLfloat diffuse_blueLight[] = {1,1,1,1}; 
	GLfloat ambient_blueLight[] = {1,1,1,1}; //{.5,0,0,1}; 
	GLfloat specular_blueLight[] = {1,1,1,1}; 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_blueLight); 
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_blueLight); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_blueLight); 

	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0f);
	GLfloat directionVector_blueLight[] = {4.0f, -10.0f, 0};
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, directionVector_blueLight);

	glPopMatrix();// end SPOTLIGHT 

}

void directionalLight() {

	//---------------------------
	
	// DIRECTiONAL LIGHT: 
	glPushMatrix();
	//createSpotLight(0,0,0);
	if(true) {//light2_isEnabled
		glEnable(GL_LIGHT2); //enable the light
	} else {
		glDisable(GL_LIGHT2);
	}
	// set last term to 0 for a spotlight (see chp 5 in ogl prog guide) 
	//GLfloat lightpos_blueLight[] = {myFalcon.pos_x+2.0f, myFalcon.pos_y + 10.0f, myFalcon.pos_z+2.0f, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	//GLfloat lightpos_blueLight[] = {2.0f, 10.0f, 2.0f, 1.0f};//1.0 //  {18.0f,0,0,1.0f};
	//glLightfv(GL_LIGHT0,GL_POSITION, lightpos_blueLight); 

	//GLfloat diffuse_blueLight[] = {1,1,1,1}; 
	//GLfloat ambient_blueLight[] = {1,1,1,1}; //{.5,0,0,1}; 
	//GLfloat specular_blueLight[] = {1,1,1,1}; 
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_blueLight); 
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_blueLight); 
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular_blueLight); 

	////glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0f);
	//GLfloat directionVector_blueLight[] = {4.0f, -10.0f, 0};
	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, directionVector_blueLight);

	glPopMatrix();// end SPOTLIGHT 

}

/* end the 3 lights functions: */

void createRedLight(float positionX, float positionY, float positionZ){

}



bool detectCollision(){
	int x = (int)(myFalcon.pos_x + 44.5);
	int y = (int)myFalcon.pos_y;
	int z = (int)(myFalcon.pos_z + 44.5);

	if ((x >= 0 && x < MAP_SIZE) && (z >= 0 && z < MAP_SIZE)){
		if (height[x][z] >= y){
			return true;
		}
		// for collision with pillars:
		if(originHasPillar && y <= pillarHeight + 100.0f) {
			return true;
		}
	}
	return false;

}

void terrain(){



	//int MAP_SIZE = 88; // now a global
	//int MAP_SIZE = 10;

	// get a noise map for each translation of the terrain:
	//int height[88][88];
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
	//glTranslatef(-64.0f,-2.0f,-44.0f);
	for (int x = 1; x < MAP_SIZE-1; x++) { 
		for (int z = 1; z < MAP_SIZE-1; z++) {
			glEnable(GL_TEXTURE_2D);
			if(height[x][z] == -3 || height[x][z] == -2 || height[x][z] == -1)
			{
				glBindTexture(GL_TEXTURE_2D, texture6);
			}
			else if(height[x][z] == -4)
			{
				glBindTexture(GL_TEXTURE_2D, texture4);
			}
			else if(height[x][z] >= 4)
			{
				glBindTexture(GL_TEXTURE_2D, texture5);
			}
			else if(height[x][z] >= 3)
			{
				glBindTexture(GL_TEXTURE_2D, texture1);
			}
			else{
				glBindTexture(GL_TEXTURE_2D, texture3);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBegin(GL_QUADS);

			//glColor3f(1.0f, 1.0f, 1.0f);
			glColor3f(0.5f, 0.5f, 0.5f);
			//glutSolidCube(height[x][z]);

			//draw vertices:
			//// place some pilars on the landscape:
			////if(x % 20 == 0) {
			if(height[x+1][z] > 3.95f) {// && z % 20 == 0) {	// we never get landscape above 4.0f with current params	

				//glPushMatrix();
				glBindTexture(GL_TEXTURE_2D, texture7);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				//glTranslatef(x,height[x][z],z);
				//glScalef(1.0f, 10.0f, 1.0f);
				//glTexCoord2f(0.0f, 0.0f);glTexCoord2f(1.0f, 0.0f);glTexCoord2f(0.0f, 1.0f);glTexCoord2f(1.0f, 1.0f);
				//glutSolidCube(1);

				// draw vertices:
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,height[x][z]+10, z); // add 10 for the pillar height
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x+1,height[x+1][z]+10, z);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x+1,height[x+1][z+1]+10, z + 1);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,height[x][z+1]+10, z+1);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glEnd();


				//glPopMatrix();
				pilarsAreDrawn = 10;
				// this is a boolean for collision detection of pillars: is the ship above a pillar?
				//if (x==(int)myFalcon.pos_x+1.0f && z==(int)myFalcon.pos_z+1.0f) { // if we're at the origin:
				if ((x-44)>=(int)myFalcon.pos_x-1.0f && (x-44)<=(int)myFalcon.pos_x+1.0f && (z-44)>=(int)myFalcon.pos_z-1.0f && (z-44)<=(int)myFalcon.pos_z+1.0f) { // if we're at the origin:
					originHasPillar = true;
					pillarHeight = 10;
					std::cout << " pilar collision ";
				} 
			} 
			// is the ship NOT above a pillar?
			else {

				// draw vertices: normal way
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,height[x][z],z);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x+1,height[x+1][z],z);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x+1,height[x+1][z+1],z + 1);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,height[x][z+1],z+1);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glEnd();


				//if (x==(int)myFalcon.pos_x+1.0f && z==(int)myFalcon.pos_z+1.0f) {
				if ((x-44)>=(int)myFalcon.pos_x-1.0f && (x-44)<=(int)myFalcon.pos_x+1.0f && (z-44)>=(int)myFalcon.pos_z-1.0f && (z-44)<=(int)myFalcon.pos_z+1.0f) { // if we're at the origin:
					// if we're at the origin, but we're not over a pillar:
					originHasPillar = false;
				}
			}

		}
	}

	pilarsAreDrawn--;

	glPopMatrix(); // end drawing vertices
}

void updateCamera() {

	//glLoadIdentity();
	//glPushMatrix();

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
		gluLookAt(
			eyeX, eyeY, eyeZ,
			lookX, lookY, lookZ,
			0.0, 1.0, 0.0);

	} else {

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
		//glPushMatrix();
	}
	//glPopMatrix();
}

void renderScene(void) {




	glLoadIdentity();

	// draw falcon:
	glEnable(GL_TEXTURE_2D);


	updateCamera();









	if(isInWireFrameMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	// update lights:
	spotlight();
	spotlightAbove();
	directionalLight();





	// collision detection
	if (detectCollision()){

		Smoke = true;
		explode();
		std::cout << "collision Detected";
	}
	else{
		Smoke = false;
	}


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	terrain();

	//Display Fog
	fog();



	//motion blur
	motionBlur();

	//smoke();
	sandStorm();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	myFalcon.draw();
	glDisable(GL_TEXTURE_2D);


	currentTime = time(NULL);
	double seconds = difftime(currentTime, explosionStart);
	if ( seconds < 1){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture9);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		for (loop = 0; loop < MAX_PARTICLES; loop++)                   // Loop Through All The Particles
		{
			if (particle[loop].active)                  // If The Particle Is Active
			{
				float x = particle[loop].x;               // Grab Our Particle X Position
				float y = particle[loop].y;               // Grab Our Particle Y Position
				float z = particle[loop].z + zoom;        // Particle Z Pos + Zoom

				// Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
				glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);

				glBegin(GL_TRIANGLE_STRIP);             // Build Quad From A Triangle Strip
				glTexCoord2d(1, 1); glVertex3f(x + 0.25f, y + 0.25f, z); // Top Right
				glTexCoord2d(0, 1); glVertex3f(x - 0.25f, y + 0.25f, z); // Top Left
				glTexCoord2d(1, 0); glVertex3f(x + 0.25f, y - 0.25f, z); // Bottom Right
				glTexCoord2d(0, 0); glVertex3f(x - 0.25f, y - 0.25f, z); // Bottom Left
				glEnd();                        // Done Building Triangle Strip

				//this moves the particle to the right place
				particle[loop].x += particle[loop].xi / (slowdown * 1000);    // Move On The X Axis By X Speed
				particle[loop].y += particle[loop].yi / (slowdown * 1000);    // Move On The Y Axis By Y Speed
				particle[loop].z += particle[loop].zi / (slowdown * 1000);    // Move On The Z Axis By Z Speed

				particle[loop].xi += particle[loop].xg;           // Take Pull On X Axis Into Account
				particle[loop].yi += particle[loop].yg;           // Take Pull On Y Axis Into Account
				particle[loop].zi += particle[loop].zg;           // Take Pull On Z Axis Into Account

				//if particle is dead Rejuvenate it
				if (particle[loop].life < 0.0f)                    // If Particle Is Burned Out
				{
					particle[loop].life = 1.0f;               // Give It New Life
					particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;   // Random Fade Value

					particle[loop].x = 0.0f;                  // Center On X Axis
					particle[loop].y = 0.0f;                  // Center On Y Axis
					particle[loop].z = 0.0f;                  // Center On Z Axis

					particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);  // X Axis Speed And Direction
					particle[loop].yi = yspeed + float((rand() % 60) - 30.0f);  // Y Axis Speed And Direction
					particle[loop].zi = float((rand() % 60) - 30.0f);     // Z Axis Speed And Direction

					//Assign a new particle Color
					particle[loop].r = colors[col][0];            // Select Red From Color Table
					particle[loop].g = colors[col][1];            // Select Green From Color Table
					particle[loop].b = colors[col][2];            // Select Blue From Color Table
				}
			}
		}
		glDisable(GL_TEXTURE_2D);
	}

	glutSwapBuffers();
}


void init(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Project");

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_COLOR_MATERIAL);

	myFalcon = Falcon();
	Image* image = loadBMP("models/rock.bmp");
	texture1 = loadTexture(image);

	image = loadBMP("Metal.bmp");
	texture2 = loadTexture(image);

	image = loadBMP("Grass.bmp");
	texture3 = loadTexture(image);

	image = loadBMP("Water.bmp");
	texture4 = loadTexture(image);

	image = loadBMP("Ice.bmp");
	texture5 = loadTexture(image);

	image = loadBMP("Sand.bmp");
	texture6 = loadTexture(image);

	image = loadBMP("Wood.bmp");
	texture7 = loadTexture(image);

	image = loadBMP("smoke.bmp");
	texture8 = loadTexture(image);

	image = loadBMP("explosion.bmp");
	texture9 = loadTexture(image);


	glBindTexture(GL_TEXTURE_2D, texture9);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	delete image;

	createSpotLight(0,0,0);
	createSpotlight2Above();
	createDirectionalLight();

	glEnable(GL_LIGHTING);

}

int main(int argc, char **argv) {
	init(argc, argv);


	// por registo de funcoes aqui
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);


	glutMouseFunc(mouseButton); // process mouse button push/release
	glutMotionFunc(mouseMove); // process mouse dragging motion

	glutMainLoop();

	return 0;
}


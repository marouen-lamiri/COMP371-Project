#include "Falcon.h"
#define PI 3.141592

Falcon::Falcon()
{
	speed = 0.05;
	//rotation
	roll = 0;
	pitch = 0;
	heading = 0;

	//Position
	pos_x = 0;
	pos_y = 0;
	pos_z = 0;

	//forward
	forwardX = 0.0f;
	forwardY = 0.0f;
	forwardZ = 1.0f;


	//load Model
	model = glmReadOBJ("3dFalconFINAL.obj");

	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90);

	displayList = glGenLists(1);
	glNewList(displayList, GL_COMPILE);
	glmDraw(model, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	glEndList();
	
}


Falcon::~Falcon()
{
}
void Falcon::draw()
{

	
	//Movement
	//glLoadIdentity();

	//translate to position in world Space
	glPushMatrix();
	glTranslatef(pos_x, pos_y, pos_z);

	//Rotate
	glRotatef(heading, 0, 1, 0);
	glRotatef(pitch, 1, 0, 0);
	glRotatef(roll, 0, 0, 1);

	//draws the Millenium Falcon
	glCallList(displayList);
	glPopMatrix();

}
//Moves the Contruct 1 unit in the Forward Direction
void Falcon::moveForward(){
	float yRotRad = (-heading / 180 * PI);
	float xRotRad = (-pitch / 180 * PI);

	float tPosX = pos_x;
	float tPosY = pos_y;
	float tPosZ = pos_z;

	pos_x -= sin(yRotRad) * speed;
	pos_z += cos(yRotRad) * speed;
	pos_y += sin(xRotRad) * speed;


	//get forward vector
	forwardX = pos_x - tPosX;
	forwardY = pos_y - tPosY;
	forwardZ = pos_z - tPosZ;

	float l = sqrt(forwardX*forwardX + forwardY *forwardY + forwardZ*forwardZ);


	forwardX /= l;
	forwardY /= l;
	forwardZ /= l;

}

void Falcon::Pitch(float p){
	pitch += p;
}
void Falcon::Heading(float h){
	heading += h;
}


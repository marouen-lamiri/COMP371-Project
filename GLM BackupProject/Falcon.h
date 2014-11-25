#pragma once
#include "glm.h"
#include "math.h"
class Falcon
{
public:
	
	
	float pos_x, pos_y, pos_z;
	float speed;
	
	int displayList;
	GLMmodel* model;

	float roll, pitch, heading;

	Falcon();
	~Falcon();
	void Falcon::draw();
	void Falcon::moveForward();
	void Falcon::Pitch(float p);
	void Falcon::Heading(float h);


};


#pragma once
#define CESSNA_POINT_COUNT 6763
#define PROPELLER_POINT_COUNT 6763

struct Point {
	GLfloat vertex_x;
	GLfloat vertex_y;
	GLfloat vertex_z;
	
	GLfloat normal_x;
	GLfloat normal_y;
	GLfloat normal_z;
};

struct Point planePoints[CESSNA_POINT_COUNT+2];
struct Point propellerPoints[PROPELLER_POINT_COUNT + 2];

struct FaceNode* planeFaceLists[33];
struct FaceNode* propellerFaces;
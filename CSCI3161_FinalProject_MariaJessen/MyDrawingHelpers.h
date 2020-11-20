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
struct Point leftPropellerPoints[PROPELLER_POINT_COUNT + 2];
struct Point rightPropellerPoints[PROPELLER_POINT_COUNT + 2];

struct FaceNode* planeFaceLists[33];
struct FaceNode* leftPropellerFaces[2];
struct FaceNode* rightPropellerFaces[2];

GLfloat leftHubLowestCoords[3] = { 10.0, 10.0, 10.0 };
GLfloat leftHubHighestCoords[3] = { -10.0, -10.0, -10.0 };

GLfloat rightHubLowestCoords[3] = { 10.0, 10.0, 10.0 };
GLfloat rightHubHighestCoords[3] = { -10.0, -10.0, -10.0, };

GLfloat leftHubFarLeft[3];
GLfloat leftHubFarRight[3];
GLfloat leftHubBottom[3];
GLfloat leftHubTop[3];
GLfloat leftHubFront[3];
GLfloat leftHubBack[3];

GLfloat leftPropellerToOrigin[3];
GLfloat rightPropellerToOrigin[3];

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

void getLowestCoordinates(struct Point point, GLfloat lowestCoords[3]) {
	lowestCoords[0] = point.vertex_x <= lowestCoords[0] ? point.vertex_x : lowestCoords[0];
	lowestCoords[1] = point.vertex_y <= lowestCoords[1] ? point.vertex_y : lowestCoords[1];
	lowestCoords[2] = point.vertex_z <= lowestCoords[2] ? point.vertex_z : lowestCoords[2];
}

void getHighestCoordinates(struct Point point, GLfloat highestCoords[3]) {
	highestCoords[0] = point.vertex_x >= highestCoords[0] ? point.vertex_x : highestCoords[0];
	highestCoords[1] = point.vertex_y >= highestCoords[1] ? point.vertex_y : highestCoords[1];
	highestCoords[2] = point.vertex_z >= highestCoords[2] ? point.vertex_z : highestCoords[2];
}

void setLeftPropellerOffsets() {
	int i;
	for (i = 0; i < 3; i++) {
		leftPropellerToOrigin[i] = -((GLfloat)leftHubHighestCoords[i] + (GLfloat)leftHubLowestCoords[i]) / 2.0;
	}
}

void setRightPropellerOffsets() {
	int i;
	for (i = 0; i < 3; i++) {
		rightPropellerToOrigin[i] = -((GLfloat)rightHubHighestCoords[i] + (GLfloat)rightHubLowestCoords[i]) / 2.0;
	}
}
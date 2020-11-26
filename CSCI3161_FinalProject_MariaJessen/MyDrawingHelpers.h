#pragma once
#define CESSNA_POINT_COUNT 6763
#define PROPELLER_POINT_COUNT 6763

#define DIRECTION_GO_STRAIGHT 0
#define DIRECTION_GO_RIGHT 1
#define DIRECTION_GO_LEFT 2


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
struct FaceNode* propellerFaces[2];

GLfloat leftHubLowestCoords[3] = { 10.0, 10.0, 10.0 };
GLfloat leftHubHighestCoords[3] = { -10.0, -10.0, -10.0 };

GLfloat propellerToOrigin[3];
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

void setPropellerOffsets() {
	int i;
	for (i = 0; i < 3; i++) {
		propellerToOrigin[i] = -((GLfloat)leftHubHighestCoords[i] + (GLfloat)leftHubLowestCoords[i]) / 2.0;
	}
}
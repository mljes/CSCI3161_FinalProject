#pragma once
#include <math.h>

#define CESSNA_POINT_COUNT 6763
#define PROPELLER_POINT_COUNT 6763

#define DIRECTION_GO_STRAIGHT 0
#define DIRECTION_GO_RIGHT 1
#define DIRECTION_GO_LEFT 2

#define MOUNTAIN_START_LEVEL 6
#define MOUNTAIN_RESOLUTION 64

#define MAX_SNOWFLAKES 100

#define SCENE_RADIUS 428
#define SCENE_HEIGHT 385

GLfloat cameraPosition[3] = { 0.0, 10.0, 50.0 };
GLfloat cameraFocusPoint[3] = { 0.0, 0.0, 0.0 };
GLfloat cameraUpVector[3] = { 0.0, 1.0, 0.0 };

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

struct Mountain {
	struct Point points[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION];
	GLfloat colors[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION][4];
	GLfloat peak;
	GLfloat xOffset;
	GLfloat zOffset;
	GLfloat xScale;
	GLfloat yScale;
	GLfloat zScale;
};

struct Mountain mountain1;
struct Mountain mountain2;
struct Mountain mountain3;
struct Mountain mountain4;

struct FaceNode* planeFaceLists[33];
struct FaceNode* propellerFaces[2];

GLfloat leftHubLowestCoords[3] = { 10.0, 10.0, 10.0 };
GLfloat leftHubHighestCoords[3] = { -10.0, -10.0, -10.0 };

GLfloat propellerToOrigin[3];
GLfloat rightPropellerToOrigin[3];

struct Snowflake {
	struct Point center;
	GLfloat initialHeight;
};

struct Snowflake snowflakes[100];

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

int mountainRow = 0;
int mountainCol = 0;

void generateMountainGrid(int mountainID, struct Point point1, struct Point point2, struct Point point3, struct Point point4, int level) {
	struct Point mid0;
	struct Point mid1;
	struct Point mid2;
	struct Point mid3;
	struct Point centrePoint;


	int i;
	if (level > 0) {
		mid0.vertex_x = (point1.vertex_x + point2.vertex_x) / 2;
		mid0.vertex_y = ((point1.vertex_y + point2.vertex_y) / 2) + rand() % (int)(pow(2, level));
		mid0.vertex_z = (point1.vertex_z + point2.vertex_z) / 2;

		mid1.vertex_x = (point2.vertex_x + point3.vertex_x) / 2;
		mid1.vertex_y = ((point2.vertex_y + point3.vertex_y) / 2) + rand() % (int)(pow(2, level));
		mid1.vertex_z = (point2.vertex_z + point3.vertex_z) / 2;

		mid2.vertex_x = (point3.vertex_x + point4.vertex_x) / 2;
		mid2.vertex_y = ((point3.vertex_y + point4.vertex_y) / 2) + rand() % (int)(pow(2, level));
		mid2.vertex_z = (point3.vertex_z + point4.vertex_z) / 2;

		mid3.vertex_x = (point4.vertex_x + point1.vertex_x) / 2;
		mid3.vertex_y = ((point4.vertex_y + point1.vertex_y) / 2) + rand() % (int)(pow(2, level));
		mid3.vertex_z = (point4.vertex_z + point1.vertex_z) / 2;

		centrePoint.vertex_x = (point1.vertex_x + point3.vertex_x) / 2;
		centrePoint.vertex_y = ((point1.vertex_y + point3.vertex_y) / 2) + rand() % (int)(pow(2, level));
		centrePoint.vertex_z = (point1.vertex_z + point3.vertex_z) / 2;

		generateMountainGrid(mountainID, point1, mid0, centrePoint, mid3, level - 1);
		generateMountainGrid(mountainID, mid0, point2, mid1, centrePoint, level - 1);
		generateMountainGrid(mountainID, centrePoint, mid1, point3, mid2, level - 1);
		generateMountainGrid(mountainID, mid3, centrePoint, mid2, point4, level - 1);
	}
	else {
		// ensure there are no openings in the edges of the mountain by pulling edge point heights to 0
		if (point1.vertex_x == 0 || point1.vertex_x >= (MOUNTAIN_RESOLUTION - 1)) {
			point1.vertex_y = 0;
		}

		switch (mountainID) {
		case 1:
			mountain1.points[(int)point1.vertex_x][(int)point1.vertex_z] = point1;
			mountain1.peak = point1.vertex_y > mountain1.peak ? point1.vertex_y : mountain1.peak;
			break;
		case 2:
			mountain2.points[(int)point1.vertex_x][(int)point1.vertex_z] = point1;
			mountain2.peak = point1.vertex_y > mountain2.peak ? point1.vertex_y : mountain2.peak;
			break;
		case 3:
			mountain3.points[(int)point1.vertex_x][(int)point1.vertex_z] = point1;
			mountain3.peak = point1.vertex_y > mountain3.peak ? point1.vertex_y : mountain3.peak;
			break;
		case 4:
			mountain4.points[(int)point1.vertex_x][(int)point1.vertex_z] = point1;
			mountain4.peak = point1.vertex_y > mountain4.peak ? point1.vertex_y : mountain4.peak;
			break;
		}
	}
}

void generateSnowFlakes() {
	int i;
	for (i = 0; i < MAX_SNOWFLAKES; i++) {
		snowflakes[i].center.vertex_x = (rand() % (40)) - 20;
		snowflakes[i].center.vertex_y = (rand() % 20) + cameraPosition[1] + 20;
		snowflakes[i].center.vertex_z = (rand() % (40)) - 20;
		snowflakes[i].initialHeight = snowflakes[i].center.vertex_y;
	}
}

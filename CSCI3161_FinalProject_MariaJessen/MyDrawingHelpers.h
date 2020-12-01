#pragma once
#include <math.h>

#define CESSNA_POINT_COUNT 6763
#define PROPELLER_POINT_COUNT 6763

#define DIRECTION_GO_STRAIGHT 0
#define DIRECTION_GO_RIGHT 1
#define DIRECTION_GO_LEFT 2

#define MOUNTAIN_START_LEVEL 6
#define MOUNTAIN_RESOLUTION 64

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

struct Point mountain1Points[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION];

int mountain1PointCount = 1;

struct FaceNode* planeFaceLists[33];
struct FaceNode* propellerFaces[2];

GLfloat leftHubLowestCoords[3] = { 10.0, 10.0, 10.0 };
GLfloat leftHubHighestCoords[3] = { -10.0, -10.0, -10.0 };

GLfloat propellerToOrigin[3];
GLfloat rightPropellerToOrigin[3];

const GLfloat snowyAltitude = 30.0;

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

void generateMountainGrid(struct Point point1, struct Point point2, struct Point point3, struct Point point4, int level) {
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

		generateMountainGrid(point1, mid0, centrePoint, mid3, level - 1);
		generateMountainGrid(mid0, point2, mid1, centrePoint, level - 1);
		generateMountainGrid(centrePoint, mid1, point3, mid2, level - 1);
		generateMountainGrid(mid3, centrePoint, mid2, point4, level - 1);
	}
	else {
		mountain1Points[(int)point1.vertex_x][(int)point1.vertex_z] = point1;
		

		printf("%d: POINT [%f %f %f] SAVED TO [%d, %d]\n", mountain1PointCount, point1.vertex_x, point1.vertex_y, point1.vertex_z, (int)point1.vertex_x, (int)point1.vertex_z);
		/*
		printf("%d: POINT [%f %f %f] SAVED TO [%d, %d]\n", mountain1PointCount, point1.vertex_x, point1.vertex_y, point1.vertex_z, mountainRow, mountainCol);

		if (mountain1PointCount % 4 == 0) printf("\n");

		mountain1PointCount++;

		if (mountainCol == MOUNTAIN_RESOLUTION) mountainRow++;
		
		if (mountainCol <= MOUNTAIN_RESOLUTION) 
			mountainCol++;
		else {
			mountainCol = 0;
		}

		if (mountainRow == MOUNTAIN_RESOLUTION) printf("*DONE*\n");
		*/
	}
}


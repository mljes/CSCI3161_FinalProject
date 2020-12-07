#pragma once
/*

CSCI3161 FINAL PROJECT
MARIA JESSEN B00743170

Helpful functions, values and structs for performing drawing operations

*/

#include <math.h>

// point counts for reading from files
#define CESSNA_POINT_COUNT 6763
#define PROPELLER_POINT_COUNT 6763

// values for generating mountain points
#define MOUNTAIN_START_LEVEL 6
#define MOUNTAIN_RESOLUTION 64

// snowflake and raindrop counts
#define MAX_SNOWFLAKES 3000
#define MAX_RAINDROPS 3000

// radius and height of scene cylinder
#define SCENE_RADIUS 428
#define SCENE_HEIGHT 385

// camera values
GLfloat cameraPosition[3] = { 0.0, 10.0, 50.0 };
GLfloat cameraFocusPoint[3] = { 0.0, 0.0, 0.0 };
GLfloat cameraUpVector[3] = { 0.0, 1.0, 0.0 };

// Point structure to store vertex with normal
struct Point {
	GLfloat vertex_x;
	GLfloat vertex_y;
	GLfloat vertex_z;
	
	GLfloat normal_x;
	GLfloat normal_y;
	GLfloat normal_z;
};

// lists of points
struct Point planePoints[CESSNA_POINT_COUNT+2];
struct Point propellerPoints[PROPELLER_POINT_COUNT + 2];

// Mountain structure to hold mountain color, transformation and point values
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

// mountains to be drawn
struct Mountain mountain1;
struct Mountain mountain2;
struct Mountain mountain3;
struct Mountain mountain4;

// arrays of linkedlists to hold face points
struct FaceNode* planeFaceLists[33];
struct FaceNode* propellerFaces[2];

// hub extremes to determine center point for propeller placement
GLfloat leftHubLowestCoords[3] = { 10.0, 10.0, 10.0 };
GLfloat leftHubHighestCoords[3] = { -10.0, -10.0, -10.0 };

// offsets for propellers
GLfloat propellerToOrigin[3];

// values to draw 1 snowflake
struct Snowflake {
	struct Point center;
	GLfloat initialHeight;
	GLfloat rippleRadius;
	GLfloat rippleOpacity;
	GLfloat showRipple;
};

// values to draw 1 raindrop
struct Raindrop {
	struct Point bottom;
	GLfloat initialHeight;
	GLfloat length;
	GLfloat rippleRadius;
	GLfloat rippleOpacity;
	GLboolean showRipple;
};

// arrays of snowflakes and raindrops
struct Snowflake snowflakes[MAX_SNOWFLAKES];
struct Raindrop raindrops[MAX_RAINDROPS];

/// <summary>
/// Compare a point to saved lowest x, y, z coordinates, update lowest if the point has lower
/// </summary>
/// <param name="point"></param>
/// <param name="lowestCoords"></param>
void getLowestCoordinates(struct Point point, GLfloat lowestCoords[3]) {
	lowestCoords[0] = point.vertex_x <= lowestCoords[0] ? point.vertex_x : lowestCoords[0];
	lowestCoords[1] = point.vertex_y <= lowestCoords[1] ? point.vertex_y : lowestCoords[1];
	lowestCoords[2] = point.vertex_z <= lowestCoords[2] ? point.vertex_z : lowestCoords[2];
}

/// <summary>
/// Compare a point to a save highest x, y, z coordinates, update highest if the point has higher
/// </summary>
/// <param name="point"></param>
/// <param name="highestCoords"></param>
void getHighestCoordinates(struct Point point, GLfloat highestCoords[3]) {
	highestCoords[0] = point.vertex_x >= highestCoords[0] ? point.vertex_x : highestCoords[0];
	highestCoords[1] = point.vertex_y >= highestCoords[1] ? point.vertex_y : highestCoords[1];
	highestCoords[2] = point.vertex_z >= highestCoords[2] ? point.vertex_z : highestCoords[2];
}

/// <summary>
/// Use highest and lowest x, y, z, coordinates of hub to get center of hub for propeller placement
/// </summary>
void setPropellerOffsets() {
	int i;
	for (i = 0; i < 3; i++) {
		propellerToOrigin[i] = -((GLfloat)leftHubHighestCoords[i] + (GLfloat)leftHubLowestCoords[i]) / 2.0;
	}
}

/// <summary>
/// Reset all snowflake heights to top
/// </summary>
void resetSnowflakeHeights() {
	int i;
	for (i = 0; i < MAX_SNOWFLAKES; i++) {
		snowflakes[i].center.vertex_y = snowflakes[i].initialHeight;
	}
}

/// <summary>
/// Reset all raindrop heights to top
/// </summary>
void resetRaindropHeights() {
	int i;
	for (i = 0; i < MAX_RAINDROPS; i++) {
		raindrops[i].bottom.vertex_y = raindrops[i].initialHeight;
	}
}

/// <summary>
/// Recursively generates squares of points, saving the top right corner from each and adding a random value to the height of each point.
/// The random range is halved on each new recursion.
/// </summary>
/// <param name="mountainID"></param>
/// <param name="point1"></param>
/// <param name="point2"></param>
/// <param name="point3"></param>
/// <param name="point4"></param>
/// <param name="level"></param>
void generateMountainGrid(int mountainID, struct Point point1, struct Point point2, struct Point point3, struct Point point4, int level) {
	struct Point mid0;
	struct Point mid1;
	struct Point mid2;
	struct Point mid3;
	struct Point centrePoint;

	int i;
	if (level > 0) {
		// Get midpoints for each line of current square
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

		// get center point of square
		centrePoint.vertex_x = (point1.vertex_x + point3.vertex_x) / 2;
		centrePoint.vertex_y = ((point1.vertex_y + point3.vertex_y) / 2) + rand() % (int)(pow(2, level));
		centrePoint.vertex_z = (point1.vertex_z + point3.vertex_z) / 2;

		// run function again with each square (set of 4 points)
		generateMountainGrid(mountainID, point1, mid0, centrePoint, mid3, level - 1);
		generateMountainGrid(mountainID, mid0, point2, mid1, centrePoint, level - 1);
		generateMountainGrid(mountainID, centrePoint, mid1, point3, mid2, level - 1);
		generateMountainGrid(mountainID, mid3, centrePoint, mid2, point4, level - 1);
	}
	else { // base case
		// ensure there are no openings in the edges of the mountain by pulling edge point heights to 0
		if (point1.vertex_x == 0 || point1.vertex_x >= (MOUNTAIN_RESOLUTION - 1)) {
			point1.vertex_y = 0;
		}

		// save point to appropriate array, at index determined by the x,z coordinates of the point
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

/// <summary>
/// Reset the ripple radius and opacity and set it invisible
/// </summary>
/// <param name="radius"></param>
/// <param name="opacity"></param>
/// <param name="visibility"></param>
void resetRippleValues(GLfloat * radius, GLfloat * opacity, GLboolean * visibility) {
	*radius = (GLfloat)0.0;
	*opacity = (GLfloat)0.5;
	visibility = GL_FALSE;
}

/// <summary>
/// Generate an array of snowflakes at random start heights and x, z coordinates
/// </summary>
void generateSnowFlakes() {
	int i;
	for (i = 0; i < MAX_SNOWFLAKES; i++) {
		// generate high y coordinate and x, z within the radius of the cylinder
		snowflakes[i].center.vertex_x = (rand() % (2 * SCENE_RADIUS)) - SCENE_RADIUS;
		snowflakes[i].center.vertex_y = (rand() % 200) + 20;
		snowflakes[i].center.vertex_z = (rand() % (2 * SCENE_RADIUS)) - SCENE_RADIUS;
		snowflakes[i].initialHeight = snowflakes[i].center.vertex_y;

		resetRippleValues(&snowflakes[i].rippleRadius, &snowflakes[i].rippleOpacity, &snowflakes[i].showRipple);
	}
}

/// <summary>
/// Generate an array of raindrops at random start heights and x, z coordinates
/// </summary>
void generateRainDrops() {
	int i, j;
	for (i = 0; i < MAX_RAINDROPS; i++) {
		// same logic as for snowflakes - set initial height and values for bottom of raindrop
		raindrops[i].bottom.vertex_x = (rand() % (2 * SCENE_RADIUS)) - SCENE_RADIUS;
		raindrops[i].bottom.vertex_y = (rand() % 200) + 20;
		raindrops[i].bottom.vertex_z = (rand() % (2 * SCENE_RADIUS)) - SCENE_RADIUS;
		raindrops[i].initialHeight = raindrops[i].bottom.vertex_y;

		//give each raindrop a very small tail
		raindrops[i].length = (float)(rand() % 51) / 100.0;

		resetRippleValues(&raindrops[i].rippleRadius, &raindrops[i].rippleOpacity, &raindrops[i].showRipple);
	}
}

/// <summary>
/// Make ripple a bit bigger and a bit less opaque
/// </summary>
/// <param name="radius"></param>
/// <param name="opacity"></param>
/// <param name="showRipple"></param>
void updateRippleRadius(GLfloat* radius, GLfloat* opacity, GLboolean* showRipple) {
	*radius += 0.01;
	*opacity -= 0.01;

	if (*opacity <= 0.0) {
		*radius = 0.0;
		*opacity = 0.5;
		*showRipple = GL_FALSE; // stop drawing ripple once it has faded out of visibility
	}
}
#include <freeglut.h>
#include <stdio.h>
#include <string.h>
#include "MyDrawingHelpers.h"
#include "LinkedList.h"
#include "MyColors.h"
#include "LoadTexture.h"
#include <time.h>
#include <math.h>
#include "TextHelpers.h"

#define M_PI 3.14159265358979323846

#define KEY_PAGE_UP 104
#define KEY_PAGE_DOWN 105
#define KEY_ARROW_UP 101
#define KEY_ARROW_DOWN 103

#define SPEED_INCREMENT 0.2

int windowWidth = 800;
int windowHeight = 800;

GLenum polygonMode = GL_LINE;

const GLint gridSize = 500;
const GLfloat gridSectionWidth = 2.0;

GLboolean isFullscreen = GL_FALSE;

GLfloat propellerRotationDeg = 0.0;
GLfloat sceneRotationDeg = 0.0;
GLfloat sceneRotationDelta = 0.0;

GLfloat planeToCameraOffset[3] = { 0.0, -1.0, -2.0 };

GLfloat planeForwardDelta = SPEED_INCREMENT;
GLfloat planeTravel = 0.0;

GLfloat planeRollDeg = 0.0;

GLint currentPlaneDirection = DIRECTION_GO_STRAIGHT;

GLboolean simpleSceneMode = GL_TRUE;
GLboolean fogMode = GL_TRUE;
GLboolean mountainTexturedMode = GL_FALSE;
GLboolean showMountains = GL_FALSE;
GLboolean showSnow = GL_FALSE;
GLboolean snowIsAccumulating = GL_FALSE;
GLboolean showRain = GL_FALSE;
GLboolean transitionSkyToClear = GL_FALSE;

GLuint skyTextureID;
GLuint seaTextureID;
GLuint mountTextureID;
GLuint altSkyTextureID;

GLfloat snowDensityPlane = 0.0;
GLfloat snowDensityMountains = 0.0;

GLint mountainShine = 0;
GLint planeShine = 50;

int specialPart = 1;

GLfloat snowflakeFallDelta = 0.0;

GLfloat planeYawAngle = 0.0;

GLfloat oldMouseX = -1;
GLfloat centreMouseBoundary = 400;

void setMaterialProperties(GLfloat diffuse[4], GLfloat ambient[4], GLfloat specular[4], GLfloat shine) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
}

void setMountainPointColors(GLfloat peak, GLfloat colors[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION][4], struct Point points[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION]) {
	int i, j;
	for (i = 0; i < MOUNTAIN_RESOLUTION; i++) {
		for (j = 0; j < MOUNTAIN_RESOLUTION; j++) {
			GLfloat height = points[i][j].vertex_y;
			if (height > (peak - MOUNTAIN_START_LEVEL)) {
				colors[i][j][0] = color_array_white[0];
				colors[i][j][1] = color_array_white[1];
				colors[i][j][2] = color_array_white[2];
			}
			else if (height > (peak - 2 * MOUNTAIN_START_LEVEL)) {
				colors[i][j][0] = color_array_grey[0];
				colors[i][j][1] = color_array_grey[1];
				colors[i][j][2] = color_array_grey[2];
			}
			else {
				GLfloat redModifier = ((float)((rand() % 401) - 200.0) / 1000.0);
				GLfloat greenModifier = ((float)((rand() % 101)) / 1000.0);
				GLfloat blueModifier = ((float)((rand() % 201) - 100.0) / 1000.0);

				colors[i][j][0] = color_array_dark_green[0] + redModifier;
				colors[i][j][1] = color_array_dark_green[1] - greenModifier;
				colors[i][j][2] = color_array_dark_green[2] + blueModifier;
			}	
		}
	}
}

void setPartColor(int partIndex) {
	GLfloat ambientColor[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat specularColor[] = { 1.0, 1.0, 1.0, 1.0 };

	if (partIndex == specialPart) {
		setMaterialProperties(color_array_red, ambientColor, specularColor, 50);
		return;
	}

	if (partIndex <= 3) {
		setMaterialProperties(color_array_yellow, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 5) {
		setMaterialProperties(color_array_black, ambientColor, specularColor, 50);
	}
	else if (partIndex == 6) {
		setMaterialProperties(color_array_light_purple, ambientColor, specularColor, 50);
	}
	else if (partIndex == 7) {
		setMaterialProperties(color_array_blue, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 13) {
		setMaterialProperties(color_array_yellow, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 25) {
		setMaterialProperties(color_array_blue, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 32) {
		setMaterialProperties(color_array_yellow, ambientColor, specularColor, 50);
	}
}

void loadPlanePoints(char filename[], struct Point* points, struct FaceNode* faces[], int maxVerticesRead) {
	FILE* planeFile;
	errno_t result = fopen_s(&planeFile, filename, "r");

	if (planeFile == 0) return;

	int pointCount = 1;
	int partCount = -1; 
	char typeChar = 0;
	int linesRead = 0;

	int typeCharScanned = fscanf_s(planeFile, "%c ", &typeChar, 2);

	while (typeCharScanned > 0) {
		if (typeChar == 'v') {
			GLfloat xPos, yPos, zPos;

			fscanf_s(planeFile, "%f %f %f\n", &xPos, &yPos, &zPos);
			struct Point newPoint = { .vertex_x=xPos, .vertex_y=yPos, .vertex_z=zPos, .normal_x=0.0, .normal_y=0.0, .normal_z=0.0 }; // create a point structure from the coordinates

			points[pointCount] = newPoint;

			if (pointCount == maxVerticesRead) {
				pointCount = 1;
			}
			else {
				pointCount++;
			}
		}
		else if (typeChar == 'n'){
			GLfloat xPos, yPos, zPos;
			fscanf_s(planeFile, "%f %f %f\n", &xPos, &yPos, &zPos);

			points[pointCount].normal_x = xPos;
			points[pointCount].normal_y = yPos;
			points[pointCount].normal_z = zPos;

			pointCount++;
		}
		else if (typeChar == 'g') {
			char partName[20];
			fgets(&partName, 19, planeFile);

			partCount++;

			printf("(%d) LOADING %s\n", partCount, partName);

			faces[partCount] = NULL; //malloc(sizeof(struct FaceNode));
		}
		else if (typeChar == 'f') {
			int pointIndex = 0;
			int scanned = fscanf_s(planeFile, "%d", &pointIndex);

			struct PointNode* list = NULL; //malloc(sizeof(struct PointNode));

			while (scanned != 0 && scanned != EOF) {
				//printf("%d ", pointIndex);
				struct Point point = points[pointIndex];

				if (partCount == 32) { // hub1 (left)
					getLowestCoordinates(point, leftHubLowestCoords);
					getHighestCoordinates(point, leftHubHighestCoords);
				}

				// create a new PointNode from that Point, insert it into the list 
				list = insertPoint(list, point);

				scanned = fscanf_s(planeFile, "%d", &pointIndex);
			}

			//printf("\n");

			GLfloat color[4] = { 1.0, 1.0, 1.0, 1.0 };
			faces[partCount] = insertFace(faces[partCount], list, color);
		}

		//typeChar = fgetc(planeFile);
		//printf("NEXT CHAR: %c\n", typeChar);
		linesRead++;
		typeCharScanned = fscanf_s(planeFile, "%c ", &typeChar, 2);
	}
}

void drawVertexWithNormal(struct PointNode* currPoint) {
	glBegin(GL_POLYGON);
	while (currPoint != NULL) {
		struct Point point = currPoint->point;

		glNormal3f(point.normal_x, point.normal_y, point.normal_z);
		glVertex3f(point.vertex_x, point.vertex_y, point.vertex_z);

		currPoint = currPoint->next;
	}
	glEnd();
}

void drawVertexWithTexture(struct Point points[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION], GLfloat colors[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION][4], int i, int j) {
	if (!mountainTexturedMode) {
		//GLfloat* ambientColor = mountain1Colors[i][j];
		GLfloat diffuseColor[4] = { 0.1, 0.1, 0.1, 1.0 };
		GLfloat specularColor[4] = { 0.0, 0.0, 0.0, 1.0 };

		setMaterialProperties(diffuseColor, colors[i][j], specularColor, 100);
	}

	glNormal3f(3 * points[i][j].vertex_x, 3 * points[i][j].vertex_y, 3 * points[i][j].vertex_z);

	if (mountainTexturedMode) glTexCoord2f(points[i][j].vertex_x / MOUNTAIN_RESOLUTION, points[i][j].vertex_z / MOUNTAIN_RESOLUTION);

	glVertex3f(points[i][j].vertex_x, points[i][j].vertex_y, points[i][j].vertex_z);
}

void drawMountain(struct Mountain mountain) {
	int i, j;
	glPushMatrix();
	
	glScalef(mountain.xScale, mountain.yScale, mountain.zScale);
	glTranslatef(mountain.xOffset, -(mountain.peak / 2), mountain.zOffset);
	setMaterialProperties(color_array_white, color_array_white, color_array_white, 0);

	if (mountainTexturedMode) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mountTextureID);
	}
	
	for (i = 0; i < MOUNTAIN_RESOLUTION - 1; i++) {
		glBegin(GL_QUADS);
		for (j = 0; j < MOUNTAIN_RESOLUTION - 1; j++) {
			drawVertexWithTexture(mountain.points, mountain.colors, i, j);
			drawVertexWithTexture(mountain.points, mountain.colors, i, j+1);
			drawVertexWithTexture(mountain.points, mountain.colors, i+1, j+1);
			drawVertexWithTexture(mountain.points, mountain.colors, i+1, j);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawCessna() {
	int i = 0;
	
	for (i = 0; i < 33; i++) {
		struct FaceNode* currFace = planeFaceLists[i];

		while (currFace != NULL) {
			struct PointNode* currPoint = currFace->pointList;

			setPartColor(i);
			drawVertexWithNormal(currPoint);

			currFace = currFace->next;
		}
	}
}

void drawPropeller() {
	// get correct set of propeller faces to draw
	GLfloat colors[2][4] = {
		COLOR_WHITE,
		COLOR_RED
	};

	int i;
	for (i = 0; i < 2; i++) {
		struct FaceNode* currFace = propellerFaces[i];

		while (currFace != NULL) {
			struct PointNode* currPoint = currFace->pointList;

			//GLfloat diffuseColor[] = { 0.1, 0.1, 0.1, 1.0 };
			GLfloat specularColor[] = { 1.0, 1.0, 1.0, 1.0 };
			setMaterialProperties(colors[i], colors[i], specularColor, 50);
			drawVertexWithNormal(currPoint);

			currFace = currFace->next;
		}
	}
	
}

/// <summary>
/// Draws a 3D sphere at the origin with the specified color and radius. Reused from my Assignment 2 code. 
/// </summary>
/// <param name="color"></param>
/// <param name="radius"></param>
void drawSphere(GLfloat radius, GLfloat color[4]) {
	glColor4fv(color);

	setMaterialProperties(color, color, color, 50);

	gluSphere(gluNewQuadric(), radius, 16, 16); // draw a sphere with specified radius, 16 longitudinal segments and 16 latitudinal segments
}

void drawGrid() {
	GLint lowerLimit = -(gridSize / 2);
	GLint upperLimit = -lowerLimit;

	int i, j;

	GLfloat color[4] = COLOR_LIGHT_PURPLE;
	color[3] = 0.5;
;
	setMaterialProperties(color, color, color_array_white, 50);

	glBegin(GL_QUADS);
	for (i = lowerLimit; i < upperLimit; i++) {
		for (j = lowerLimit; j < upperLimit; j++) {
			GLfloat currentGridPoints[4][3] = {
				{gridSectionWidth * i, 0.0, gridSectionWidth * j},
				{gridSectionWidth * i, 0.0, gridSectionWidth * j + gridSectionWidth},
				{gridSectionWidth * i + gridSectionWidth, 0.0, gridSectionWidth * j + gridSectionWidth},
				{gridSectionWidth * i + gridSectionWidth, 0.0, gridSectionWidth * j}
			};

			glNormal3fv(currentGridPoints[0]);
			glVertex3fv(currentGridPoints[0]);

			glNormal3fv(currentGridPoints[1]);
			glVertex3fv(currentGridPoints[1]);

			glNormal3fv(currentGridPoints[2]);
			glVertex3fv(currentGridPoints[2]);

			glNormal3fv(currentGridPoints[3]);
			glVertex3fv(currentGridPoints[3]);
		}
	}
	glEnd();

}

void drawAxes() {
	GLfloat axisLength = 5.0;
	glLineWidth(5.0); // use lines of width 5
	glBegin(GL_LINES);

	// +x axis
	setMaterialProperties(color_array_red, color_array_red, color_array_red, 50);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(axisLength, 0.0, 0.0);

	// +y axis
	setMaterialProperties(color_array_green, color_array_green, color_array_green, 50);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, axisLength, 0.0);

	// +z axis
	setMaterialProperties(color_array_blue, color_array_blue, color_array_blue, 50);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, axisLength);

	glEnd();

	glLineWidth(1.0); // reset line width

	GLfloat sphereColor[] = { 1.0, 1.0, 1.0, 1.0 };
	drawSphere(0.5, sphereColor);
}

void drawFog(GLfloat density, GLfloat color[4]) {
	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, color);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, density);
}

void drawSceneryCylinder() {
	
	GLUquadricObj *diskPtr, *cylinderPtr;
	diskPtr = gluNewQuadric();
	cylinderPtr = gluNewQuadric();

	int quadricDrawingStyle = polygonMode == GL_LINE ? GLU_LINE : GLU_FILL;

	gluQuadricDrawStyle(diskPtr, quadricDrawingStyle);
	gluQuadricNormals(diskPtr, GLU_SMOOTH);
	gluQuadricTexture(diskPtr, GL_TRUE);

	gluQuadricDrawStyle(cylinderPtr, quadricDrawingStyle);
	gluQuadricNormals(cylinderPtr, GLU_SMOOTH);
	gluQuadricTexture(cylinderPtr, GL_TRUE);
	
	glPushMatrix();
	setMaterialProperties(color_array_white, color_array_white, color_array_white, 100);
	glEnable(GL_TEXTURE_2D);
	glRotatef(270.0, 1.0, 0.0, 0.0);

	if (fogMode) {
		if (color_array_white_scene[3] > 0)
			drawFog(0.005, color_array_rose_transparent);
		else
			drawFog(0.005, color_array_grey_transparent);
	}

	glBindTexture(GL_TEXTURE_2D, seaTextureID);
	gluDisk(diskPtr, 0.0, 600, 100, 100);
	glDisable(GL_FOG);

	glTranslatef(0.0, -10.0, 0.0);

	if (showSnow || showRain || transitionSkyToClear) {
		setMaterialProperties(color_array_white, color_array_white, color_array_white, 100);
		glBindTexture(GL_TEXTURE_2D, altSkyTextureID);
		gluCylinder(cylinderPtr, SCENE_RADIUS + 1, SCENE_RADIUS + 1, SCENE_HEIGHT, 100, 200);
		
		glEnable(GL_COLOR_MATERIAL);
		glColor4fv(color_array_white_scene);
	}

	if (color_array_white_scene[3] > 0) {
		setMaterialProperties(color_array_white_scene, color_array_white_scene, color_array_white_scene, 100);
		glBindTexture(GL_TEXTURE_2D, skyTextureID);
		gluCylinder(cylinderPtr, SCENE_RADIUS, SCENE_RADIUS, SCENE_HEIGHT, 100, 200);
	}

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
}

void drawSnowflakeArm(struct Point center, GLfloat endX, GLfloat endY) {
	glNormal3f(center.vertex_x, center.vertex_y, center.vertex_z);
	glVertex3f(center.vertex_x, center.vertex_y, center.vertex_z);

	glNormal3f(center.vertex_x + endX, center.vertex_y + endY, center.vertex_z);
	glVertex3f(center.vertex_x + endX, center.vertex_y + endY, center.vertex_z);
}

void drawRipples(GLfloat xPos, GLfloat zPos, GLfloat opacity, GLfloat radius) {
	int i;

	glPushMatrix();
	glTranslatef(xPos, 0.01, zPos);

	glEnable(GL_COLOR_MATERIAL);
	glColor4f(color_array_grey[0], color_array_grey[1], color_array_grey[2], opacity);

	glBegin(GL_LINE_STRIP);

	// Draw the top half of the circle using the circle formula
	for (i = (-radius * 100.0); i <= (radius * 100.0); i++) {
		GLfloat xPos = i / 100.0;
		GLfloat zPos = radius * sqrtf(1 - ((xPos / radius) * (xPos / radius))); // calculate z pos using circle formula

		glVertex3f(xPos, 0.0, zPos); // draw a vertex in the line strip
	}
	// Draw bottom half of circle
	for (i = (radius * 100.0); i >= (-radius * 100.0); i--) {
		GLfloat xPos = i / 100.0;
		GLfloat zPos = -radius * sqrtf(1 - ((xPos / radius) * (xPos / radius))); // calculate z pos using circle formula

		glVertex3f(xPos, 0.0, zPos); // draw a vertex in the line strip
	}

	glEnd();
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
}

void drawSnowflakes() {
	int i; 
	glPushMatrix();
	setMaterialProperties(color_array_white, color_array_white, color_array_white, 200);

	for (i = 0; i < MAX_SNOWFLAKES; i++) {
		glBegin(GL_LINES);

		drawSnowflakeArm(snowflakes[i].center, 0.1, 0.0);
		drawSnowflakeArm(snowflakes[i].center, -0.1, 0.0);
		drawSnowflakeArm(snowflakes[i].center, 0.0, 0.1);
		drawSnowflakeArm(snowflakes[i].center, 0.0, -0.1);

		drawSnowflakeArm(snowflakes[i].center, 0.06, 0.06);
		drawSnowflakeArm(snowflakes[i].center, 0.06, -0.06);
		drawSnowflakeArm(snowflakes[i].center, -0.06, -0.06);
		drawSnowflakeArm(snowflakes[i].center, -0.06, 0.06);

		glEnd();

		snowflakes[i].center.vertex_y -= 0.1;

		if (snowflakes[i].center.vertex_y <= 0.0) {
			snowIsAccumulating = GL_TRUE;
			snowflakes[i].showRipple = GL_TRUE;
			snowflakes[i].center.vertex_y = snowflakes[i].initialHeight;
		}

		if (snowflakes[i].showRipple) {
			drawRipples(snowflakes[i].center.vertex_x, snowflakes[i].center.vertex_z, snowflakes[i].rippleOpacity, snowflakes[i].rippleRadius);
		}
	}

	glPopMatrix();
}

void drawRaindrops() {
	int i;
	glPushMatrix();

	glLineWidth(2.0);
	
	for (i = 0; i < MAX_RAINDROPS; i++) {
		GLfloat length = raindrops[i].length;
		glBegin(GL_LINES);
		setMaterialProperties(color_raindrop_bottom, color_raindrop_bottom, color_raindrop_bottom, 200);

		glNormal3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y, raindrops[i].bottom.vertex_z);
		glVertex3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y, raindrops[i].bottom.vertex_z);

		setMaterialProperties(color_raindrop_top, color_raindrop_top, color_raindrop_top, 200);

		glNormal3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y + length, raindrops[i].bottom.vertex_z);
		glVertex3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y + length, raindrops[i].bottom.vertex_z);

		glEnd();

		raindrops[i].bottom.vertex_y -= 0.5;

		if (raindrops[i].bottom.vertex_y <= 0.0) {
			raindrops[i].showRipple = GL_TRUE;
			raindrops[i].bottom.vertex_y = raindrops[i].initialHeight;
		}

		if (raindrops[i].showRipple) {
			drawRipples(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_z, raindrops[i].rippleOpacity, raindrops[i].rippleRadius);
		}
	}
	glLineWidth(1.0);
	glPopMatrix();
}

void drawInfoText() {
	setMaterialProperties(color_array_green, color_array_green, color_array_green, 200);
	GLint lineLength = 18;
	GLfloat letterWidth = 0.2;
	GLfloat letterHeight = 0.3;
	GLfloat startingX = cameraPosition[0] - letterWidth * (lineLength / 2);

	char text[5][18] = {
		"------------------",
		"WEATHER:  CLEAR   ", 
		"SPEED:    0000KM/H",
		"ALTITUDE: 0000KM  ",
		"------------------"
	};

	setWeatherText(text[1], showSnow, showRain);
	setNumericalText(text[2], planeForwardDelta, 135);
	setNumericalText(text[3], cameraPosition[1] + planeToCameraOffset[1], 15);
	
	int i, j;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 18; j++) {
			glRasterPos3f(startingX + (letterWidth * j), cameraPosition[1] + 2 + (letterHeight * i), cameraPosition[2] - 20);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i][j]);
		}
	}
}

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1.0, };
	GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1.0, };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1.0, };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat lightPosition[] = { 0.0, 200.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
				cameraFocusPoint[0], cameraFocusPoint[1], cameraFocusPoint[2],
				cameraUpVector[0], cameraUpVector[1], cameraUpVector[2]);
	
	glPointSize(20.0);
	setMaterialProperties(color_array_red, color_array_red, color_array_red, 50);
	glBegin(GL_POINTS);
	glVertex3fv(cameraFocusPoint);
	glEnd();

	glPushMatrix();

	if (simpleSceneMode) {
		drawAxes();
		drawGrid();
	}
	else {
		drawSceneryCylinder();
	}

	if (showMountains) {
		if (showSnow) drawFog(snowDensityMountains, color_array_white);
		drawMountain(mountain1);
		drawMountain(mountain2);
		drawMountain(mountain3);
		drawMountain(mountain4);
		glDisable(GL_FOG);
	}

	if (showSnow) {
		drawSnowflakes();
	}
	else if (showRain) {
		drawRaindrops();
	}

	glPopMatrix();

	glPushMatrix();	

	glTranslatef(cameraPosition[0] + planeToCameraOffset[0], cameraPosition[1] + planeToCameraOffset[1], cameraPosition[2] + planeToCameraOffset[2]);
	glRotatef(270.0, 0.0, 1.0, 0.0);
	glRotatef(-planeYawAngle, 0.0, 1.0, 0.0);
	glRotatef(planeRollDeg, 1.0, 0.0, 0.0);
	glScalef(0.5, 0.5, 0.5);

	if (showSnow) drawFog(snowDensityPlane, color_array_white);
	drawCessna();
	glDisable(GL_FOG);

	glPushMatrix();
	glTranslatef(-propellerToOrigin[0], -propellerToOrigin[1], -propellerToOrigin[2]);
	glRotatef(propellerRotationDeg, 1.0, 0.0, 0.0);
	glTranslatef(propellerToOrigin[0], propellerToOrigin[1],propellerToOrigin[2]);
	drawPropeller();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-propellerToOrigin[0], -propellerToOrigin[1], propellerToOrigin[2]);
	glRotatef(propellerRotationDeg, 1.0, 0.0, 0.0);
	glTranslatef(propellerToOrigin[0], propellerToOrigin[1], propellerToOrigin[2]);
	drawPropeller();
	glPopMatrix();

	glPopMatrix();

	drawInfoText();

	glutSwapBuffers(); // send drawing information to OpenGL
}

void myIdle() {
	GLfloat xOffsetPlane = (centreMouseBoundary - oldMouseX) / windowWidth;
	planeYawAngle -= xOffsetPlane / (planeForwardDelta * 10);

	printf("NEW YAW: %f\n", planeYawAngle);

	GLfloat xOffsetCamera = sin(planeYawAngle * M_PI / 180.0) * planeForwardDelta;
	GLfloat zOffsetCamera = cos(planeYawAngle * M_PI / 180.0) * planeForwardDelta;

	cameraPosition[0] += xOffsetCamera;
	cameraPosition[2] -= zOffsetCamera;

	planeToCameraOffset[0] = sin(planeYawAngle * M_PI / 180.0) * 2.0;
	planeToCameraOffset[2] = cos(planeYawAngle * M_PI / 180.0) * -2.0;

	planeRollDeg = abs(planeYawAngle) < 45.0 ? -planeYawAngle : planeRollDeg;

	cameraFocusPoint[0] = cameraPosition[0] + sin(planeYawAngle * M_PI / 180.0) * 50;
	cameraFocusPoint[2] = cameraPosition[2] + cos(planeYawAngle * M_PI / 180.0) * -50;

	printf("CAMERA: X: %f X: %f\n", cameraPosition[0], cameraPosition[2]);
	printf("FOCUS: X: %f Z: %f\n", cameraFocusPoint[0], cameraFocusPoint[2]);

	propellerRotationDeg += 15.0;

	double centreMouseBoundary = windowWidth / 2;

	if (showSnow || showRain) {
		color_array_white_scene[3] = color_array_white_scene[3] <= 0.0 ? 0.0 : (color_array_white_scene[3] - 0.01);
	}

	else if (transitionSkyToClear) {
		if (color_array_white_scene[3] >= 1.0) {
			color_array_white_scene[3] = 1.0;
			transitionSkyToClear = GL_FALSE;
		}
		else {
			color_array_white_scene[3] += 0.01;
		}
	}

	if (showSnow) {
		snowflakeFallDelta -= 10.0;

		if (snowIsAccumulating) {
			snowDensityPlane = snowDensityPlane >= 1.0 ? 1.0 : snowDensityPlane + 0.01;
			snowDensityMountains = snowDensityMountains >= 1.0 ? 1.0 : snowDensityMountains + 0.0001;
		}

		int i, j;
		for (i = 0; i < MAX_SNOWFLAKES; i++) {
			if (snowflakes[i].showRipple) {
				updateRippleRadius(&snowflakes[i].rippleRadius, &snowflakes[i].rippleOpacity, &snowflakes[i].showRipple);
			}
		}
	}

	if (showRain) {
		mountainShine += 10.0;
		planeShine += 10.0;

		int i, j;
		for (i = 0; i < MAX_RAINDROPS; i++) {
			if (raindrops[i].showRipple) {
				updateRippleRadius(&raindrops[i].rippleRadius, &raindrops[i].rippleOpacity, &raindrops[i].showRipple);
			}
		}
	}

	planeTravel += planeForwardDelta;

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y) {
	printf("PRESSED %c\n", key);
	switch (key) {
	case 'w': // toggle line/fill mode for drawing polygons
		polygonMode = polygonMode == GL_LINE ? GL_FILL : GL_LINE;
		break;
	case 'f':
		if (isFullscreen) { // already fullscreen, want to go back to windowed
			glutReshapeWindow(windowWidth, windowHeight);
		}
		else { // currently windowed, want to go to fullscreen
			glutFullScreen();
		}
		isFullscreen = !isFullscreen;
		break;
	case 'q':
		exit(0);
		break;
	case 's':
		simpleSceneMode = !simpleSceneMode;
		break;
	case 'b':
		fogMode = !fogMode;
		break;
	case 'm':
		showMountains = !showMountains;
		break;
	case 't':
		mountainTexturedMode = !mountainTexturedMode;
		break;
	case 'x':
		showSnow = !showSnow;
		if (showSnow) {
			showRain = GL_FALSE;			
		}
		else {
			snowIsAccumulating = GL_FALSE;
			snowDensityMountains = 0.0;
			snowDensityPlane = 0.0;
			//color_array_white_scene[3] = 1.0;
			transitionSkyToClear = GL_TRUE;
		}
		break;
	case 'r':
		showRain = !showRain;
		if (showRain) {
			showSnow = GL_FALSE;
		}
		else {
			GLint mountainShine = 0;
			GLint planeShine = 50;
			//color_array_white_scene[3] = 1.0;
			transitionSkyToClear = GL_TRUE;
		}
	}
}

void mySpecialKeyboard(int key, int x, int y) {
	const GLfloat verticalDelta = 0.2;

	printf("Pressed: %d\n", key);
	switch (key) {
	case KEY_PAGE_UP: // increase forward speed
		planeForwardDelta += SPEED_INCREMENT;
		break;
	case KEY_PAGE_DOWN: // decrease forward speed, but keep above 0
		planeForwardDelta -= SPEED_INCREMENT;
		planeForwardDelta = planeForwardDelta <= 0.0 ? SPEED_INCREMENT : planeForwardDelta;
		break;
	case KEY_ARROW_UP: // move plane up
		cameraPosition[1] += verticalDelta;
		cameraFocusPoint[1] += verticalDelta;
		break;
	case KEY_ARROW_DOWN: // move plane down
		cameraPosition[1] -= verticalDelta;
		cameraFocusPoint[1] -= verticalDelta;
		break;
	}
}

void myPassiveMotion(int x, int y) {
	oldMouseX = x;

	centreMouseBoundary = (windowWidth / 2);

	if (x != centreMouseBoundary) {
		currentPlaneDirection = x > centreMouseBoundary ? DIRECTION_GO_RIGHT : DIRECTION_GO_LEFT;
	}
	else {
		currentPlaneDirection = DIRECTION_GO_STRAIGHT;
	}

	printf("MOUSE X: %d\tANGLE: %f\n", x, planeYawAngle);
	
}

void myReshape(int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight);

	windowWidth = newWidth;
	windowHeight = newHeight;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)newWidth / (float)newHeight, 0.1, 5000);
	glutWarpPointer(windowWidth / 2, windowHeight / 2);
}

void initSkyTexture() {
	glGenTextures(1, &skyTextureID);

	glBindTexture(GL_TEXTURE_2D, skyTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, SKY_WIDTH, SKY_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, skyTexture);
}

void initAltSkyTexture() {
	glGenTextures(1, &altSkyTextureID);

	glBindTexture(GL_TEXTURE_2D, altSkyTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, ALT_SKY_WIDTH, ALT_SKY_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, altSkyTexture);
}

void initSeaTexture() {
	glGenTextures(1, &seaTextureID);

	glBindTexture(GL_TEXTURE_2D, seaTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, SEA_WIDTH, SEA_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, seaTexture);
}

void initMountainTexture() {
	glGenTextures(1, &mountTextureID);

	glBindTexture(GL_TEXTURE_2D, mountTextureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, MOUNT_WIDTH, MOUNT_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, mountTexture);
}

void initMountain(struct Mountain* mountain, int mountainID) {
	struct Point mountainPoint1 = { 0.0, 0.0,  0.0 };
	struct Point mountainPoint2 = { MOUNTAIN_RESOLUTION, 0.0,  0.0 };
	struct Point mountainPoint3 = { MOUNTAIN_RESOLUTION, 0.0, MOUNTAIN_RESOLUTION };
	struct Point mountainPoint4 = { 0.0, 0.0, MOUNTAIN_RESOLUTION };

	generateMountainGrid(mountainID, mountainPoint1, mountainPoint2, mountainPoint3, mountainPoint4, MOUNTAIN_START_LEVEL);

	setMountainPointColors(mountain->peak, mountain->colors, mountain->points);

	mountain->xOffset = (float)((rand() % 101) - 50.0);
	mountain->zOffset = (float)((rand() % 101) - 50.0);

	mountain->xScale = ((float)(rand() % 10) + 1.0) / 10.0;
	mountain->yScale = mountain-> peak > 30 ? 0.2 : ((float)(rand() % 5) + 1.0) / 10.0;
	mountain->zScale = ((float)(rand() % 10) + 1.0) / 10.0;
}

void initializeGL() {
	glEnable(GL_DEPTH_TEST);          // enable depth testing
	glClearColor(0.0, 0.0, 0.0, 1.0); // set background color (black)

	glMatrixMode(GL_PROJECTION);      // use projection mode (just while we set the camera properties for perspective)
	glLoadIdentity(); // load the identity matrix

	gluPerspective(45, (float)windowWidth / (float)windowHeight, 0.1, 5000);

	glMatrixMode(GL_MODELVIEW); // use model-view mode now that we want to draw things

	glutWarpPointer(windowWidth / 2, windowHeight / 2);

	initSkyTexture();
	initSeaTexture();
	initMountainTexture();
	initAltSkyTexture();
}

void main(int argc, char** argv) {
	srand(time(0));
	loadPlanePoints("cessna.txt", &planePoints[0], planeFaceLists, CESSNA_POINT_COUNT);
	loadPlanePoints("propeller.txt", &propellerPoints[0], propellerFaces, PROPELLER_POINT_COUNT);
	
	printf("LOADING SKY TEXTURE...\n");
	loadImage(SKY_TEXTURE_FILENAME);

	printf("LOADING SEA TEXTURE...\n");
	loadImage(SEA_TEXTURE_FILENAME);

	printf("LOADING MOUNTAIN TEXTURE...\n");
	loadImage(MOUNT_TEXTURE_FILENAME);

	printf("LOADING ALTERNATE SKY TEXTURE...\n");
	loadImage(ALT_SKY_TEXTURE_FILENAME);

	setPropellerOffsets();

	initMountain(&mountain1, 1);
	initMountain(&mountain2, 2);
	initMountain(&mountain3, 3);
	initMountain(&mountain4, 4);

	generateSnowFlakes();
	generateRainDrops();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Maria Jessen's Final Project");

	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySpecialKeyboard);
	glutPassiveMotionFunc(myPassiveMotion);
	glutReshapeFunc(myReshape);

	initializeGL(); // set up OpenGL initial values
	glutMainLoop(); // enter the main drawing loop
}
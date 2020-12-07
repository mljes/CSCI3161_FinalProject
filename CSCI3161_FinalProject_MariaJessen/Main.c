/*

CSCI3161 FINAL PROJECT
MARIA JESSEN B00743170

Flight simulator. Allows user to travel through a scene using the mouse and keyboard. 

*/

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

// Keyboard numbers for special keys
#define KEY_PAGE_UP 104
#define KEY_PAGE_DOWN 105
#define KEY_ARROW_UP 101
#define KEY_ARROW_DOWN 103


#define SPEED_INCREMENT 0.2 // Amount to increase/decrease plane speed by

// Initial window dimensions
int windowWidth = 800;
int windowHeight = 800;

GLenum polygonMode = GL_LINE; // used for toggling between wireframe and filled modes

const GLint gridSize = 500; // length of grid, in squares
const GLfloat gridSectionWidth = 2.0; // length of each square in grid

GLboolean isFullscreen = GL_FALSE;

GLfloat propellerRotationDeg = 0.0; // amount to rotate by when drawing propeller

GLfloat planeToCameraOffset[3] = { 0.0, -1.0, -2.0 }; // used to draw plane a little in front of and below camera

GLfloat planeForwardDelta = SPEED_INCREMENT; // amount that plane will travel by at each frame

GLfloat planeRollDeg = 0.0; // amount of roll to apply to the plane (rotation about the fuselage)

GLboolean simpleSceneMode = GL_TRUE;        // indicates whether to draw simple square scene or the sky/sea cylinder
GLboolean fogMode = GL_TRUE;                // indicates whether to draw fog on the water
GLboolean mountainTexturedMode = GL_FALSE;  // indates whether to draw the mountains with textures or just coloring
GLboolean showMountains = GL_FALSE;         // indicates whether to draw the mountains
GLboolean showSnow = GL_FALSE;              // indicates whether to draw snowflakes and show the alternate sky
GLboolean snowIsAccumulating = GL_FALSE;    // indicates whether to draw fog on mountains and plane to simulate snow accumulation
GLboolean showRain = GL_FALSE;              // indicates whether to draw raindrops and show the alternate sky
GLboolean transitionSkyToClear = GL_FALSE;  // indicates whether we are fading the regular sky back into view

// texture IDs for when we want to bind textures
GLuint skyTextureID; 
GLuint seaTextureID;
GLuint mountTextureID;
GLuint altSkyTextureID;

// Density of fog to simulate snow accumulation
GLfloat snowDensityPlane = 0.0;
GLfloat snowDensityMountains = 0.0;

// Amount of shine on mountain and plane materials
GLint mountainShine = 0;
GLint planeShine = 50;

// Amount to rotate the plane (about the Y axis of the plane itself)
GLfloat planeYawAngle = 0.0;

// Previous mouse position
GLfloat oldMouseX = -1;

// Centre of window in terms of X
GLfloat centreMouseBoundary = 400;

// Density of grey fog (shown during rain and snow)
GLfloat snowFogDensity = 0.0;

/// <summary>
/// Sets the lighting properties of the current material to be drawn
/// </summary>
/// <param name="diffuse"></param>
/// <param name="ambient"></param>
/// <param name="specular"></param>
/// <param name="shine"></param>
void setMaterialProperties(GLfloat diffuse[4], GLfloat ambient[4], GLfloat specular[4], GLfloat shine) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);   // ambient color
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);   // diffuse color
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular); // specular color
	glMaterialf(GL_FRONT, GL_SHININESS, shine);    // shine amount
}

/// <summary>
/// Sets the color in which to draw each mountain vertex based on its height
/// </summary>
/// <param name="peak"></param>
/// <param name="colors"></param>
/// <param name="points"></param>
void setMountainPointColors(GLfloat peak, GLfloat colors[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION][4], struct Point points[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION]) {
	int i, j;
	for (i = 0; i < MOUNTAIN_RESOLUTION; i++) {
		for (j = 0; j < MOUNTAIN_RESOLUTION; j++) {
			GLfloat height = points[i][j].vertex_y;

			// Very top - white
			if (height > (peak - MOUNTAIN_START_LEVEL)) {
				colors[i][j][0] = color_array_white[0];
				colors[i][j][1] = color_array_white[1];
				colors[i][j][2] = color_array_white[2];
			}
			// Just below the top - grey
			else if (height > (peak - 2 * MOUNTAIN_START_LEVEL)) {
				colors[i][j][0] = color_array_grey[0];
				colors[i][j][1] = color_array_grey[1];
				colors[i][j][2] = color_array_grey[2];
			}
			// Rest of mountain - varying (randomly) shades of green
			else {
				// Random values to modify the basic green color
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

/// <summary>
/// Set the color of the current plane part being drawn based on the part number
/// </summary>
/// <param name="partIndex"></param>
void setPartColor(int partIndex) {
	GLfloat ambientColor[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat specularColor[] = { 1.0, 1.0, 1.0, 1.0 };

	if (partIndex <= 3) { // yellow parts
		setMaterialProperties(color_array_yellow, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 5) { // black parts
		setMaterialProperties(color_array_black, ambientColor, specularColor, 50);
	}
	else if (partIndex == 6) { // purple parts
		setMaterialProperties(color_array_light_purple, ambientColor, specularColor, 50);
	}
	else if (partIndex == 7) { // blue parts
		setMaterialProperties(color_array_blue, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 13) { // more yellow parts
		setMaterialProperties(color_array_yellow, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 25) { // more blue parts
		setMaterialProperties(color_array_blue, ambientColor, specularColor, 50);
	}
	else if (partIndex <= 32) { // more yellow parts
		setMaterialProperties(color_array_yellow, ambientColor, specularColor, 50);
	}
}

/// <summary>
/// Load the points, normals and faces from a file
/// </summary>
/// <param name="filename"></param>
/// <param name="points"></param>
/// <param name="faces"></param>
/// <param name="maxVerticesRead"></param>
void loadPlanePoints(char filename[], struct Point* points, struct FaceNode* faces[], int maxVerticesRead) {
	FILE* planeFile;
	errno_t result = fopen_s(&planeFile, filename, "r"); // open file

	if (planeFile == 0) return; // catch file read errors

	int pointCount = 1; // for placing points into array
	int partCount = -1; // for matching points to the plane part they belong to
	char typeChar = 0;  // for checking when we're done reading the file

	int typeCharScanned = fscanf_s(planeFile, "%c ", &typeChar, 2); // get first type character 

	while (typeCharScanned > 0) {
		if (typeChar == 'v') { // loading a vertex
			GLfloat xPos, yPos, zPos;

			fscanf_s(planeFile, "%f %f %f\n", &xPos, &yPos, &zPos); // read coordinates for vertex
			struct Point newPoint = { .vertex_x=xPos, .vertex_y=yPos, .vertex_z=zPos, .normal_x=0.0, .normal_y=0.0, .normal_z=0.0 }; // create a point structure from the coordinates

			points[pointCount] = newPoint; // put point into array

			// keep track of the number of points read so that we can store them
			if (pointCount == maxVerticesRead) {
				pointCount = 1; // reset for when we read the normals so they can be matched with vertexes
			}
			else {
				pointCount++;
			}
		}
		else if (typeChar == 'n'){ // loading a normal
			GLfloat xPos, yPos, zPos;
			fscanf_s(planeFile, "%f %f %f\n", &xPos, &yPos, &zPos); // read coordinates for normal

			// match up normal with point stored in array: place normals in array
			points[pointCount].normal_x = xPos;
			points[pointCount].normal_y = yPos;
			points[pointCount].normal_z = zPos;

			pointCount++;
		}
		else if (typeChar == 'g') { // loading a plane component / part
			char partName[20];
			fgets(&partName, 19, planeFile); // read the partname

			partCount++; // increment place to store current part

			printf("(%d) LOADING %s\n", partCount, partName);

			faces[partCount] = NULL; // get array location for part ready to store faces
		}
		else if (typeChar == 'f') { // loading a face
			int pointIndex = 0;
			int scanned = fscanf_s(planeFile, "%d", &pointIndex); // read first point index for face

			struct PointNode* list = NULL; // set up a list to store points for face

			while (scanned != 0 && scanned != EOF) {
				struct Point point = points[pointIndex]; // get point from array based on point index we read

				if (partCount == 32) { // hub1 (left) - need to get highest and lowest x,y,z to find center for positioning the propellers later
					getLowestCoordinates(point, leftHubLowestCoords);
					getHighestCoordinates(point, leftHubHighestCoords);
				}

				// create a new PointNode from that Point, insert it into the list 
				list = insertPoint(list, point);

				scanned = fscanf_s(planeFile, "%d", &pointIndex); // get next point index
			}

			GLfloat color[4] = { 1.0, 1.0, 1.0, 1.0 };
			faces[partCount] = insertFace(faces[partCount], list, color); // save the list of points and color into the faces list for current component
		}

		typeCharScanned = fscanf_s(planeFile, "%c ", &typeChar, 2); // get next type character
	}
}

/// <summary>
/// Draws vertexes and a normals from a list of Point structures.
/// </summary>
/// <param name="currPoint"></param>
void drawVertexesWithNormals(struct PointNode* currPoint) {
	glBegin(GL_POLYGON);
	while (currPoint != NULL) {
		struct Point point = currPoint->point; // get point from list node

		// draw normal and vertex from point structure
		glNormal3f(point.normal_x, point.normal_y, point.normal_z);
		glVertex3f(point.vertex_x, point.vertex_y, point.vertex_z);

		currPoint = currPoint->next; // get next point node from list
	}
	glEnd();
}

/// <summary>
/// Draw a mountain vertex with a texture mapped to it, or draw it with colours from the coloring function
/// </summary>
/// <param name="points"></param>
/// <param name="colors"></param>
/// <param name="i"></param>
/// <param name="j"></param>
void drawVertexWithTexture(struct Point points[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION], GLfloat colors[MOUNTAIN_RESOLUTION][MOUNTAIN_RESOLUTION][4], int i, int j) {
	if (!mountainTexturedMode) { // not textured mode, so just use colors
		GLfloat diffuseColor[4] = { 0.1, 0.1, 0.1, 1.0 };
		GLfloat specularColor[4] = { 0.0, 0.0, 0.0, 1.0 };

		setMaterialProperties(diffuseColor, colors[i][j], specularColor, 100);
	}

	glNormal3f(3 * points[i][j].vertex_x, 3 * points[i][j].vertex_y, 3 * points[i][j].vertex_z);

	// map texture coordinates to the current mountain x,z coordinates
	if (mountainTexturedMode) glTexCoord2f(points[i][j].vertex_x / MOUNTAIN_RESOLUTION, points[i][j].vertex_z / MOUNTAIN_RESOLUTION);

	glVertex3f(points[i][j].vertex_x, points[i][j].vertex_y, points[i][j].vertex_z);
}

/// <summary>
/// Draw a mountain
/// </summary>
/// <param name="mountain"></param>
void drawMountain(struct Mountain mountain) {
	int i, j;
	glPushMatrix();
	
	// Transform the mountain with random scale factor and translation
	glScalef(mountain.xScale, mountain.yScale, mountain.zScale);
	glTranslatef(mountain.xOffset, -(mountain.peak / 2), mountain.zOffset);

	setMaterialProperties(color_array_white, color_array_white, color_array_white, 0); // set white color under the texture 

	if (mountainTexturedMode) { // want to texture the mountain, so enable texturing and grab correct texture to draw
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mountTextureID);
	}
	
	// draw the mountain points from the array for this mountain
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

	glDisable(GL_TEXTURE_2D); // stop texturing
	glPopMatrix();
}

/// <summary>
/// Draw the plane from the stored vertexes, faces and normals
/// </summary>
void drawCessna() {
	int i = 0;
	
	for (i = 0; i < 33; i++) {
		struct FaceNode* currFace = planeFaceLists[i]; // grab current plane face

		while (currFace != NULL) {
			struct PointNode* currPoint = currFace->pointList; // grab point list from the face node

			setPartColor(i); // set the color of the part based on part number
			drawVertexesWithNormals(currPoint); // draw the vertexes for this face

			currFace = currFace->next; // get the next face
		}
	}
}

/// <summary>
/// Draw a propeller
/// </summary>
void drawPropeller() {
	// colors for different propeller parts
	GLfloat colors[2][4] = {
		COLOR_WHITE,
		COLOR_RED
	};

	int i;
	for (i = 0; i < 2; i++) {
		struct FaceNode* currFace = propellerFaces[i]; // get the current component (list of faces)

		while (currFace != NULL) {
			struct PointNode* currPoint = currFace->pointList; // get the points for the current face

			GLfloat specularColor[] = { 1.0, 1.0, 1.0, 1.0 }; 
			setMaterialProperties(colors[i], colors[i], specularColor, 50); // set the color using the array above
			drawVertexesWithNormals(currPoint); // draw points for current face

			currFace = currFace->next; // get the next face
		}
	}
}

/// <summary>
/// Draws a 3D sphere at the origin with the specified color and radius. Reused from my Assignment 2 code. 
/// </summary>
/// <param name="color"></param>
/// <param name="radius"></param>
void drawSphere(GLfloat radius, GLfloat color[4]) {
	setMaterialProperties(color, color, color, 50);
	gluSphere(gluNewQuadric(), radius, 16, 16); // draw a sphere with specified radius, 16 longitudinal segments and 16 latitudinal segments
}

/// <summary>
/// Draw simple scene's grid around the origin
/// </summary>
void drawGrid() {
	GLint lowerLimit = -(gridSize / 2); // used for far left and back
	GLint upperLimit = -lowerLimit;     // used for far right and front

	int i, j;

	GLfloat color[4] = COLOR_LIGHT_PURPLE;
	color[3] = 0.5; // make the grid a bit translucent
;
	setMaterialProperties(color, color, color_array_white, 50);

	glBegin(GL_QUADS);
	for (i = lowerLimit; i < upperLimit; i++) {
		for (j = lowerLimit; j < upperLimit; j++) {
			// build current square of the grid from current position and width of each square
			GLfloat currentGridPoints[4][3] = {
				{gridSectionWidth * i, 0.0, gridSectionWidth * j},
				{gridSectionWidth * i, 0.0, gridSectionWidth * j + gridSectionWidth},
				{gridSectionWidth * i + gridSectionWidth, 0.0, gridSectionWidth * j + gridSectionWidth},
				{gridSectionWidth * i + gridSectionWidth, 0.0, gridSectionWidth * j}
			};

			// draw points for current square
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

/// <summary>
/// Draw the axis and origin marker
/// </summary>
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

/// <summary>
/// Draw fog of a specified minimum density color
/// </summary>
/// <param name="density"></param>
/// <param name="color"></param>
void drawFog(GLfloat density, GLfloat color[4]) {
	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, color);     // fog color passed in 
	glFogf(GL_FOG_MODE, GL_EXP);      // exponential fog density function
	glFogf(GL_FOG_DENSITY, density);  // minimum density passed in
}

/// <summary>
/// Draw the water disk
/// </summary>
/// <param name="quadricDrawingStyle"></param>
void drawWater(int quadricDrawingStyle) {
	// Set up a pointer for the quadric
	GLUquadricObj* diskPtr;
	diskPtr = gluNewQuadric();

	gluQuadricDrawStyle(diskPtr, quadricDrawingStyle); // line or solid
	gluQuadricNormals(diskPtr, GLU_SMOOTH); // smooth material
	gluQuadricTexture(diskPtr, GL_TRUE);    // texture coordinates calculated for us

	glEnable(GL_TEXTURE_2D); // enable texturing

	setMaterialProperties(color_array_white_water, color_array_white_water, color_array_white_water, 100); // white (or grey) background for texture
	glRotatef(270.0, 1.0, 0.0, 0.0); // rotate disk to sit flat

	if (fogMode) { // draw fog if the mode is on
		if (color_array_white_scene[3] > 0) // regular fog
			drawFog(0.005 * color_array_white_scene[3], color_array_rose_transparent);

		if (snowFogDensity > 0) { // rain/snow grey fog
			drawFog(snowFogDensity, color_array_grey_transparent);
		}
	}

	glBindTexture(GL_TEXTURE_2D, seaTextureID); // get the sea texture
	gluDisk(diskPtr, 0.0, 600, 100, 100); // draw disk
	glDisable(GL_FOG); // turn off fog to draw other things
}

/// <summary>
/// Draw the sky cylinder
/// </summary>
/// <param name="quadricDrawingStyle"></param>
void drawSky(int quadricDrawingStyle) {
	// set up pointer for quadric
	GLUquadricObj* cylinderPtr;
	cylinderPtr = gluNewQuadric();

	gluQuadricDrawStyle(cylinderPtr, quadricDrawingStyle); // line or solid
	gluQuadricNormals(cylinderPtr, GLU_SMOOTH); // smooth quadric style
	gluQuadricTexture(cylinderPtr, GL_TRUE); // texture coordinates calculated for us

	glTranslatef(0.0, -10.0, 0.0); // move cylinder down a bit to prevent gap

	glEnable(GL_TEXTURE_2D); // turn on texturing

	if (showSnow || showRain || transitionSkyToClear) { // draw the alterate sky behind the regular one if we need it
		setMaterialProperties(color_array_white, color_array_white, color_array_white, 100);
		glBindTexture(GL_TEXTURE_2D, altSkyTextureID); // use alternate sky texture
		gluCylinder(cylinderPtr, SCENE_RADIUS + 1, SCENE_RADIUS + 1, SCENE_HEIGHT, 100, 200); // draw cylinder

		glEnable(GL_COLOR_MATERIAL); // use to get easier fading (Source: https://community.khronos.org/t/how-to-fade-a-textured-object-in-out/54181/5)
		glColor4fv(color_array_white_scene);
	}

	if (color_array_white_scene[3] > 0) { // regular sky not faded out to nothing, so draw it
		setMaterialProperties(color_array_white_scene, color_array_white_scene, color_array_white_scene, 100);
		glBindTexture(GL_TEXTURE_2D, skyTextureID); // get sky texture
		gluCylinder(cylinderPtr, SCENE_RADIUS, SCENE_RADIUS, SCENE_HEIGHT, 100, 200); // draw cylinder
	}

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
}

/// <summary>
/// Draw the sky cylinder and water disk
/// </summary>
void drawSceneryCylinder() {
	int quadricDrawingStyle = polygonMode == GL_LINE ? GLU_LINE : GLU_FILL; // set drawing mode for quadrics based on whether we are drawing everything else as wireframes
	
	glPushMatrix();
	drawWater(quadricDrawingStyle);
	drawSky(quadricDrawingStyle);
	glPopMatrix();
}

/// <summary>
/// Draw snow flake arm from a center point and offsets
/// </summary>
/// <param name="center"></param>
/// <param name="endX"></param>
/// <param name="endY"></param>
void drawSnowflakeArm(struct Point center, GLfloat endX, GLfloat endY) {
	glNormal3f(center.vertex_x, center.vertex_y, center.vertex_z);
	glVertex3f(center.vertex_x, center.vertex_y, center.vertex_z);

	glNormal3f(center.vertex_x + endX, center.vertex_y + endY, center.vertex_z);
	glVertex3f(center.vertex_x + endX, center.vertex_y + endY, center.vertex_z);
}

/// <summary>
/// Draw a circle to simulate a ripple from a snowflake or raindrop hitting the water
/// Based on code from my Assignment 2
/// </summary>
/// <param name="xPos"></param>
/// <param name="zPos"></param>
/// <param name="opacity"></param>
/// <param name="radius"></param>
void drawRipples(GLfloat xPos, GLfloat zPos, GLfloat opacity, GLfloat radius) {
	int i;

	glPushMatrix();
	glTranslatef(xPos, 0.01, zPos); // draw the ripple very slighting above the water

	glEnable(GL_COLOR_MATERIAL); // for fading
	glColor4f(color_array_grey[0], color_array_grey[1], color_array_grey[2], opacity); // color will fade

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

/// <summary>
/// Draw snowflakes from values stored in array
/// </summary>
void drawSnowflakes() {
	int i; 
	glPushMatrix();
	

	for (i = 0; i < MAX_SNOWFLAKES; i++) {
		glPushMatrix();
		glBegin(GL_LINES);
		setMaterialProperties(color_array_white, color_array_white, color_array_white, 200);

		// rotate snowflakes to give them a sense of "wobble" as they fall
		glTranslatef(snowflakes[i].center.vertex_x, snowflakes[i].center.vertex_y, snowflakes[i].center.vertex_z);
		glRotatef((rand() % 360) - 180, 1.0, 0.0, 0.0);
		glRotatef((rand() % 360) - 180, 0.0, 1.0, 0.0);
		glRotatef((rand() % 360) - 180, 0.0, 0.0, 1.0);
		glTranslatef(-snowflakes[i].center.vertex_x, -snowflakes[i].center.vertex_y, -snowflakes[i].center.vertex_z);

		// draw arms of snowflake
		drawSnowflakeArm(snowflakes[i].center, 0.2, 0.0);
		drawSnowflakeArm(snowflakes[i].center, -0.2, 0.0);
		drawSnowflakeArm(snowflakes[i].center, 0.0, 0.2);
		drawSnowflakeArm(snowflakes[i].center, 0.0, -0.2);
		drawSnowflakeArm(snowflakes[i].center, 0.12, 0.12);
		drawSnowflakeArm(snowflakes[i].center, 0.12, -0.12);
		drawSnowflakeArm(snowflakes[i].center, -0.12, -0.12);
		drawSnowflakeArm(snowflakes[i].center, -0.12, 0.12);

		glEnd();
		glPopMatrix();

		snowflakes[i].center.vertex_y -= 0.1; // move snowflake down a bit for the next time we draw it

		if (snowflakes[i].center.vertex_y <= 0.0) { // hit the water
			snowIsAccumulating = GL_TRUE;        // enable fog for snow accumulation
			snowflakes[i].showRipple = GL_TRUE;
			snowflakes[i].center.vertex_y = snowflakes[i].initialHeight; // move snowflake back to the top
		}

		if (snowflakes[i].showRipple) { // draw the ripples
			drawRipples(snowflakes[i].center.vertex_x, snowflakes[i].center.vertex_z, snowflakes[i].rippleOpacity, snowflakes[i].rippleRadius);
		}
	}

	glPopMatrix();
}

/// <summary>
/// Draw raindrops from values stored in array
/// </summary>
void drawRaindrops() {
	int i;
	glPushMatrix();
	glLineWidth(2.0); // make raindrop lines thicker
	
	for (i = 0; i < MAX_RAINDROPS; i++) {
		GLfloat length = raindrops[i].length;
		glBegin(GL_LINES);

		// draw bottom of raindrop with 1/2 opacity
		setMaterialProperties(color_raindrop_bottom, color_raindrop_bottom, color_raindrop_bottom, 200);
		glNormal3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y, raindrops[i].bottom.vertex_z);
		glVertex3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y, raindrops[i].bottom.vertex_z);

		// draw top of raindrop with less opacity
		setMaterialProperties(color_raindrop_top, color_raindrop_top, color_raindrop_top, 200);
		glNormal3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y + length, raindrops[i].bottom.vertex_z);
		glVertex3f(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_y + length, raindrops[i].bottom.vertex_z);

		glEnd();

		raindrops[i].bottom.vertex_y -= 0.6; // move raindrop down for next draw

		if (raindrops[i].bottom.vertex_y <= 0.0) { // hit the water
			raindrops[i].showRipple = GL_TRUE; // will draw ripples 
			raindrops[i].bottom.vertex_y = raindrops[i].initialHeight; // move raindrop back to top
		}

		if (raindrops[i].showRipple) { // draw the ripples
			drawRipples(raindrops[i].bottom.vertex_x, raindrops[i].bottom.vertex_z, raindrops[i].rippleOpacity, raindrops[i].rippleRadius);
		}
	}
	glLineWidth(1.0); // reset the line width for the next time
	glPopMatrix();
}

/// <summary>
/// Draw the speedometer, altimeter and weather indicator
/// </summary>
/// <param name="xPos"></param>
/// <param name="yPos"></param>
/// <param name="zPos"></param>
void drawInfoText(GLfloat xPos, GLfloat yPos, GLfloat zPos) {
	setMaterialProperties(color_array_green, color_array_green, color_array_green, 200);
	GLint lineLength = 18;
	GLfloat letterWidth = 0.5;
	GLfloat letterHeight = 0.7;

	char text[5][18] = {
		"------------------",
		"WEATHER:  SUN     ", 
		"SPEED:    0000KM/H",
		"ALTITUDE: 0000KM  ",
		"------------------"
	};

	// set the text values 
	setWeatherText(text[1], showSnow, showRain);
	setNumericalText(text[2], planeForwardDelta, 135);
	setNumericalText(text[3], cameraPosition[1] + planeToCameraOffset[1], 15);
	
	// save original coordinates to offset from later
	GLfloat startPositionX = xPos;
	GLfloat startPositionZ = zPos;

	yPos = yPos + 10; // move up a bit

	int i, j;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 18; j++) {
			// draw next letter at a trigonometric offset from the starting position
			xPos = xPos + (cos(planeYawAngle * M_PI / 180.0) * letterWidth);
			zPos = zPos + (sin(planeYawAngle * M_PI / 180.0) * letterWidth);

			glRasterPos3f(xPos, yPos + 2 + (letterHeight * i), zPos);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i][j]);
		}

		// reset the starting coordinates for the next line
		xPos = startPositionX;
		zPos = startPositionZ;
	}
}

/// <summary>
/// Main drawing function. Draws each frame.
/// </summary>
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the color and depth buffers
	glMatrixMode(GL_MODELVIEW); // enter modelview matrix so we can manipulate drawings
	glLoadIdentity();

	glEnable(GL_BLEND); // want to use tranparency, so we need alpha mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LIGHTING); // turn on lighting
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE); // set all normals to unit normals

	// set light source properties
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1.0, };
	GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1.0, };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1.0, };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// position the light
	GLfloat lightPosition[] = { 0.0, 200.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // wireframe or solid mode

	// set up the camera
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
		cameraFocusPoint[0], cameraFocusPoint[1], cameraFocusPoint[2],
		cameraUpVector[0], cameraUpVector[1], cameraUpVector[2]);

	glPushMatrix();

	if (simpleSceneMode) { // just draw the grid and axis/origin marker
		drawAxes();
		drawGrid();
	}
	else { // draw the advanced scene
		drawSceneryCylinder();
	}

	if (showMountains) { // draw the mountains
		if (showSnow) drawFog(snowDensityMountains, color_array_white); // draw "snow" fog
		drawMountain(mountain1);
		drawMountain(mountain2);
		drawMountain(mountain3);
		drawMountain(mountain4);
		glDisable(GL_FOG);
	}

	if (showSnow) { // draw falling snowflakes
		drawSnowflakes();
	}
	else if (showRain) { // draw falling raindrops
		drawRaindrops();
	}

	glPopMatrix();

	glPushMatrix();
	drawInfoText(cameraFocusPoint[0], cameraFocusPoint[1], cameraFocusPoint[2]); // draw the altimeter/speedometer/weather indicator
	glPopMatrix();

	glPushMatrix();	

	// move to draw the plane just in front of and below the camera
	glTranslatef(cameraPosition[0] + planeToCameraOffset[0], cameraPosition[1] + planeToCameraOffset[1], cameraPosition[2] + planeToCameraOffset[2]);
	
	glRotatef(270.0, 0.0, 1.0, 0.0);
	glRotatef(-planeYawAngle, 0.0, 1.0, 0.0); // rotate the plane around its Y axis for yaw
	glRotatef(planeRollDeg, 1.0, 0.0, 0.0);   // rotate the plane around the fuselage for roll
	glScalef(0.5, 0.5, 0.5); // make the plane smaller

	if (showSnow) drawFog(snowDensityPlane, color_array_white); // draw "snow" fog
	drawCessna();
	glDisable(GL_FOG);

	glPushMatrix();
	// rotate and draw the left propeller
	glTranslatef(-propellerToOrigin[0], -propellerToOrigin[1], -propellerToOrigin[2]);
	glRotatef(propellerRotationDeg, 1.0, 0.0, 0.0);
	glTranslatef(propellerToOrigin[0], propellerToOrigin[1],propellerToOrigin[2]);
	drawPropeller();
	glPopMatrix();

	glPushMatrix();
	// rotate and draw the right propeller
	glTranslatef(-propellerToOrigin[0], -propellerToOrigin[1], propellerToOrigin[2]);
	glRotatef(propellerRotationDeg, 1.0, 0.0, 0.0);
	glTranslatef(propellerToOrigin[0], propellerToOrigin[1], propellerToOrigin[2]);
	drawPropeller();
	glPopMatrix();

	glPopMatrix();

	glutSwapBuffers(); // send drawing information to OpenGL
}

/// <summary>
/// Updates values for each frame so values can be used to modify drawing
/// </summary>
void myIdle() {
	centreMouseBoundary = windowWidth / 2; // get the middle of the screen
	
	GLfloat xOffsetPlane = (centreMouseBoundary - oldMouseX) / windowWidth; // get an xoffset for the angle calculation
	planeYawAngle -= xOffsetPlane / (planeForwardDelta * 2); // determine angle of yaw from mouse movement

	// Trigonometrically determine the new camera position
	// Places camera on an angle on a triangle. The planeForwardDelta is the hypotenuse, the adjacent side is the z offset, opposite is the x offset
	// The camera is at the angle in question (angle in question is the yaw angle)
	GLfloat xOffsetCamera = sin(planeYawAngle * M_PI / 180.0) * planeForwardDelta;
	GLfloat zOffsetCamera = cos(planeYawAngle * M_PI / 180.0) * planeForwardDelta;

	// move the camera by the offsets
	cameraPosition[0] += xOffsetCamera;
	cameraPosition[2] -= zOffsetCamera;

	// use another triangle (same angle) to set the plane in relation to the new camera position. Camera, plane and focus point are all on hypotenuse line
	planeToCameraOffset[0] = sin(planeYawAngle * M_PI / 180.0) * 2.0;
	planeToCameraOffset[2] = cos(planeYawAngle * M_PI / 180.0) * -2.0;

	// plane roll angle determined by plane offset used in yaw determination
	planeRollDeg = abs(xOffsetPlane * 50) < 45.0 ? (xOffsetPlane * 50) : planeRollDeg;

	// use another triangle (same angle) to set the plane in relation to the new camera position. Camera, plane and focus point are all on hypotenuse line
	cameraFocusPoint[0] = cameraPosition[0] + sin(planeYawAngle * M_PI / 180.0) * 50;
	cameraFocusPoint[2] = cameraPosition[2] + cos(planeYawAngle * M_PI / 180.0) * -50;

	propellerRotationDeg += 15.0; // increase rotation of propeller

	if (showSnow || showRain) { // fade out the regular sky if we're in snow/rain mode
		color_array_white_scene[3] = color_array_white_scene[3] <= 0.0 ? 0.0 : (color_array_white_scene[3] - 0.01);
		snowFogDensity = snowFogDensity >= 0.006 ? 0.006 : (snowFogDensity + 0.0001); // fade in the grey fog

		int i;
		for (i = 0; i < 3; i++) { // make the water a litte more grey (darker)
			color_array_white_water[i] = color_array_white_water[i] <= 0.4 ? 0.4 : color_array_white_water[i] - 0.01;
		}
	}

	else if (transitionSkyToClear) { // fade the regular sky back in if we're transitioning back to clear
		if (color_array_white_scene[3] >= 1.0) {
			color_array_white_scene[3] = 1.0;
			transitionSkyToClear = GL_FALSE; // done transitioning since we're back to full opacity
		}
		else { // fade in the regular sky
			color_array_white_scene[3] += 0.01;

			snowFogDensity = snowFogDensity <= 0.0 ? 0.0 : (snowFogDensity - 0.0001); // fade out the grey fog

			int i;
			for (i = 0; i < 3; i++) { // make sky bright blue again
				color_array_white_water[i] = color_array_white_water[i] >= 1.0 ? 1.0 : color_array_white_water[i] + 0.01;
			}
		}
	}

	if (showSnow) {
		if (snowIsAccumulating) { // increase fog density for snow accumulation
			snowDensityPlane = snowDensityPlane >= 1.0 ? 1.0 : snowDensityPlane + 0.01;
			snowDensityMountains = snowDensityMountains >= 1.0 ? 1.0 : snowDensityMountains + 0.0001;
		}

		int i, j;
		for (i = 0; i < MAX_SNOWFLAKES; i++) { // increase radii for ripples 
			if (snowflakes[i].showRipple) { // only update radius for a snowflake's ripple if ripple is being drawn
				updateRippleRadius(&snowflakes[i].rippleRadius, &snowflakes[i].rippleOpacity, &snowflakes[i].showRipple);
			}
		}
	}

	if (showRain) {
		mountainShine += 10.0; //increase shine (wanted to make things look wet, didn't work great)
		planeShine += 10.0;

		int i, j;
		for (i = 0; i < MAX_RAINDROPS; i++) { // increase radii for ripples
			if (raindrops[i].showRipple) { // only update radius for a raindrop's ripple if the ripple is being drawn
				updateRippleRadius(&raindrops[i].rippleRadius, &raindrops[i].rippleOpacity, &raindrops[i].showRipple);
			}
		}
	}

	glutPostRedisplay(); // send update to be drawn
}

/// <summary>
/// Handles keyboard interactions for regular keys
/// </summary>
/// <param name="key"></param>
/// <param name="x"></param>
/// <param name="y"></param>
void myKeyboard(unsigned char key, int x, int y) {
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
	case 'q': // quit the program
		exit(0);
		break;
	case 's': // toggle simple scene mode
		simpleSceneMode = !simpleSceneMode;
		break;
	case 'b': // toggle sea fog visibility
		fogMode = !fogMode;
		break;
	case 'm': // toggle mountain visibility
		showMountains = !showMountains;
		break;
	case 't': // toggle texturing for mountains
		mountainTexturedMode = !mountainTexturedMode;
		break;
	case 'x': // toggle snow mode
		showSnow = !showSnow;
		if (showSnow) {
			showRain = GL_FALSE; // don't want snow and rain at the same time		
		}
		else { // turn off snow
			snowIsAccumulating = GL_FALSE;
			snowDensityMountains = 0.0;
			snowDensityPlane = 0.0;
			transitionSkyToClear = GL_TRUE; // start transitioning back to regular sky
		}
		break;
	case 'r':
		showRain = !showRain;
		if (showRain) {
			showSnow = GL_FALSE; // don't want snow and rain at the same time
		}
		else { // turn off rain
			GLint mountainShine = 0;
			GLint planeShine = 50;
			transitionSkyToClear = GL_TRUE; // start transitioning back to regular sky
		}
	}
}

/// <summary>
/// Handles keyboard interactions for special keys
/// </summary>
/// <param name="key"></param>
/// <param name="x"></param>
/// <param name="y"></param>
void mySpecialKeyboard(int key, int x, int y) {
	const GLfloat verticalDelta = 0.2;

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

/// <summary>
/// Handle mouse movements
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void myPassiveMotion(int x, int y) {
	oldMouseX = x; // save the current x position so we can use it to calculate the angle to turn by in idle function
}

/// <summary>
/// Handle window reshapes
/// </summary>
/// <param name="newWidth"></param>
/// <param name="newHeight"></param>
void myReshape(int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight); // make the new viewport with the new dimensions

	// save the new dimensions
	windowWidth = newWidth;
	windowHeight = newHeight;

	// reset the projection with the new dimensions
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)newWidth / (float)newHeight, 0.1, 5000);
	glutWarpPointer(windowWidth / 2, windowHeight / 2); // move the pointer to the center of the screen so we can start moving straight forward
}

/// <summary>
/// Set up the sky texture
/// </summary>
void initSkyTexture() {
	glGenTextures(1, &skyTextureID); // get a texture ID

	glBindTexture(GL_TEXTURE_2D, skyTextureID); // bind to this ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat mode for wrapping in S coordinate
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repeat mode for wrapping in T coordinate
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, SKY_WIDTH, SKY_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, skyTexture); // make bitmaps
}

/// <summary>
///  Set up the alternate sky texture (for cloudy weather)
/// </summary>
void initAltSkyTexture() {
	glGenTextures(1, &altSkyTextureID); // get a texture ID

	glBindTexture(GL_TEXTURE_2D, altSkyTextureID); // bind to this ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat mode for wrapping in S coordinate
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repeat mode for wrapping in T coordinate
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, ALT_SKY_WIDTH, ALT_SKY_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, altSkyTexture); // make bitmaps
}

/// <summary>
/// Set up sea texture
/// </summary>
void initSeaTexture() {
	glGenTextures(1, &seaTextureID); // get a texture ID

	glBindTexture(GL_TEXTURE_2D, seaTextureID); // bind to this ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat mode for wrapping in S coordinate
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repeat mode for wrapping in T coordinate
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, SEA_WIDTH, SEA_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, seaTexture); // make bitmaps
}

/// <summary>
/// Set up the mountain texture
/// </summary>
void initMountainTexture() {
	glGenTextures(1, &mountTextureID); // get a texture ID

	glBindTexture(GL_TEXTURE_2D, mountTextureID); // bind to this ID 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, MOUNT_WIDTH, MOUNT_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, mountTexture); // make bitmaps
}

/// <summary>
/// Set up mountain values
/// </summary>
/// <param name="mountain"></param>
/// <param name="mountainID"></param>
void initMountain(struct Mountain* mountain, int mountainID) {
	// set inital points for mountain point function
	struct Point mountainPoint1 = { 0.0, 0.0,  0.0 };
	struct Point mountainPoint2 = { MOUNTAIN_RESOLUTION, 0.0,  0.0 };
	struct Point mountainPoint3 = { MOUNTAIN_RESOLUTION, 0.0, MOUNTAIN_RESOLUTION };
	struct Point mountainPoint4 = { 0.0, 0.0, MOUNTAIN_RESOLUTION };

	// recursively generate mountain points
	generateMountainGrid(mountainID, mountainPoint1, mountainPoint2, mountainPoint3, mountainPoint4, MOUNTAIN_START_LEVEL);

	// set mountain point colors
	setMountainPointColors(mountain->peak, mountain->colors, mountain->points);

	// set values for translating mountain
	mountain->xOffset = (float)((rand() % 101) - 50.0);
	mountain->zOffset = (float)((rand() % 101) - 50.0);

	// set values for scaling mountain
	mountain->xScale = ((float)(rand() % 10) + 1.0) / 10.0;
	mountain->yScale = mountain-> peak > 30 ? 0.2 : ((float)(rand() % 5) + 1.0) / 10.0;
	mountain->zScale = ((float)(rand() % 10) + 1.0) / 10.0;
}

/// <summary>
/// Set up OpenGL
/// </summary>
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

/// <summary>
/// Print what the keys do
/// </summary>
void printKeybindings() {
	printf("Scene Controls\n------------------\n");
	printf("f: toggle fullscreen\n");
	printf("g : toggle fog\n");
	printf("m : toggle mountains\n");
	printf("t : toggle mountain texture\n");
	printf("s : toggle sea & sky\n");
	printf("w : toggle wire frame\n");
	printf("x : toggle snow mode\n");
	printf("r : toggle rain mode\n");
	printf("q : quit\n\n");
	printf("Camera Controls\n");
	printf("---------------- -\n");
	printf("Page Up : faster\n");
	printf("Page Down : slower\n");
	printf("Up    Arrow : move up\n");
	printf("Down  Arrow : move down\n");
	printf("Mouse Right : move right\n");
	printf("Mouse Left : move left");
}

void main(int argc, char** argv) {
	srand(time(0));

	// Load the plane and propeller objects
	loadPlanePoints("cessna.txt", &planePoints[0], planeFaceLists, CESSNA_POINT_COUNT);
	loadPlanePoints("propeller.txt", &propellerPoints[0], propellerFaces, PROPELLER_POINT_COUNT);
	
	// Set offsets to place propellers on hubs
	setPropellerOffsets();

	// Load the texture images
	printf("LOADING SKY TEXTURE...\n");
	loadImage(SKY_TEXTURE_FILENAME);

	printf("LOADING SEA TEXTURE...\n");
	loadImage(SEA_TEXTURE_FILENAME);

	printf("LOADING MOUNTAIN TEXTURE...\n");
	loadImage(MOUNT_TEXTURE_FILENAME);

	printf("LOADING ALTERNATE SKY TEXTURE...\n");
	printf("Source: http://www.benkyoustudio.com/Textures/TexturesSnowAndIce/\n");
	loadImage(ALT_SKY_TEXTURE_FILENAME);

	// set up mountain points for 4 mountains
	initMountain(&mountain1, 1);
	initMountain(&mountain2, 2);
	initMountain(&mountain3, 3);
	initMountain(&mountain4, 4);

	// generate snowflakes and raindrops
	generateSnowFlakes();
	generateRainDrops();

	printKeybindings();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // using alpha channel, double buffereing and depth buffering
	glutInitWindowSize(windowWidth, windowHeight); // set window size
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Maria Jessen's Final Project"); // create the window

	// set callback functions
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySpecialKeyboard);
	glutPassiveMotionFunc(myPassiveMotion);
	glutReshapeFunc(myReshape);

	initializeGL(); // set up OpenGL initial values
	glutMainLoop(); // enter the main drawing loop
}
#include <freeglut.h>
#include <stdio.h>
#include "MyDrawingHelpers.h"
#include "LinkedList.h"
#include "MyColors.h"

int windowWidth = 800;
int windowHeight = 800;

GLfloat cameraPosition[3] = { 0.0, 10.0, 40.0 };
GLfloat cameraFocusPoint[3] = { 0.0, 0.0, 0.0 };
GLfloat cameraUpVector[3] = { 0.0, 1.0, 0.0 };

GLenum polygonMode = GL_FILL;

const GLint gridSize = 100;
const GLfloat gridSectionWidth = 2.0;

GLboolean isFullscreen = GL_FALSE;

void setMaterialProperties(GLfloat diffuse[4], GLfloat ambient[4]) {
	GLfloat specularMat[] = { 1.0, 1.0, 1.0, 1.0 }; // neutral specular highlight will reflect color of light source
	GLfloat shine = 100.0; // no a vector, just one val
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMat);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
}

void setPartColor(int partIndex) {
	GLfloat ambientColor[] = { 0.0, 0.0, 0.0, 1.0 };

	if (partIndex <= 3) {
		setMaterialProperties(color_array_yellow, ambientColor);
	}
	else if (partIndex <= 5) {
		setMaterialProperties(color_array_black, ambientColor);
	}
	else if (partIndex == 6) {
		setMaterialProperties(color_array_light_purple, ambientColor);
	}
	else if (partIndex == 7) {
		setMaterialProperties(color_array_blue, ambientColor);
	}
	else if (partIndex <= 13) {
		setMaterialProperties(color_array_yellow, ambientColor);
	}
	else if (partIndex <= 25) {
		setMaterialProperties(color_array_blue, ambientColor);
	}
	else if (partIndex <= 32) {
		setMaterialProperties(color_array_yellow, ambientColor);
	}
}

void loadPlanePoints() {
	FILE* planeFile;
	errno_t result = fopen_s(&planeFile, "cessna.txt", "r");

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

			planePoints[pointCount] = newPoint;

			if (pointCount == CESSNA_POINT_COUNT) {
				pointCount = 1;
			}
			else {
				pointCount++;
			}
		}
		else if (typeChar == 'n'){
			GLfloat xPos, yPos, zPos;
			fscanf_s(planeFile, "%f %f %f\n", &xPos, &yPos, &zPos);

			planePoints[pointCount].normal_x = xPos;
			planePoints[pointCount].normal_y = yPos;
			planePoints[pointCount].normal_z = zPos;

			pointCount++;
		}
		else if (typeChar == 'g') {
			char partName[20];
			fgets(&partName, 19, planeFile);
			printf("LOADING %s\n", partName);

			partCount++;
			faces[partCount] = NULL; //malloc(sizeof(struct FaceNode));
		}
		else if (typeChar == 'f') {
			int pointIndex = 0;
			int scanned = fscanf_s(planeFile, "%d", &pointIndex);

			struct PointNode* list = NULL; //malloc(sizeof(struct PointNode));

			while (scanned != 0 && scanned != EOF) {
				//printf("%d ", pointIndex);
				struct Point point = planePoints[pointIndex];

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
		//printf("NEXT CHAR: %c RESULT: %d LINES READ: %d\n", typeChar, typeCharScanned, linesRead);
	}
}

void drawCessna() {
	int i = 0;
	
	for (i = 0; i < 33; i++) {
		struct FaceNode* currFace = faces[i];

		while (currFace != NULL) {
			struct PointNode* currPoint = currFace->pointList;

			glBegin(GL_POLYGON);
			setPartColor(i);

			while (currPoint != NULL) {
				struct Point point = currPoint->point;

				glNormal3f(point.normal_x, point.normal_y, point.normal_z);
				glVertex3f(point.vertex_x, point.vertex_y, point.vertex_z);
				
				currPoint = currPoint->next;
			}
			glEnd();

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

	setMaterialProperties(color, color);

	gluSphere(gluNewQuadric(), radius, 16, 16); // draw a sphere with specified radius, 16 longitudinal segments and 16 latitudinal segments
}

void drawGrid() {
	GLint lowerLimit = -(gridSize / 2);
	GLint upperLimit = -lowerLimit;

	int i, j;

	//printf("%d, %d\n", lowerLimit, upperLimit);
	GLfloat color[4] = COLOR_LIGHT_PURPLE;
	color[3] = 0.5;

	setMaterialProperties(color, color);

	glBegin(GL_QUADS);
	for (i = lowerLimit; i < upperLimit; i++) {
		for (j = lowerLimit; j < upperLimit; j++) {
			//printf("%d, %d\n", i, j);
			glVertex3f(gridSectionWidth * i, 0.0, gridSectionWidth * j);
			glVertex3f(gridSectionWidth * i, 0.0, gridSectionWidth * j + gridSectionWidth);
			glVertex3f(gridSectionWidth * i + gridSectionWidth, 0.0, gridSectionWidth * j + gridSectionWidth);
			glVertex3f(gridSectionWidth * i + gridSectionWidth, 0.0, gridSectionWidth * j);
		}
	}
	glEnd();

}

void drawAxes() {
	glLineWidth(5.0); // use lines of width 5
	glBegin(GL_LINES);

	// +x axis
	//glColor3f(1.0, 0.0, 0.0);
	setMaterialProperties(color_array_red, color_array_red);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, 0.0);

	// +y axis
	//glColor3f(0.0, 1.0, 0.0);
	setMaterialProperties(color_array_green, color_array_green);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 5.0, 0.0);

	// +z axis
	//glColor3f(0.0, 0.0, 1.0);
	setMaterialProperties(color_array_blue, color_array_blue);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 5.0);

	glEnd();

	glLineWidth(1.0); // reset line width

	GLfloat sphereColor[] = { 1.0, 1.0, 1.0, 1.0 };
	drawSphere(1.0, sphereColor);
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

	GLfloat lightPosition[] = { 0.0, 0.0, 50.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
				cameraFocusPoint[0], cameraFocusPoint[1], cameraFocusPoint[2],
				cameraUpVector[0], cameraUpVector[1], cameraUpVector[2]);

	drawAxes();
	drawGrid();

	glPushMatrix();
	glTranslatef(cameraPosition[0], cameraPosition[1] - 1.0, cameraPosition[2] - 2.0);
	glRotatef(270.0, 0.0, 1.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
	drawCessna();
	glPopMatrix();

	glutSwapBuffers(); // send drawing information to OpenGL
}

void myIdle() {

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
	}
}

void myReshape(int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)newWidth / (float)newHeight, 0.1, 50.0);
}

void initializeGL() {
	glEnable(GL_DEPTH_TEST);          // enable depth testing
	glClearColor(0.0, 0.0, 0.0, 1.0); // set background color (black)

	glMatrixMode(GL_PROJECTION);      // use projection mode (just while we set the camera properties for perspective)
	glLoadIdentity(); // load the identity matrix

	gluPerspective(45, (float)windowWidth / (float)windowHeight, 0.1, 60.0);

	glMatrixMode(GL_MODELVIEW); // use model-view mode now that we want to draw things
}

void main(int argc, char** argv) {
	loadPlanePoints();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Maria Jessen's Final Project");

	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);
	glutKeyboardFunc(myKeyboard);
	glutReshapeFunc(myReshape);

	initializeGL(); // set up OpenGL initial values
	glutMainLoop(); // enter the main drawing loop
}
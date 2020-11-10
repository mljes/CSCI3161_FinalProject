#include <freeglut.h>
int windowWidth = 800;
int windowHeight = 800;

GLfloat cameraPosition[3] = { 10.0, 10.0, 25.0 };
GLfloat cameraFocusPoint[3] = { 0.0, 0.0, 0.0 };
GLfloat cameraUpVector[3] = { 0.0, 1.0, 0.0 };


const GLint gridSize = 100;
/// <summary>
/// Draws a 3D sphere at the origin with the specified color and radius. Reused from my Assignment 2 code. 
/// </summary>
/// <param name="color"></param>
/// <param name="radius"></param>
void drawSphere(GLfloat radius, GLfloat color[4]) {
	glColor4fv(color);
	gluSphere(gluNewQuadric(), radius, 16, 16); // draw a sphere with specified radius, 16 longitudinal segments and 16 latitudinal segments
}

void drawGrid() {

}

void drawAxes() {
	glLineWidth(5.0); // use lines of width 5
	glBegin(GL_LINES);

	// +x axis
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, 0.0);

	// +y axis
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 5.0, 0.0);

	// +z axis
	glColor3f(0.0, 0.0, 1.0);
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

	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
				cameraFocusPoint[0], cameraFocusPoint[1], cameraFocusPoint[2],
				cameraUpVector[0], cameraUpVector[1], cameraUpVector[2]);

	drawAxes();

	glutSwapBuffers(); // send drawing information to OpenGL
}

void myIdle() {
	return;
}

void myKeyboard() {
	return;
}

void initializeGL() {
	glEnable(GL_DEPTH_TEST);          // enable depth testing
	glClearColor(0.0, 0.0, 0.0, 1.0); // set background color (black)

	glMatrixMode(GL_PROJECTION);      // use projection mode (just while we set the camera properties for perspective)
	glLoadIdentity(); // load the identity matrix

	gluPerspective(45, (float)windowWidth / (float)windowHeight, 0.1, 50.0);

	glMatrixMode(GL_MODELVIEW); // use model-view mode now that we want to draw things
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Maria Jessen's Final Project");

	glutDisplayFunc(myDisplay);
	//glutIdleFunc(myIdle);
	//glutKeyboardFunc(myKeyboard);

	initializeGL(); // set up OpenGL initial values
	glutMainLoop(); // enter the main drawing loop
}
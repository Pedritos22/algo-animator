#include <stdio.h>
#include <GL/glut.h>


#define HEIGHT 1080
#define WIDTH 1920


void setup() {

	// Set background dark
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Set point color and size to 1 pixel
	glColor3f(0.0, 1.0, 0.0);
	glPointSize(5.0);

	// Matrix projection and reset with identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the coordinates to be used with the viewport
	gluOrtho2D(0, WIDTH, HEIGHT, 0);
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	/*
	glVertex2f(100.0, 100.0);      // Top left
	glVertex2f(150.0, 100.0);     // Top right
	glVertex2f(150.0, 300.0);	   // Bottom right 
	glVertex2f(100.0, 300.0);    // Bottom left

	glVertex2f(160.0, 100.0);
	glVertex2f(210.0, 100.0);
	glVertex2f(210.0, 300.0);
	glVertex2d(160.0, 300.0);
	*/
	
	float rect_width = 5.0;
	float space = 10.0;

	for (float pos_x = 100.0; pos_x < 500.0; pos_x += 10.0) {
		glVertex2f(pos_x, 100.0);
		glVertex2f(pos_x + rect_width, 100.0);
		glVertex2f(pos_x + rect_width, 300.0);
		glVertex2d(pos_x, 300.0);
	}

	glEnd();
	glFlush();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL Window");
	setup();
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}

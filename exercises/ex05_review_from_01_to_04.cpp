#include <GL/glut.h>

#define _WINDOW_HEIGHT 300
#define _WINDOW_WIDTH 300

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	GLfloat f_w = (GLfloat)width / (GLfloat)_WINDOW_WIDTH;
	GLfloat f_h = (GLfloat)height / (GLfloat)_WINDOW_HEIGHT;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0*f_w, 1.0*f_w, -1.0*f_h, 1.0*f_h, -1.0, 1.0);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	glutWireSphere(0.3, 15, 15);

	glFlush(); 
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutCreateWindow("title");
	glutInitWindowSize(_WINDOW_WIDTH, _WINDOW_HEIGHT);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}
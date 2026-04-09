#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>	

static int Day = 0, Time = 0;
void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                                   // CTM = I 
    glColor3f(1.0, 0.3, 0.3);                           // 색 1번 설정
    glutWireSphere(0.2, 20, 16);  						// 구 그리기 (1st)
    glPushMatrix();
    glRotatef((GLfloat)Day, 0.0, 1.0, 0.0);		        // CTM = I * Ry(Day)	
    glTranslatef(0.7, 0.0, 0.0);					        // CTM = I * Ry(Day) * T(0.7, 0.0, 0.0)
    glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);		        // CTM = I * Ry(Day) * T(0.7, 0.0, 0.0) * Ry(Time)	
    glColor3f(0.5, 0.6, 0.7);                            // 색 2번 설정
    glutWireSphere(0.1, 10, 8);  					    // 구 그리기 (2nd) -> 색 2번, CTM = I * Ry(Day) * T(0.7, 0.0, 0.0) * Ry(Time)
    glPushMatrix();
    glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);	        // CTM = I * Ry(Day) * T(0.7, 0.0, 0.0) * Ry(Time) * Ry(Time)
    glTranslatef(0.2, 0.0, 0.0);				        // CTM = I * Ry(Day) * T(0.7, 0.0, 0.0) * Ry(Time) * Ry(Time) * T(0.2, 0.0, 0.0)
    glColor3f(0.9, 0.8, 0.2);                   // 색 3번 설정
    glutWireSphere(0.04, 10, 8);  				// 구 그리기 (3rd) -> 색 3번, CTM = I * Ry(Day) * T(0.7, 0.0, 0.0) * Ry(Time) * Ry(Time) * T(0.2, 0.0, 0.0) 
    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();                                   // = glFlush()
}

void MyKeyboard(unsigned char key, int x, int y) {  // key: 눌린 키, x: 마우스 커서의 x좌표, y: 마우스 커서의 y좌표
    switch (key) {                                  // 아스키 코드값이 key에 저장되어 있음 
    case 'd':
        Day = (Day + 10) % 360;
        glutPostRedisplay();                       // 디스플레이 이벤트 발생시키기 
        break;
    case 't':
        Time = (Time + 5) % 360;
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glutDisplayFunc(MyDisplay);                      // Callback 함수
    glutKeyboardFunc(MyKeyboard);
    glutMainLoop();
    return 0;
}

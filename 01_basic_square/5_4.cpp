#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>	

void MyDisplay() {					
    glClear(GL_COLOR_BUFFER_BIT);   // 실제로 화면을 지움 
    glViewport(0, 0, 300, 300);

//  glColor3f(1.0, 1.0, 1.0);   // 흰색
    glColor3f(0.0, 1.0, 0.0);   // 녹색

    glBegin(GL_POLYGON);       	 	
        glVertex3f(-0.5, -0.5, 0.0);    // 꼭지점 지정 
        glVertex3f(0.5, -0.5, 0.0);
        glVertex3f(0.5, 0.5, 0.0);
        glVertex3f(-0.5, 0.5, 0.0);
    glEnd();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); 			//GLUT 윈도우 함수 
    glutInitDisplayMode(GLUT_RGB);  
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");

    glClearColor(0.0, 0.0, 0.0, 1.0);	// 화면을 지울 색을 지정 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glutDisplayFunc(MyDisplay);  	  	//GLUT 콜백함수 등록
    glutMainLoop();					//이벤트 루프 진입 
    return 0;
}

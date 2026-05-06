#include <GL/glut.h>

void mydisplay() {
	glClear(GL_COLOR_BUFFER_BIT);		// 화면 clear (glClear ~ glFlush)
	glBegin(GL_POLYGON);				// 그래픽 Object를 선언	(glBegin ~ glEnd)
		glVertex3f(-0.5, -0.5, 0.0);		// 점 지정 (z는 0평면 위에 존재)
		glVertex3f(0.5, -0.5, 0.0);			// 점 지정
		glVertex3f(0.5, 0.5, 0.0);			// 점 지정
		glVertex3f(-0.5, 0.5, 0.0);		// 해당 줄을 지우는 경우 삼각형이 됨
	glEnd();							// 그래픽 Object 내용 마무리
	glFlush();							// 버퍼에 있는 내용을 그리도록 함 
}

int main(int arge, char* argv[]) {		
	glutInit(&arge, argv);				// GLUT 라이브러리 초기화 
	glutCreateWindow("Test");			// 윈도우 생성
	glutDisplayFunc(mydisplay);			// display함수 지정
	glutMainLoop();						// 메인 루프 돌입
	return 0;							// 메인루프가 끝날 때까지 실행되지 않음 
}
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define _WINDOW_WIDTH 300
#define _WINDOW_HEIGHT 300

int mode = 0;

void Mydisplay() {
	glClear(GL_COLOR_BUFFER_BIT);		// 컬러버퍼에 초기화 색을 가함
	glColor3f(0.5, 0.5, 0.5);			// 색상설정: 회색 (0.5: Red, 0.5: Green, 0.5: Blue)
	if (mode == 0) {
		glBegin(GL_POLYGON);				// 사각형 시작 
		glVertex3f(-0.5, -0.5, 0.0);		// 좌하단 좌표
		glVertex3f(0.5, -0.5, 0.0);			// 우하단 좌표
		glVertex3f(0.5, 0.5, 0.0);			// 우상단 좌표
		glVertex3f(-0.5, 0.5, 0.0);			// 좌상단 좌표
		glEnd();							// 사각형 끝
	}
	else if (mode == 1) {
		glutWireTeapot(0.6);				// 크기가 0.6인 와이어프레임 티팟 그리기
	}
	glFlush();							// 버퍼에 있는 내용을 그리도록 함 
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	case 'a':
		mode = (mode + 1) % 2;
		break;
	case 'Q':
		exit(0); break;
	case 'q':
		exit(0); break;
	case 27:							// ESC 키의 ASCII 코드
		exit(0); break; 
	}
	glutPostRedisplay();
}

void MyReshape(int NewWidth, int NewHeight) {
	glViewport(0, 0, NewWidth, NewHeight);									// 뷰포트 설정: (0, 0)에서 시작하여 NewWidth, NewHeight 크기로 설정
	GLfloat WidthFactor = (GLfloat)NewWidth / (GLfloat)_WINDOW_WIDTH;		// 너비 비율 계산
	GLfloat HeightFactor = (GLfloat)NewHeight / (GLfloat)_WINDOW_HEIGHT;	// 높이 비율 계산
	glMatrixMode(GL_PROJECTION);											// 투영 행렬 모드로 설정
	glLoadIdentity();														// 행렬 초기화
	glOrtho(-1.0 * WidthFactor, 1.0 * WidthFactor,							// 직교 투영 설정: 왼쪽, 오른쪽, 아래, 위, 근평면, 원평면
		-1.0 * HeightFactor, 1.0 * HeightFactor, -1.0, 1.0);
}

int main(int arge, char** argv) {		
	glutInit(&arge, argv);								// GLUT 라이브러리 초기화 
	glutInitDisplayMode(GLUT_RGB);						// 디스플레이 모드 설정 (RGB)
	glutInitWindowSize(_WINDOW_WIDTH, _WINDOW_HEIGHT);	// 윈도우 크기 설정
	glutInitWindowPosition(0, 0);						// 윈도우 위치 설정
	glutCreateWindow("OpenGL Example Drawing");			// 윈도우 생성
	glClearColor(1.0, 1.0, 1.0, 1.0);					// 초기화 색은 백색

	// 콜백 함수 등록
	glutDisplayFunc(Mydisplay);							// display함수 지정
	glutKeyboardFunc(MyKeyboard);						// keyboard함수 지정
	glutReshapeFunc(MyReshape);							// reshape함수 지정

	glutMainLoop();										// 메인 루프 돌입
	return 0;											// 메인루프가 끝날 때까지 실행되지 않음 
}
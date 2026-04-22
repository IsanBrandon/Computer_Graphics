#include <GL/glut.h>   // GLUT 라이브러리: 윈도우 생성, 이벤트 처리, 기본 도형(주전자 등) 제공
#include <GL/gl.h>     // OpenGL 핵심 함수 선언
#include <GL/glu.h>    // GLU 유틸리티 함수 선언 (gluLookAt, gluPerspective 등)

int Width, Height;     // 현재 윈도우의 가로/세로 크기를 저장할 전역 변수

// OpenGL의 초기 상태를 설정하는 함수
void Init() {
	glClearColor(1.0, 1.0, 1.0, 1.0);        // 화면을 지울 때 사용할 배경색을 흰색(RGBA=1,1,1,1)으로 설정

	glMatrixMode(GL_PROJECTION);             // 현재 다룰 행렬을 투영행렬(PROJECTION)로 선택
	glLoadIdentity();                        // 투영행렬을 단위행렬로 초기화
	glOrtho(-2.0, 2.0, -2.0, 2.0, 0.5, 5.0); // 직교 투영(orthographic projection) 설정
	// x: -2~2, y: -2~2 범위를 화면에 보이게 함
	// z: 0.5~5.0 범위의 물체만 보이도록 깊이 범위 설정

	glMatrixMode(GL_MODELVIEW);              // 이제 모델뷰 행렬을 다루도록 전환
	glLoadIdentity();                        // 모델뷰 행렬도 단위행렬로 초기화
}

// 실제 장면(바닥 + 주전자)을 그리는 함수
void DrawScene() {
	glColor3f(0.7, 0.7, 0.7);                // 바닥 색상을 회색으로 설정

	glPushMatrix();                          // 현재 모델뷰 행렬 상태를 스택에 저장
	glTranslatef(0.0, -1.0, 0.0);            // 바닥을 y축 아래로 1만큼 이동

	glBegin(GL_QUADS);                       // 사각형(quad) 하나를 그리기 시작
	glVertex3f(2.0, 0.0, 2.0);           // 바닥 사각형의 첫 번째 꼭짓점
	glVertex3f(2.0, 0.0, -2.0);          // 두 번째 꼭짓점
	glVertex3f(-2.0, 0.0, -2.0);         // 세 번째 꼭짓점
	glVertex3f(-2.0, 0.0, 2.0);          // 네 번째 꼭짓점
	glEnd();                                 // 사각형 그리기 종료

	glPopMatrix();                           // 바닥 이동 변환을 적용하기 전 상태로 행렬 복원

	glColor3f(0.3, 0.3, 0.7);                // 주전자 색상을 파란빛이 도는 색으로 설정

	glPushMatrix();                          // 주전자에 적용할 변환을 위해 현재 행렬 저장
	glTranslatef(0.0, 0.0, -0.5);        // 주전자를 z축 음의 방향으로 약간 이동
	glutWireTeapot(1.0);                 // 크기 1.0의 와이어프레임 주전자 그리기
	glPopMatrix();                           // 주전자에 적용한 변환 제거
}

// 화면에 실제로 그림을 그리는 디스플레이 콜백 함수
void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);            // 컬러 버퍼를 지워 이전 프레임 내용 제거
	glColor3f(1.0, 1.0, 1.0);                // 현재 색상을 흰색으로 설정
	// (이 코드에서는 이후 DrawScene에서 다시 색을 설정하므로 큰 영향은 없음)

// --------------------------------------------------
// 1) 왼쪽 아래 뷰포트: 정면도(front view)
// --------------------------------------------------
	glViewport(0, 0, Width / 2, Height / 2); // 윈도우의 왼쪽 아래 1/4 영역을 뷰포트로 설정

	glMatrixMode(GL_MODELVIEW);              // 모델뷰 행렬을 다루겠다고 명시
	glPushMatrix();                          // 현재 모델뷰 행렬 저장
	glLoadIdentity();                        // [수정] 카메라 변환 전에 행렬을 초기화
	// 왜 수정했나?
	// 이전에 남아 있던 변환이 섞이면 시점이 꼬일 수 있으므로,
	// 각 뷰마다 깨끗한 상태(identity)에서 시작하도록 함
	gluLookAt(0.0, 0.0, 1.0,                 // 카메라 위치: z축 +1 지점
		0.0, 0.0, 0.0,                 // 바라보는 대상: 원점
		0.0, 1.0, 0.0);                // 위쪽 방향: y축 +방향
	DrawScene();                             // 해당 카메라 시점으로 장면 그리기
	glPopMatrix();                           // 행렬 복원

	// --------------------------------------------------
	// 2) 오른쪽 아래 뷰포트: 측면도(side view)
	// --------------------------------------------------
	glViewport(Width / 2, 0, Width / 2, Height / 2); // 윈도우의 오른쪽 아래 1/4 영역

	glMatrixMode(GL_MODELVIEW);              // 모델뷰 행렬 사용
	glPushMatrix();                          // 현재 모델뷰 행렬 저장
	glLoadIdentity();                        // [수정] 카메라 적용 전 행렬 초기화
	// 왜 수정했나?
	// 첫 번째 뷰의 변환 결과가 두 번째 뷰에 누적되지 않도록 하기 위함
	gluLookAt(1.0, 0.0, 0.0,                 // 카메라 위치: x축 +1 지점
		0.0, 0.0, 0.0,                 // 바라보는 대상: 원점
		0.0, 1.0, 0.0);                // 위쪽 방향: y축 +방향
	DrawScene();                             // 측면 시점으로 장면 그리기
	glPopMatrix();                           // 행렬 복원

	// --------------------------------------------------
	// 3) 왼쪽 위 뷰포트: 평면도(top view)
	// --------------------------------------------------
	glViewport(0, Height / 2, Width / 2, Height / 2); // 윈도우의 왼쪽 위 1/4 영역

	glMatrixMode(GL_MODELVIEW);              // 모델뷰 행렬 사용
	glPushMatrix();                          // 현재 모델뷰 행렬 저장
	glLoadIdentity();                        // [수정] 카메라 적용 전 행렬 초기화
	// 왜 수정했나?
	// top view도 독립적인 카메라 상태에서 그려야 하기 때문
	gluLookAt(0.0, 1.0, 0.0,                 // 카메라 위치: y축 +1 지점(위에서 내려다봄)
		0.0, 0.0, 0.0,                 // 바라보는 대상: 원점
		0.0, 0.0, -1.0);               // 위쪽 방향을 -z축으로 지정
	// 위에서 보는 경우 y축이 시선 방향과 겹치므로,
	// 보조 up 벡터를 z축 방향으로 따로 지정해야 함
	DrawScene();                             // 평면도 시점으로 장면 그리기
	glPopMatrix();                           // 행렬 복원

	// --------------------------------------------------
	// 4) 오른쪽 위 뷰포트: 원근 투영(perspective) 3D view
	// --------------------------------------------------
	glViewport(Width / 2, Height / 2, Width / 2, Height / 2); // 윈도우의 오른쪽 위 1/4 영역

	glMatrixMode(GL_PROJECTION);             // 투영행렬을 조작하기 위해 PROJECTION 모드로 전환
	glPushMatrix();                          // 기존 투영행렬 저장
	glLoadIdentity();                        // 투영행렬 초기화
	gluPerspective(30.0, 1.0, 3.0, 50.0);   // 원근 투영 설정
	// 시야각 30도, 종횡비 1.0, near=3.0, far=50.0

	glMatrixMode(GL_MODELVIEW);              // 모델뷰 행렬 모드로 전환
	glPushMatrix();                          // 현재 모델뷰 행렬 저장
	glLoadIdentity();                        // [수정] 원근 시점에서도 모델뷰 행렬 초기화
	// 왜 수정했나?
	// perspective 뷰 역시 이전 모델 변환/카메라 변환의 영향을 받지 않게 하기 위해
	gluLookAt(5.0, 5.0, 5.0,                 // 카메라를 대각선 위쪽 위치에 둠
		0.0, 0.0, 0.0,                 // 원점을 바라보게 설정
		0.0, 1.0, 0.0);                // 위쪽 방향은 y축 +방향
	DrawScene();                             // 원근감 있는 3D 시점으로 장면 그리기
	glPopMatrix();                           // 모델뷰 행렬 복원

	glMatrixMode(GL_PROJECTION);             // 다시 PROJECTION 모드로 전환
	glPopMatrix();                           // 원래의 투영행렬 복원

	glMatrixMode(GL_MODELVIEW);              // [수정] 마지막에 다시 MODELVIEW 모드로 복귀
	// 왜 수정했나?
	// 이 코드는 다음 프레임에서도 모델뷰 행렬에 대해
	// gluLookAt, glPushMatrix 등을 사용할 것이므로
	// 현재 행렬 모드를 GL_MODELVIEW로 되돌려 두는 것이 안전함
	// 그렇지 않으면 다음 호출에서 카메라 변환이
	// 투영행렬에 적용되어 화면이 비정상적으로 보일 수 있음

	glFlush();                               // 지금까지의 OpenGL 명령을 강제로 실행
	// 단일 버퍼(GLUT_SINGLE) 사용 시 화면 출력 마무리에 사용
}

// 윈도우 크기가 변경될 때 호출되는 리셰이프 콜백 함수
void MyReshape(int w, int h) {
	if (h == 0) h = 1;                      // [보강] 높이가 0이 되는 극단적 상황 방지
	// 왜 넣었나?
	// 일부 환경에서 창 크기 조절 중 h가 0이 되면
	// 이후 뷰포트/비율 계산에서 문제가 생길 수 있기 때문

	Width = w;                              // 변경된 윈도우 가로 크기 저장
	Height = h;                             // 변경된 윈도우 세로 크기 저장
}

// 프로그램의 시작점(main 함수)
int main(int argc, char** argv) {
	Width = 500; Height = 500;              // 초기 윈도우 크기를 500x500으로 설정

	glutInit(&argc, argv);                  // [수정] GLUT 시스템 초기화
	// 왜 수정했나?
	// GLUT를 사용하기 전에 반드시 초기화해야 함
	// 이 호출 없이 glutInitDisplayMode, glutCreateWindow 등을 사용하면
	// 환경에 따라 실행 오류 또는 비정상 동작이 발생할 수 있음

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); // 단일 버퍼 + RGBA 컬러 모드 사용
	glutInitWindowSize(Width, Height);      // 생성할 윈도우의 초기 크기 설정
	glutInitWindowPosition(0, 0);           // 화면에서 윈도우가 처음 나타날 위치 설정
	glutCreateWindow("OpenGL Sample Drawing"); // 제목이 있는 윈도우 생성

	Init();                                 // OpenGL 초기 설정 수행

	glutDisplayFunc(MyDisplay);             // 화면을 그릴 때 호출할 디스플레이 함수 등록
	glutReshapeFunc(MyReshape);             // 창 크기가 바뀔 때 호출할 함수 등록

	glutMainLoop();                         // GLUT 이벤트 루프 시작 (프로그램이 계속 실행됨)
	return 0;                               // 일반적으로 여기에 도달하지 않지만, 형식상 작성
}
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

// 하나의 길쭉한 큐브 막대를 그리는 함수
void DrawBar(float tx, float ty, float tz,
	float rx, float ry, float rz, float angle,
	float sx, float sy, float sz)
{
	glPushMatrix(); // 현재 행렬 상태 저장

	// 위치 이동
	glTranslatef(tx, ty, tz); // 큐브를 (tx, ty, tz) 위치로 이동

	// 회전
	glRotatef(angle, rx, ry, rz); // angle만큼 (rx, ry, rz) 축을 중심으로 회전

	// 크기 조절
	glScalef(sx, sy, sz); // 큐브를 길쭉하게 만들기 위해 x축으로는 1, y축과 z축으로는 0.2로 스케일링

	// 기본 도형 출력
	glutSolidCube(1.0f); // 크기가 1인 큐브를 그립니다.

	glPopMatrix();
}

// '박'을 단순화해서 그리는 함수
void DrawPark()
{
	// =========================
	// [1] ㅂ 부분
	// 전체적으로 왼쪽에 배치
	DrawBar(-1.6f, 0.0f, 0.0f, 0, 0, 1, 0, 0.25f, 2.4f, 0.25f); // 왼쪽 세로
	DrawBar(-0.7f, 0.0f, 0.0f, 0, 0, 1, 0, 0.25f, 2.4f, 0.25f); // 오른쪽 세로

	DrawBar(-1.15f, 1.05f, 0.0f, 0, 0, 1, 0, 1.15f, 0.25f, 0.25f); // 위 가로
	DrawBar(-1.15f, 0.0f, 0.0f, 0, 0, 1, 0, 1.15f, 0.22f, 0.22f); // 가운데 가로
	DrawBar(-1.15f, -1.05f, 0.0f, 0, 0, 1, 0, 1.15f, 0.25f, 0.25f); // 아래 가로

	// =========================
	// [2] ㅏ 부분
	// ㅂ 바로 오른쪽에 붙여 배치
	DrawBar(0.25f, 0.0f, 0.0f, 0, 0, 1, 0, 0.25f, 2.4f, 0.25f); // 세로획
	DrawBar(0.70f, 0.25f, 0.0f, 0, 0, 1, 0, 0.90f, 0.22f, 0.22f); // 짧은 가로획

	// =========================
	// [3] 받침 ㄱ 부분
	// 글자 아래쪽에 붙여 배치
	DrawBar(0.10f, -1.95f, 0.0f, 0, 0, 1, 0, 2.30f, 0.25f, 0.25f); // 아래 가로
	DrawBar(1.05f, -1.35f, 0.0f, 0, 0, 1, 0, 0.25f, 1.20f, 0.25f); // 오른쪽 세로

	// =========================
	// [4] 장식용 torus (선택)
	glPushMatrix();
	glTranslatef(-1.15f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutWireTorus(0.04, 0.18, 12, 20);
	glPopMatrix();
}

void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 화면과 깊이 버퍼 초기화
	glLoadIdentity(); // 모델뷰 행렬 초기화

	// 카메라 설정
	gluLookAt(0.0, 0.0, 10.0, // 카메라 위치
		0.0, 0.0, 0.0, // 바라보는 점
		0.0, 1.0, 0.0); // 업 벡터
	
	// 전체 글자를 보기 좋게 약간 회전
	glRotatef(15.0f, 1.0f, 0.0f, 0.0f); // x축을 중심으로 15도 회전
	glRotatef(-15.0f, 0.0f, 1.0f, 0.0f); // y축을 중심으로 -15도 회전

	// 색상
	glColor3f(0.2f, 0.9f, 0.3f); // 밝은 녹색

	DrawPark(); // '박' 글자 그리기

	glutSwapBuffers(); // 더블 버퍼링을 사용하여 화면 업데이트
}

void MyReshape(int w, int h)
{
	if (h == 0) h = 1; 

	glViewport(0, 0, w, h); // 뷰포트 설정

	glMatrixMode(GL_PROJECTION); // 투영 행렬 모드로 전환
	glLoadIdentity(); // 
	gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

void MyInit()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 700);
	glutCreateWindow("3D Korean Letter: Park");

	MyInit();

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);

	glutMainLoop();
	return 0;
}

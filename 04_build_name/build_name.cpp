#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

// 애니메이션 제어 변수
static bool g_animRunning = false;
static int g_lastTime = 0; // milliseconds
static float g_totalTime = 0.0f; // seconds
static float g_rotate = 0.0f; // 전체 회전 각도
static float g_bob = 0.0f; 


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
	glutSolidCube(1.0f); // 크기가 1인 큐브

	glPopMatrix();
}

// '박' 글자 그리기
void DrawPark()
{
	// ㅂ: 왼쪽 세로
	glColor3f(0.9f, 0.1f, 0.1f);
	DrawBar(-1.6f, 0.0f, 0.0f, 0, 0, 1, 0, 0.25f, 2.4f, 0.25f);

	// ㅂ: 오른쪽 세로
	glColor3f(0.1f, 0.1f, 0.9f);
	DrawBar(-0.7f, 0.0f, 0.0f, 0, 0, 1, 0, 0.25f, 2.4f, 0.25f);

	// ㅂ: 가운데 가로
	glColor3f(0.2f, 0.9f, 0.3f);
	DrawBar(-1.15f, 0.0f, 0.0f, 0, 0, 1, 0, 1.15f, 0.22f, 0.22f);

	// ㅂ: 아래 가로
	glColor3f(0.8f, 0.1f, 0.6f);
	DrawBar(-1.15f, -1.05f, 0.0f, 0, 0, 1, 0, 1.15f, 0.25f, 0.25f);

	// ㅏ: 세로획
	glColor3f(0.0f, 0.8f, 0.9f);
	DrawBar(0.25f, 0.0f, 0.0f, 0, 0, 1, 0, 0.25f, 2.4f, 0.25f);

	// ㅏ: 짧은 가로획
	glColor3f(1.0f, 0.3f, 0.3f);
	DrawBar(0.70f, 0.25f, 0.0f, 0, 0, 1, 0, 0.90f, 0.22f, 0.22f);

    // 받침 ㄱ: 아래 가로
	const float ghx = 0.10f;
	const float ghy = -1.95f;
	const float ghl = 2.30f;
	const float ght = 0.25f; 
	glColor3f(0.7f, 0.4f, 0.1f);
	DrawBar(ghx, ghy, 0.0f, 0, 0, 1, 0, ghl, ght, ght);

	// 받침 ㄱ: 오른쪽 세로 
	const float vw = 0.25f; 
	const float vh = 1.20f; 
	float rightEdge = ghx + ghl * 0.5f; 
	float vertCenterX = rightEdge - vw * 0.5f; 
	float vertCenterY = ghy - ght * 0.5f - vh * 0.5f; 
	glColor3f(0.2f, 0.6f, 0.2f);
	DrawBar(vertCenterX, vertCenterY, 0.0f, 0, 0, 1, 0, vw, vh, vw);
}

void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glLoadIdentity(); 
	// 카메라 설정
	// 애니메이션 중이면 카메라가 천천히 원형 궤도를 따라 회전하도록 함(카메라 애니메이션)
	float camX = 0.0f;
	float camY = 0.0f;
	float camZ = 10.0f;
	if (g_animRunning)
	{
		// 느리게 회전: 한 바퀴 약 31초(속도 0.2 rad/s)
		float angle = g_totalTime * 0.2f;
		camX = 10.0f * sinf(angle);
		camZ = 10.0f * cosf(angle);
		camY = 2.0f * sinf(angle * 0.5f);
	}
	else
	{
		camX = 0.0f; camY = 0.0f; camZ = 10.0f;
	}

	gluLookAt((double)camX, (double)camY + 1.5, (double)camZ, // 카메라 위치
		0.0, 0.0, 0.0, // 바라보는 점
		0.0, 1.0, 0.0); // 업 벡터

	DrawPark();

	glutSwapBuffers(); // 더블 버퍼링을 사용하여 화면 업데이트
}

void MyReshape(int w, int h)
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h); // 뷰포트 설정

	glMatrixMode(GL_PROJECTION); // 투영 행렬 모드로 전환
	glLoadIdentity(); 
	gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

void MyInit()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST); 
}

// 애니메이션 업데이트 (타이머 기반)
void UpdateAnimation(int value)
{
	int timeNow = glutGet(GLUT_ELAPSED_TIME);
	if (g_lastTime == 0) g_lastTime = timeNow;
	int delta = timeNow - g_lastTime;
	g_lastTime = timeNow;

	if (g_animRunning)
	{
		g_totalTime += delta / 1000.0f;
		// 전체 회전 속도
		g_rotate = fmodf(g_totalTime * 10.0f, 360.0f);
		// bobbing (위아래 이동)
		g_bob = sinf(g_totalTime * 2.0f);
	}

	glutPostRedisplay();
	// 16ms ~ 60fps. 계속 타이머 등록하여 20초 이상 애니메이션 가능
	glutTimerFunc(16, UpdateAnimation, 0);
}

// 키보드 입력: s - 시작/정지, r - 초기화
void MyKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
	case 'S':
		g_animRunning = !g_animRunning;
		if (g_animRunning && g_lastTime == 0)
			g_lastTime = glutGet(GLUT_ELAPSED_TIME);
		break;
	case 'r':
	case 'R':
		// 초기 상태로 리셋
		g_animRunning = false;
		g_lastTime = 0;
		g_totalTime = 0.0f;
		g_rotate = 0.0f;
		g_bob = 0.0f;
		glutPostRedisplay();
		break;
	default:
		break;
	}
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
	glutKeyboardFunc(MyKeyboard);
	// 시작 시 타이머 등록
	glutTimerFunc(16, UpdateAnimation, 0);

	glutMainLoop();
	return 0;
}

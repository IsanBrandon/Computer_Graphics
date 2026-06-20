#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

// --------------------------------------------------
// 초기 설정 함수
// --------------------------------------------------
void Init()
{
    // 배경색을 흰색으로 설정
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // z-buffer를 이용한 은선/은면 제거 활성화
    glEnable(GL_DEPTH_TEST);
}

// --------------------------------------------------
// 삼각형 도형 그리기
// --------------------------------------------------
void DrawTriangles()
{
    // 삼각형 색상: 주황색
    glColor3f(1.0f, 0.2f, 0.0f);

    glBegin(GL_TRIANGLES);

    // 왼쪽 삼각형
    // 큐브 뒤쪽에 배치하여 일부가 큐브에 의해 가려지도록 함
    glVertex3f(-1.5f, -0.6f, -1.0f);
    glVertex3f(-0.3f, -0.6f, -1.0f);
    glVertex3f(-0.9f, 0.5f, -1.0f);

    // 오른쪽 삼각형
    // 큐브 오른쪽 뒤에 배치
    glVertex3f(0.3f, -0.6f, -1.0f);
    glVertex3f(1.5f, -0.6f, -1.0f);
    glVertex3f(0.9f, 0.5f, -1.0f);

    // 위쪽 삼각형
    // 큐브 위쪽 뒤에 배치하여 아래쪽 일부가 큐브에 가려지도록 함
    glVertex3f(-0.4f, 0.5f, -1.0f);
    glVertex3f(0.4f, 0.5f, -1.0f);
    glVertex3f(0.0f, 1.4f, -1.0f);

    glEnd();
}

// --------------------------------------------------
// 육면체 그리기
// --------------------------------------------------
void DrawCube()
{
    float s = 0.75f; // 큐브 한 변의 절반 길이

    glBegin(GL_QUADS);

    // 앞면 - 밝은 청록색
    glColor3f(0.0f, 0.85f, 0.75f);
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);

    // 오른쪽 면 - 조금 어두운 청록색
    glColor3f(0.0f, 0.65f, 0.65f);
    glVertex3f(s, -s, s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, s);

    // 윗면 - 밝은 노란색 계열
    glColor3f(1.0f, 0.8f, 0.0f);
    glVertex3f(-s, s, s);
    glVertex3f(s, s, s);
    glVertex3f(s, s, -s);
    glVertex3f(-s, s, -s);

    // 왼쪽 면
    glColor3f(0.0f, 0.55f, 0.60f);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s, -s);

    // 뒷면
    glColor3f(0.0f, 0.45f, 0.50f);
    glVertex3f(s, -s, -s);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(s, s, -s);

    // 아랫면
    glColor3f(0.0f, 0.35f, 0.45f);
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, s);
    glVertex3f(-s, -s, s);

    glEnd();
}

// --------------------------------------------------
// 화면 출력 함수
// --------------------------------------------------
void Display()
{
    // 색상 버퍼와 깊이 버퍼를 함께 초기화
    // GL_DEPTH_BUFFER_BIT를 지워야 이전 프레임의 깊이 정보가 남지 않음
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 모델뷰 행렬 선택
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 카메라 설정
    gluLookAt(
        0.0, 0.0, 5.0,   // 카메라 위치
        0.0, 0.0, 0.0,   // 바라보는 지점
        0.0, 1.0, 0.0    // 위쪽 방향
    );

    // 장면 전체를 회전시켜 3차원 입체 형태가 보이도록 함
    glRotatef(25.0f, 1.0f, 1.0f, 0.0f);

    // 삼각형을 먼저 그림
    DrawTriangles();

    // 큐브를 나중에 그림
    // 하지만 z-buffer가 켜져 있기 때문에
    // 실제 화면에는 카메라에 더 가까운 면이 우선적으로 표시됨
    DrawCube();

    // 더블 버퍼 교체
    glutSwapBuffers();
}

// --------------------------------------------------
// 창 크기 변경 함수
// --------------------------------------------------
void Reshape(int w, int h)
{
    // 창 전체를 viewport로 사용
    glViewport(0, 0, w, h);

    // 투영 행렬 선택
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 원근 투영 설정
    gluPerspective(
        45.0,                  // 시야각
        (float)w / (float)h,   // 종횡비
        1.0,                   // near plane
        100.0                  // far plane
    );
}

// --------------------------------------------------
// 메인 함수
// --------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    // RGB 색상 버퍼 + 더블 버퍼 + 깊이 버퍼 사용
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // 윈도우 크기 설정
    glutInitWindowSize(800, 600);

    // 윈도우 생성
    glutCreateWindow("Hidden Surface Removal with Z-buffer");

    // 초기화 함수 호출
    Init();

    // 콜백 함수 등록
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);

    // GLUT 메인 루프 시작
    glutMainLoop();

    return 0;
}
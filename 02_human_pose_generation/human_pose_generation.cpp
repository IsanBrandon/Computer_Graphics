#include <string>
#include <limits>
#include <windows.h>
#include <GL/glut.h>      // 환경에 따라 <GL/freeglut.h> 로 바꿔야 할 수도 있음
#include <iostream>
#include <cmath>

using namespace std;

// ============================================================
// [1] 전역 변수
// ============================================================

// 관절 회전각 배열
// 0: torso
// 1: neck
// 2: left shoulder
// 3: left elbow
// 4: left wrist
// 5: right shoulder
// 6: right elbow
// 7: right wrist
float jointAngle[8] = { 0.0f, 0.0f, 25.0f, -20.0f, 0.0f, -25.0f, 20.0f, 0.0f };

// 카메라 회전용 각도
float cameraAngleX = 8.0f;
float cameraAngleY = 0.0f;

// 애니메이션 관련 변수
bool isAnimating = false;
float animTime = 0.0f;

// ============================================================
// [2] 신체 크기 상수
// ============================================================

const float TORSO_W = 2.2f;
const float TORSO_H = 3.6f;
const float TORSO_D = 1.1f;

const float NECK_RADIUS = 0.16f;
const float NECK_LEN = 0.40f;

const float HEAD_RADIUS = 0.62f;

const float UPPER_ARM_RADIUS = 0.18f;
const float UPPER_ARM_LEN = 1.65f;

const float LOWER_ARM_RADIUS = 0.15f;
const float LOWER_ARM_LEN = 1.35f;

const float HAND_W = 0.34f;
const float HAND_H = 0.52f;
const float HAND_D = 0.22f;

const float JOINT_RADIUS = 0.11f;

// ============================================================
// [3] 보조 함수
// ============================================================

// 원기둥을 그리는 함수
// GLU 원기둥은 기본적으로 +z 방향으로 길이가 생긴다.
void drawCylinder(float radius, float height)
{
    GLUquadric* quad = gluNewQuadric();

    gluCylinder(quad, radius, radius, height, 24, 24);

    // 아래 원판
    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0, radius, 24, 1);
    glPopMatrix();

    // 위 원판
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0, radius, 24, 1);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

// 박스(육면체)를 원하는 크기로 그리는 함수
void drawBox(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

// 관절 위치를 눈에 띄게 보이도록 구를 그리는 함수
void drawJointSphere(float radius)
{
    glColor3f(0.95f, 0.82f, 0.65f);
    glutSolidSphere(radius, 20, 20);
}

// ============================================================
// [4] 각 신체 부위 그리기
// ============================================================

// 몸체
void drawTorso()
{
    glColor3f(0.25f, 0.80f, 0.35f);   // 초록색 몸체
    drawBox(TORSO_W, TORSO_H, TORSO_D);
}

// 목
void drawNeck()
{
    glColor3f(0.95f, 0.85f, 0.65f);

    glPushMatrix();
    // 원기둥 기본축(+z)을 +y 방향으로 세우기
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(NECK_RADIUS, NECK_LEN);
    glPopMatrix();
}

// 머리
void drawHead()
{
    glColor3f(0.96f, 0.84f, 0.68f);
    glutSolidSphere(HEAD_RADIUS, 30, 30);
}

// 팔 분절 하나를 "관절 원점에서 시작하여 아래(-y)로 뻗도록" 그리는 함수
void drawArmSegment(float radius, float length, float r, float g, float b)
{
    glColor3f(r, g, b);

    glPushMatrix();
    // 원기둥 기본축(+z)을 아래(-y) 방향으로 회전
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(radius, length);
    glPopMatrix();
}

// 손
void drawHand()
{
    glColor3f(0.96f, 0.84f, 0.68f);

    glPushMatrix();
    glTranslatef(0.0f, -HAND_H * 0.5f, 0.0f);
    drawBox(HAND_W, HAND_H, HAND_D);
    glPopMatrix();
}

// ============================================================
// [5] 계층구조를 이용한 상체 전체 그리기
// ============================================================

void drawUpperBody()
{
    // 몸체(루트)
    glPushMatrix();

    // 몸체 전체 회전
    glRotatef(jointAngle[0], 0.0f, 1.0f, 0.0f);

    // 몸체
    drawTorso();

    // --------------------------------------------------------
    // 목 -> 머리
    // --------------------------------------------------------
    glPushMatrix();

    // 몸체 윗면 중앙
    glTranslatef(0.0f, TORSO_H / 2.0f, 0.0f);

    // 목 회전
    glRotatef(jointAngle[1], 0.0f, 0.0f, 1.0f);

    // 목 관절
    drawJointSphere(0.08f);

    // 목
    drawNeck();

    // 머리 위치
    glTranslatef(0.0f, NECK_LEN + HEAD_RADIUS, 0.0f);

    // 머리
    drawHead();

    glPopMatrix();

    // --------------------------------------------------------
    // 왼쪽 팔
    // --------------------------------------------------------
    glPushMatrix();

    // 왼쪽 어깨 위치를 몸통 바깥쪽으로 조금 더 이동
    glTranslatef(-(TORSO_W / 2.0f + 0.18f), TORSO_H / 2.0f - 0.40f, 0.0f);

    // 어깨 회전
    glRotatef(jointAngle[2], 0.0f, 0.0f, 1.0f);

    // 어깨 관절
    drawJointSphere(JOINT_RADIUS);

    // 위팔
    drawArmSegment(UPPER_ARM_RADIUS, UPPER_ARM_LEN, 0.35f, 0.55f, 0.95f);

    // 팔꿈치 위치
    glTranslatef(0.0f, -UPPER_ARM_LEN, 0.0f);

    // 팔꿈치 회전
    glRotatef(jointAngle[3], 0.0f, 0.0f, 1.0f);

    // 팔꿈치 관절
    drawJointSphere(JOINT_RADIUS * 0.95f);

    // 아래팔
    drawArmSegment(LOWER_ARM_RADIUS, LOWER_ARM_LEN, 0.20f, 0.40f, 0.85f);

    // 손목 위치
    glTranslatef(0.0f, -LOWER_ARM_LEN, 0.0f);

    // 손목 회전
    glRotatef(jointAngle[4], 0.0f, 0.0f, 1.0f);

    // 손목 관절
    drawJointSphere(JOINT_RADIUS * 0.85f);

    // 손
    drawHand();

    glPopMatrix();

    // --------------------------------------------------------
    // 오른쪽 팔
    // --------------------------------------------------------
    glPushMatrix();

    // 오른쪽 어깨 위치
    glTranslatef((TORSO_W / 2.0f + 0.18f), TORSO_H / 2.0f - 0.40f, 0.0f);

    // 어깨 회전
    glRotatef(jointAngle[5], 0.0f, 0.0f, 1.0f);

    // 어깨 관절
    drawJointSphere(JOINT_RADIUS);

    // 위팔
    drawArmSegment(UPPER_ARM_RADIUS, UPPER_ARM_LEN, 0.35f, 0.55f, 0.95f);

    // 팔꿈치 위치
    glTranslatef(0.0f, -UPPER_ARM_LEN, 0.0f);

    // 팔꿈치 회전
    glRotatef(jointAngle[6], 0.0f, 0.0f, 1.0f);

    // 팔꿈치 관절
    drawJointSphere(JOINT_RADIUS * 0.95f);

    // 아래팔
    drawArmSegment(LOWER_ARM_RADIUS, LOWER_ARM_LEN, 0.20f, 0.40f, 0.85f);

    // 손목 위치
    glTranslatef(0.0f, -LOWER_ARM_LEN, 0.0f);

    // 손목 회전
    glRotatef(jointAngle[7], 0.0f, 0.0f, 1.0f);

    // 손목 관절
    drawJointSphere(JOINT_RADIUS * 0.85f);

    // 손
    drawHand();

    glPopMatrix();

    glPopMatrix();
}

// ============================================================
// [6] 좌표축 그리기
// ============================================================

void drawAxes(float len)
{
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);

    // x축 - 빨강
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-len, 0.0f, 0.0f);
    glVertex3f(len, 0.0f, 0.0f);

    // y축 - 초록
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -len, 0.0f);
    glVertex3f(0.0f, len, 0.0f);

    // z축 - 파랑
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -len);
    glVertex3f(0.0f, 0.0f, len);

    glEnd();

    glEnable(GL_LIGHTING);
}

// ============================================================
// [7] 화면 출력
// ============================================================

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 카메라를 뒤로 더 빼서 몸통이 과하게 커 보이지 않게 조정
    gluLookAt(0.0, 1.8, 11.5,   // eye
        0.0, 1.2, 0.0,    // center
        0.0, 1.0, 0.0);   // up

    // 전체 모델 회전
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);

    // 좌표축
    drawAxes(4.0f);

    // 모델
    glPushMatrix();
    drawUpperBody();
    glPopMatrix();

    glutSwapBuffers();
}

// ============================================================
// [8] 창 크기 변경 시 투영 재설정
// ============================================================

void reshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 원근 왜곡을 약간 줄이기 위해 시야각 유지 + far 범위 적당히 조절
    gluPerspective(45.0, (double)w / (double)h, 1.0, 50.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ============================================================
// [9] 관절 각도 입력
// ============================================================

void inputJointAngle()
{
    int part;
    float angle;

    cout << "\n========== Joint Number List ==========\n";
    cout << "0 : torso\n";
    cout << "1 : neck\n";
    cout << "2 : left shoulder\n";
    cout << "3 : left elbow\n";
    cout << "4 : left wrist\n";
    cout << "5 : right shoulder\n";
    cout << "6 : right elbow\n";
    cout << "7 : right wrist\n";
    cout << "=======================================\n";

    cout << "Enter joint number: ";
    cin >> part;

    if (part < 0 || part > 7)
    {
        cout << "Invalid number.\n";
        return;
    }

    cout << "Enter angle (degrees): ";
    cin >> angle;

    jointAngle[part] = angle;

    cout << "Applied.\n";
    glutPostRedisplay();
}

// ============================================================
// [10] 자세 초기화
// ============================================================

void resetPose()
{
    jointAngle[0] = 0.0f;    // torso
    jointAngle[1] = 0.0f;    // neck
    jointAngle[2] = 25.0f;   // left shoulder
    jointAngle[3] = -20.0f;  // left elbow
    jointAngle[4] = 0.0f;    // left wrist
    jointAngle[5] = -25.0f;  // right shoulder
    jointAngle[6] = 20.0f;   // right elbow
    jointAngle[7] = 0.0f;    // right wrist
}

// ============================================================
// [11] 애니메이션
// ============================================================

void timer(int value)
{
    if (isAnimating)
    {
        animTime += 0.05f;

        // 오른팔 인사 동작
        jointAngle[5] = -45.0f + 12.0f * sin(animTime);
        jointAngle[6] = 50.0f + 25.0f * sin(animTime * 2.0f);
        jointAngle[7] = 18.0f * sin(animTime * 2.5f);

        // 목도 약간 움직이게 함
        jointAngle[1] = 8.0f * sin(animTime * 0.8f);
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // 약 60fps
}

// ============================================================
// [12] 키보드 입력 처리
// ============================================================

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'i':
    case 'I':
        inputJointAngle();
        break;

    case 'r':
    case 'R':
        resetPose();
        glutPostRedisplay();
        break;

    case 'a':
    case 'A':
        isAnimating = !isAnimating;
        break;

    case 27: // ESC
        exit(0);
        break;
    }
}

// 방향키로 카메라 회전
void specialKeyboard(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        cameraAngleY -= 5.0f;
        break;

    case GLUT_KEY_RIGHT:
        cameraAngleY += 5.0f;
        break;

    case GLUT_KEY_UP:
        cameraAngleX -= 5.0f;
        break;

    case GLUT_KEY_DOWN:
        cameraAngleX += 5.0f;
        break;
    }

    glutPostRedisplay();
}

// ============================================================
// [13] OpenGL 초기 설정
// ============================================================

void init()
{
    glClearColor(0.93f, 0.93f, 0.93f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // 조명 설정
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lightPos[] = { 5.0f, 8.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    GLfloat ambient[] = { 0.30f, 0.30f, 0.30f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    GLfloat diffuse[] = { 0.90f, 0.90f, 0.90f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

// ============================================================
// [14] 메인 함수
// ============================================================

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hierarchical Upper Body Modeling");

    init();
    resetPose();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutTimerFunc(16, timer, 0);

    cout << "========================================\n";
    cout << "Hierarchical Upper Body Modeling Program\n";
    cout << "i : input joint number and angle\n";
    cout << "r : reset pose\n";
    cout << "a : animation ON/OFF\n";
    cout << "Arrow keys : rotate camera\n";
    cout << "ESC : exit\n";
    cout << "========================================\n";

    glutMainLoop();
    return 0;
}
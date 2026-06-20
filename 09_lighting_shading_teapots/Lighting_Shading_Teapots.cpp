#include <GL/glut.h>
#include <cmath>

// 애니메이션 각도
float lightAngle = 0.0f;

// 조명 위치
GLfloat light0Pos[] = { 3.0f, 4.0f, 3.0f, 1.0f };
GLfloat light1Pos[] = { -3.0f, 3.0f, -3.0f, 1.0f };

// 초기 설정
void init()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // 회색 배경

    glEnable(GL_DEPTH_TEST);                 // 깊이 테스트
    glEnable(GL_LIGHTING);                   // 조명 활성화
    glEnable(GL_LIGHT0);                     // 첫 번째 조명
    glEnable(GL_LIGHT1);                     // 두 번째 조명
    glEnable(GL_NORMALIZE);                  // 스케일 후 노멀 보정

    // 전체 환경광
    GLfloat globalAmbient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // LIGHT0 설정
    GLfloat light0Ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light0Diffuse[] = { 0.9f, 0.8f, 0.7f, 1.0f };
    GLfloat light0Specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);

    // LIGHT1 설정
    GLfloat light1Ambient[] = { 0.05f, 0.05f, 0.1f, 1.0f };
    GLfloat light1Diffuse[] = { 0.4f, 0.6f, 1.0f, 1.0f };
    GLfloat light1Specular[] = { 0.7f, 0.8f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1Ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Specular);
}

// 바닥 그리기
void drawFloor()
{
    GLfloat floorAmbient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat floorDiffuse[] = { 0.45f, 0.45f, 0.45f, 1.0f };
    GLfloat floorSpecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat floorShininess[] = { 5.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, floorAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floorDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floorSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, floorShininess);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-6.0f, -1.0f, -6.0f);
    glVertex3f(6.0f, -1.0f, -6.0f);
    glVertex3f(6.0f, -1.0f, 6.0f);
    glVertex3f(-6.0f, -1.0f, 6.0f);
    glEnd();
}

// 첫 번째 주전자: 붉은 유광 재질
void drawRedTeapot()
{
    GLfloat ambient[] = { 0.25f, 0.05f, 0.05f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.1f, 0.1f, 1.0f };
    GLfloat specular[] = { 1.0f, 0.8f, 0.8f, 1.0f };
    GLfloat shininess[] = { 80.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
    glTranslatef(-1.7f, 0.0f, 0.0f);
    glutSolidTeapot(0.8);
    glPopMatrix();
}

// 두 번째 주전자: 푸른 무광 재질
void drawBlueTeapot()
{
    GLfloat ambient[] = { 0.05f, 0.05f, 0.25f, 1.0f };
    GLfloat diffuse[] = { 0.1f, 0.2f, 0.8f, 1.0f };
    GLfloat specular[] = { 0.2f, 0.2f, 0.3f, 1.0f };
    GLfloat shininess[] = { 15.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
    glTranslatef(1.7f, 0.0f, 0.0f);
    glutSolidTeapot(0.8);
    glPopMatrix();
}

// 조명 위치 표시용 작은 구
void drawLightSphere(GLfloat pos[], float r, float g, float b)
{
    glDisable(GL_LIGHTING);

    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glutSolidSphere(0.12, 20, 20);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

// 화면 출력
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        0.0, 3.0, 7.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0
    );

    // 시간에 따라 움직이는 조명 위치 계산
    light0Pos[0] = 3.0f * cos(lightAngle);
    light0Pos[2] = 3.0f * sin(lightAngle);
    light0Pos[1] = 3.0f;

    light1Pos[0] = 3.0f * cos(-lightAngle * 0.7f);
    light1Pos[2] = 3.0f * sin(-lightAngle * 0.7f);
    light1Pos[1] = 2.0f + sin(lightAngle) * 1.0f;

    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

    drawFloor();
    drawRedTeapot();
    drawBlueTeapot();

    drawLightSphere(light0Pos, 1.0f, 0.9f, 0.3f);
    drawLightSphere(light1Pos, 0.3f, 0.5f, 1.0f);

    glutSwapBuffers();
}

// 창 크기 변경
void reshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

// 애니메이션
void timer(int value)
{
    lightAngle += 0.03f;

    if (lightAngle > 6.28318f)
        lightAngle = 0.0f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// 메인 함수
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Lighting and Shading Teapot Scene");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return 0;
}
#include <GL/glut.h>   // GLUT 라이브러리: 창 생성, 입력 처리, 기본 도형 출력 함수 제공
#include <GL/gl.h>     // OpenGL 기본 함수 제공
#include <GL/glu.h>    // GLU 유틸리티 함수 제공, 예: gluLookAt()


void InitLight() {                                      // 조명과 재질을 초기화하는 함수
    GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };     // 물체 표면의 확산 반사 색상 (빛을 받았을 때 물체가 기본적으로 보이는 색)
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };    // 물체 표면의 정반사 색상 (하이라이트 부분의 색)
    GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };     // 물체 표면의 주변광 반사 색상 (직접 빛을 받지 않는 부분의 기본 밝기)
    GLfloat mat_shininess[] = { 50.0 };                 // 정반사 광택 정도 (값이 클수록 하이라이트가 작고 날카로워짐)
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };  // 광원의 정반사 성분
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };   // 광원의 확산광 성분
    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };   // 광원의 주변광 성분

    GLfloat light_position[] = { -3, 2, 3.0, 0.0 };     // 광원의 위치 또는 방향 (마지막 값이 0.0이면 방향광)
    // 즉, (-3, 2, 3) 방향에서 빛이 들어오는 것처럼 처리됨

    glShadeModel(GL_SMOOTH);                            // 부드러운 음영 처리 (면 단위가 아니라 정점 간 색을 보간하여 자연스럽게 보이게 함)
    glEnable(GL_LIGHTING);                              // OpenGL 조명 계산 활성화
    glEnable(GL_LIGHT0);                                // 첫 번째 광원 GL_LIGHT0 활성화
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // GL_LIGHT0의 위치 또는 방향 설정
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);    // GL_LIGHT0의 확산광 설정
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // GL_LIGHT0의 정반사광 설정
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);    // GL_LIGHT0의 주변광 설정
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);    // 물체 앞면의 확산 반사 재질 설정
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);  // 물체 앞면의 정반사 재질 설정
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);    // 물체 앞면의 주변광 반사 재질 설정
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);// 물체 앞면의 광택 정도 설정
}


void InitVisibility() {         // 보이는 면 처리와 깊이 테스트를 초기화하는 함수 
    glEnable(GL_CULL_FACE);     // 뒷면 제거 기능 활성화 (보이지 않는 면을 그리지 않아서 렌더링 효율을 높임)
    glFrontFace(GL_CW);         // 시계 방향으로 정의된 면을 앞면으로 간주 (기본값은 GL_CCW이므로 여기서는 의도적으로 반대로 설정한 것)
    glCullFace(GL_BACK);        // 뒷면을 제거
    glEnable(GL_DEPTH_TEST);    // 깊이 테스트 활성화 (가까운 물체가 먼 물체를 가리도록 함)
}


void MyDisplay() {              // 화면을 실제로 그리는 디스플레이 콜백 함수
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // 색상 버퍼와 깊이 버퍼를 모두 초기화 (깊이 버퍼를 지우지 않으면 이전 프레임의 깊이 정보가 남을 수 있음)
    glMatrixMode(GL_MODELVIEW); // 이후 행렬 연산을 모델뷰 행렬에 적용
    glLoadIdentity();           // 모델뷰 행렬을 단위행렬로 초기화

    gluLookAt(                  // 카메라 위치와 바라보는 방향 설정
        0.0, 0.4, 0.5,          // eye:    (0.0, 0.4, 0.5)  카메라 위치
        0.0, -0.5, -1.0,        // center: (0.0, -0.5, -1.0) 카메라가 바라보는 지점
        0.0, 1.0, 0.0           // up:     (0.0, 1.0, 0.0)  화면의 위쪽 방향
    );

    glutSolidTeapot(0.58);      // 반지름 크기 0.58의 솔리드 주전자 출력
    glFlush();                  // 현재까지의 OpenGL 명령을 강제로 실행 (단일 버퍼 모드에서는 glFlush() 사용)
}


void MyReshape(int w, int h) {  // 창 크기가 바뀔 때 호출되는 콜백 함수
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);   // OpenGL이 그림을 그릴 영역을 창 전체로 설정
    glMatrixMode(GL_PROJECTION);                // 이후 행렬 연산을 투영 행렬에 적용
    glLoadIdentity();                           // 투영 행렬 초기화

    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 2.0);   // 직교 투영 설정
    // x 범위: -1.0 ~ 1.0
    // y 범위: -1.0 ~ 1.0
    // z 범위: -1.0 ~ 2.0
}


int main(int argc, char** argv) {   // 프로그램 시작점
    glutInit(&argc, argv);          // GLUT 초기화

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);    // 디스플레이 모드 설정
    // GLUT_SINGLE: 단일 버퍼 사용
    // GLUT_RGBA: RGBA 색상 모드 사용
    // GLUT_DEPTH: 깊이 버퍼 사용

    glutInitWindowSize(700, 700);               // 윈도우 크기 설정
    glutInitWindowPosition(0, 0);               // 윈도우 초기 위치 설정
    glutCreateWindow("OpenGL Sample Drawing");  // 윈도우 생성
    glClearColor(0.5, 0.5, 0.5, 0.0);           // 배경색 설정(회색 배경)
    InitLight();                                // 조명과 재질 초기화
    InitVisibility();                           // 은면 제거와 깊이 테스트 초기화
    glutDisplayFunc(MyDisplay);                 // 화면 출력 함수 등록
    glutReshapeFunc(MyReshape);                 // 창 크기 변경 함수 등록

    glutMainLoop(); // GLUT 이벤트 루프 시작 (이 함수 이후에는 사용자가 창을 닫기 전까지 계속 실행됨)

    return 0;
}
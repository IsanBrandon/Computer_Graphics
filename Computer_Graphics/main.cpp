#include <windows.h>
#include <GL/glut.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ===============================
// Basic Math
// ===============================

struct Vec3
{
    float x, y, z;
};

struct Mat4
{
    float m[16] = { 0.0f };
};

Mat4 Identity()
{
    Mat4 r;
    r.m[0] = 1.0f;
    r.m[5] = 1.0f;
    r.m[10] = 1.0f;
    r.m[15] = 1.0f;
    return r;
}

Mat4 Multiply(const Mat4& a, const Mat4& b)
{
    Mat4 r;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            r.m[col * 4 + row] =
                a.m[0 * 4 + row] * b.m[col * 4 + 0] +
                a.m[1 * 4 + row] * b.m[col * 4 + 1] +
                a.m[2 * 4 + row] * b.m[col * 4 + 2] +
                a.m[3 * 4 + row] * b.m[col * 4 + 3];
        }
    }

    return r;
}

Mat4 Translate(float x, float y, float z)
{
    Mat4 r = Identity();
    r.m[12] = x;
    r.m[13] = y;
    r.m[14] = z;
    return r;
}

Mat4 RotateX(float degree)
{
    Mat4 r = Identity();

    float rad = degree * 3.141592f / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    r.m[5] = c;
    r.m[6] = s;
    r.m[9] = -s;
    r.m[10] = c;

    return r;
}

Mat4 RotateY(float degree)
{
    Mat4 r = Identity();

    float rad = degree * 3.141592f / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    r.m[0] = c;
    r.m[2] = -s;
    r.m[8] = s;
    r.m[10] = c;

    return r;
}

Mat4 RotateZ(float degree)
{
    Mat4 r = Identity();

    float rad = degree * 3.141592f / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    r.m[0] = c;
    r.m[1] = s;
    r.m[4] = -s;
    r.m[5] = c;

    return r;
}

Vec3 GetTranslation(const Mat4& mat)
{
    return { mat.m[12], mat.m[13], mat.m[14] };
}

Vec3 Subtract(const Vec3& a, const Vec3& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vec3 Cross(const Vec3& a, const Vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float Dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Length(const Vec3& v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 Normalize(const Vec3& v)
{
    float len = Length(v);

    if (len < 0.0001f)
        return { 0.0f, 0.0f, 0.0f };

    return { v.x / len, v.y / len, v.z / len };
}

// ===============================
// Joint Structure
// ===============================

struct Joint
{
    string name;

    float offset[3] = { 0.0f, 0.0f, 0.0f };

    vector<string> channels;
    vector<Joint*> children;

    bool isEndSite = false;
    Mat4 globalTransform;
};

// ===============================
// Global Data
// ===============================

Joint* root = nullptr;

int frameCount = 0;
float frameTime = 0.0f;
int totalJointCount = 0;

vector<vector<float>> motionData;

int currentFrame = 0;
bool isPlaying = true;
float playbackSpeed = 1.0f;

bool showGrid = true;
bool showAxis = true;
bool showJoints = true;
bool showBones = true;
bool showBodyMesh = true;
bool useCylinderBone = false;

float cameraDistance = 250.0f;
float cameraYaw = 45.0f;
float cameraPitch = 20.0f;

int windowWidth = 1000;
int windowHeight = 700;

// ===============================
// Utility
// ===============================

bool Contains(const string& text, const string& pattern)
{
    return text.find(pattern) != string::npos;
}

// ===============================
// BVH Parser
// ===============================

Joint* ParseJoint(ifstream& file, const string& jointType)
{
    string name;

    if (jointType == "End")
    {
        string site;
        file >> site;
        name = "EndSite";
    }
    else
    {
        file >> name;
    }

    Joint* joint = new Joint();
    joint->name = name;
    joint->isEndSite = (jointType == "End");
    joint->globalTransform = Identity();

    totalJointCount++;

    string token;
    file >> token; // {

    while (file >> token)
    {
        if (token == "OFFSET")
        {
            file >> joint->offset[0]
                >> joint->offset[1]
                >> joint->offset[2];
        }
        else if (token == "CHANNELS")
        {
            int channelCount;
            file >> channelCount;

            for (int i = 0; i < channelCount; i++)
            {
                string channel;
                file >> channel;
                joint->channels.push_back(channel);
            }
        }
        else if (token == "JOINT")
        {
            Joint* child = ParseJoint(file, "JOINT");
            joint->children.push_back(child);
        }
        else if (token == "End")
        {
            Joint* child = ParseJoint(file, "End");
            joint->children.push_back(child);
        }
        else if (token == "}")
        {
            break;
        }
    }

    return joint;
}

bool LoadBVH(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Failed to open BVH file: " << filename << endl;
        return false;
    }

    string token;

    file >> token;
    if (token != "HIERARCHY")
        return false;

    file >> token;
    if (token != "ROOT")
        return false;

    root = ParseJoint(file, "ROOT");

    file >> token;
    if (token != "MOTION")
        return false;

    file >> token; // Frames:
    file >> frameCount;

    file >> token; // Frame
    file >> token; // Time:
    file >> frameTime;

    motionData.resize(frameCount);

    string line;
    getline(file, line);

    for (int i = 0; i < frameCount; i++)
    {
        getline(file, line);

        stringstream ss(line);
        float value;

        while (ss >> value)
        {
            motionData[i].push_back(value);
        }
    }

    cout << "==============================" << endl;
    cout << "BVH Load Complete" << endl;
    cout << "Frames: " << frameCount << endl;
    cout << "Frame Time: " << frameTime << endl;
    cout << "Total Joints: " << totalJointCount << endl;

    if (!motionData.empty())
        cout << "Channels per Frame: " << motionData[0].size() << endl;

    cout << "==============================" << endl;

    return true;
}

// ===============================
// Forward Kinematics
// ===============================

Mat4 MakeLocalTransform(Joint* joint, const vector<float>& frameData, int& channelIndex)
{
    Mat4 local = Translate(
        joint->offset[0],
        joint->offset[1],
        joint->offset[2]
    );

    if (!joint->isEndSite)
    {
        for (size_t i = 0; i < joint->channels.size(); i++)
        {
            if (channelIndex >= (int)frameData.size())
                break;

            string channel = joint->channels[i];
            float value = frameData[channelIndex++];

            Mat4 t = Identity();

            if (channel == "Xposition")
                t = Translate(value, 0.0f, 0.0f);
            else if (channel == "Yposition")
                t = Translate(0.0f, value, 0.0f);
            else if (channel == "Zposition")
                t = Translate(0.0f, 0.0f, value);
            else if (channel == "Xrotation")
                t = RotateX(value);
            else if (channel == "Yrotation")
                t = RotateY(value);
            else if (channel == "Zrotation")
                t = RotateZ(value);

            local = Multiply(local, t);
        }
    }

    return local;
}

void UpdatePose(Joint* joint, const vector<float>& frameData, int& channelIndex, const Mat4& parentTransform)
{
    if (joint == nullptr)
        return;

    Mat4 local = MakeLocalTransform(joint, frameData, channelIndex);
    joint->globalTransform = Multiply(parentTransform, local);

    for (Joint* child : joint->children)
    {
        UpdatePose(child, frameData, channelIndex, joint->globalTransform);
    }
}

// ===============================
// Drawing Utilities
// ===============================

void DrawGrid(float size = 200.0f, float step = 10.0f)
{
    glColor3f(0.25f, 0.25f, 0.25f);
    glLineWidth(1.0f);

    glBegin(GL_LINES);

    for (float i = -size; i <= size; i += step)
    {
        glVertex3f(i, 0.0f, -size);
        glVertex3f(i, 0.0f, size);

        glVertex3f(-size, 0.0f, i);
        glVertex3f(size, 0.0f, i);
    }

    glEnd();
}

void DrawAxis(float length = 80.0f)
{
    glLineWidth(3.0f);

    glColor3f(1.0f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.1f, 1.0f, 0.1f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);
    glEnd();

    glColor3f(0.1f, 0.3f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);
    glEnd();

    glLineWidth(1.0f);
}

float GetJointRadius(Joint* joint)
{
    if (joint->isEndSite)
        return 0.9f;

    if (Contains(joint->name, "Head"))
        return 6.0f;

    if (Contains(joint->name, "Hips") || Contains(joint->name, "Chest"))
        return 3.0f;

    return 2.0f;
}

float GetBoneRadius(Joint* parent, Joint* child)
{
    string p = parent->name;
    string c = child->name;

    if (Contains(p, "Hips") || Contains(p, "Chest") || Contains(c, "Chest"))
        return 1.5f;

    if (Contains(c, "Hip") || Contains(c, "Knee") || Contains(c, "Ankle"))
        return 1.25f;

    if (Contains(c, "Shoulder") || Contains(c, "Elbow") || Contains(c, "Wrist"))
        return 1.0f;

    if (child->isEndSite)
        return 0.6f;

    return 1.0f;
}

void DrawSphereAt(const Vec3& p, float radius)
{
    glPushMatrix();

    glTranslatef(p.x, p.y, p.z);
    glutSolidSphere(radius, 16, 16);

    glPopMatrix();
}

void DrawCubeAt(const Vec3& p, float sx, float sy, float sz)
{
    glPushMatrix();

    glTranslatef(p.x, p.y, p.z);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);

    glPopMatrix();
}

void DrawLineBetween(const Vec3& a, const Vec3& b)
{
    glColor3f(0.85f, 0.85f, 0.85f);
    glLineWidth(4.0f);

    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();

    glLineWidth(1.0f);
}

void DrawCylinderBetween(const Vec3& a, const Vec3& b, float radius)
{
    Vec3 dir = Subtract(b, a);
    float length = Length(dir);

    if (length < 0.001f)
        return;

    Vec3 zAxis = { 0.0f, 0.0f, 1.0f };
    Vec3 dirNorm = Normalize(dir);

    Vec3 rotationAxis = Cross(zAxis, dirNorm);
    float dot = Dot(zAxis, dirNorm);

    if (dot > 1.0f)
        dot = 1.0f;

    if (dot < -1.0f)
        dot = -1.0f;

    float angle = acosf(dot) * 180.0f / 3.141592f;

    glPushMatrix();

    glTranslatef(a.x, a.y, a.z);

    if (Length(rotationAxis) > 0.0001f)
    {
        rotationAxis = Normalize(rotationAxis);
        glRotatef(angle, rotationAxis.x, rotationAxis.y, rotationAxis.z);
    }
    else if (dot < 0.0f)
    {
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    }

    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, length, 12, 1);
    gluDeleteQuadric(quad);

    glPopMatrix();
}

void DrawBodyMesh(Joint* joint, const Vec3& p)
{
    if (!showBodyMesh || joint->isEndSite)
        return;

    if (joint->name == "Chest")
    {
        glColor3f(0.35f, 0.55f, 0.9f);
        DrawCubeAt(p, 14.0f, 20.0f, 8.0f);
    }
    else if (joint->name == "Hips")
    {
        glColor3f(0.25f, 0.45f, 0.8f);
        DrawCubeAt(p, 12.0f, 8.0f, 7.0f);
    }
}

// ===============================
// Skeleton Rendering
// ===============================

void DrawSkeleton(Joint* joint)
{
    if (joint == nullptr)
        return;

    Vec3 p = GetTranslation(joint->globalTransform);

    DrawBodyMesh(joint, p);

    if (showJoints)
    {
        if (joint->isEndSite)
            glColor3f(0.9f, 0.6f, 0.1f);
        else if (Contains(joint->name, "Head"))
            glColor3f(1.0f, 0.8f, 0.45f);
        else
            glColor3f(1.0f, 0.72f, 0.18f);

        DrawSphereAt(p, GetJointRadius(joint));
    }

    for (Joint* child : joint->children)
    {
        Vec3 c = GetTranslation(child->globalTransform);

        if (showBones)
        {
            glColor3f(0.82f, 0.82f, 0.82f);

            if (useCylinderBone)
                DrawCylinderBetween(p, c, GetBoneRadius(joint, child));
            else
                DrawLineBetween(p, c);
        }

        DrawSkeleton(child);
    }
}

// ===============================
// HUD
// ===============================

void DrawText(float x, float y, const string& text)
{
    glRasterPos2f(x, y);

    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
}

void DrawHUD()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);

    int y = windowHeight - 25;

    DrawText(15, y, "BVH 3D Human Model Viewer"); y -= 20;
    DrawText(15, y, "Frame: " + to_string(currentFrame + 1) + " / " + to_string(frameCount)); y -= 20;
    DrawText(15, y, string("State: ") + (isPlaying ? "Playing" : "Paused")); y -= 20;
    DrawText(15, y, "Speed: " + to_string(playbackSpeed) + "x"); y -= 20;
    DrawText(15, y, "Joints: " + to_string(totalJointCount)); y -= 20;
    DrawText(15, y, string("Body Mesh: ") + (showBodyMesh ? "ON" : "OFF")); y -= 20;
    DrawText(15, y, string("Bone Mode: ") + (useCylinderBone ? "Cylinder" : "Line")); y -= 30;

    DrawText(15, y, "[Space] Play/Pause"); y -= 20;
    DrawText(15, y, "[Left/Right] Prev/Next Frame"); y -= 20;
    DrawText(15, y, "[+/-] Speed Control"); y -= 20;
    DrawText(15, y, "[R] Reset Frame"); y -= 20;
    DrawText(15, y, "[1] Grid  [2] Axis  [3] Joints  [4] Bones"); y -= 20;
    DrawText(15, y, "[5] Cylinder/Line Bone  [6] Body Mesh"); y -= 20;
    DrawText(15, y, "[WASD] Camera Rotate  [Q/E] Zoom");

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

// ===============================
// Camera
// ===============================

void SetupCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        45.0,
        (double)windowWidth / (double)windowHeight,
        0.1,
        2000.0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float yawRad = cameraYaw * 3.141592f / 180.0f;
    float pitchRad = cameraPitch * 3.141592f / 180.0f;

    float camX = cameraDistance * cosf(pitchRad) * sinf(yawRad);
    float camY = cameraDistance * sinf(pitchRad);
    float camZ = cameraDistance * cosf(pitchRad) * cosf(yawRad);

    gluLookAt(
        camX, camY, camZ,
        0.0, 60.0, 0.0,
        0.0, 1.0, 0.0
    );
}

// ===============================
// Callbacks
// ===============================

void display()
{
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetupCamera();

    if (showGrid) DrawGrid();
    if (showAxis) DrawAxis();

    if (root != nullptr && !motionData.empty())
    {
        int channelIndex = 0;
        UpdatePose(root, motionData[currentFrame], channelIndex, Identity());
        DrawSkeleton(root);
    }

    DrawHUD();

    glutSwapBuffers();
}

void timer(int value)
{
    if (isPlaying && frameCount > 0)
        currentFrame = (currentFrame + 1) % frameCount;

    glutPostRedisplay();

    int delay = 16;

    if (frameTime > 0.0f)
    {
        delay = (int)((frameTime * 1000.0f) / playbackSpeed);

        if (delay < 1)
            delay = 1;
    }

    glutTimerFunc(delay, timer, 0);
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;

    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;

    case ' ':
        isPlaying = !isPlaying;
        break;

    case 'r':
    case 'R':
        currentFrame = 0;
        break;

    case '+':
    case '=':
        playbackSpeed += 0.25f;
        if (playbackSpeed > 4.0f)
            playbackSpeed = 4.0f;
        break;

    case '-':
    case '_':
        playbackSpeed -= 0.25f;
        if (playbackSpeed < 0.25f)
            playbackSpeed = 0.25f;
        break;

    case '1':
        showGrid = !showGrid;
        break;

    case '2':
        showAxis = !showAxis;
        break;

    case '3':
        showJoints = !showJoints;
        break;

    case '4':
        showBones = !showBones;
        break;

    case '5':
        useCylinderBone = !useCylinderBone;
        break;

    case '6':
        showBodyMesh = !showBodyMesh;
        break;

    case 'a':
    case 'A':
        cameraYaw -= 5.0f;
        break;

    case 'd':
    case 'D':
        cameraYaw += 5.0f;
        break;

    case 'w':
    case 'W':
        cameraPitch += 5.0f;
        break;

    case 's':
    case 'S':
        cameraPitch -= 5.0f;
        break;

    case 'q':
    case 'Q':
        cameraDistance -= 10.0f;
        if (cameraDistance < 30.0f)
            cameraDistance = 30.0f;
        break;

    case 'e':
    case 'E':
        cameraDistance += 10.0f;
        break;
    }

    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
    if (frameCount <= 0)
        return;

    switch (key)
    {
    case GLUT_KEY_RIGHT:
        isPlaying = false;
        currentFrame = (currentFrame + 1) % frameCount;
        break;

    case GLUT_KEY_LEFT:
        isPlaying = false;
        currentFrame = (currentFrame - 1 + frameCount) % frameCount;
        break;
    }

    glutPostRedisplay();
}

// ===============================
// Main
// ===============================

int main(int argc, char** argv)
{
    bool loaded = LoadBVH("sample.bvh");

    if (!loaded)
        cout << "BVH loading failed." << endl;

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("BVH 3D Human Model Viewer - 202011797_Park Jae-Hyuk");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    int delay = 16;

    if (frameTime > 0.0f)
    {
        delay = (int)(frameTime * 1000.0f);

        if (delay < 1)
            delay = 1;
    }

    glutTimerFunc(delay, timer, 0);

    glutMainLoop();

    return 0;
}
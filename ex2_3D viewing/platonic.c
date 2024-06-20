#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define _PI 3.14159f
#define SCALE_FACTOR 0.5f
#define TRANSLATE_FACTOR 1.5f

int recursive_level = 1;
static int use_my_transforms = 1; // 0: Use OpenGL, 1: Use custom

enum { TRANS, ROT, SCALE };
static int g_op_mode = TRANS;

enum { PERSP, ORTHO };
static int g_proj_mode = PERSP;

// window dimension
static int g_width = 800;
static int g_height = 800;

// angle (in degree) to rotate around x, y, z
static GLfloat g_angle[3] = { 0.0f, 0.0f, 0.0f };

// amount to translate along x, y, z
static GLfloat g_trans[3] = { 0.0f, 0.0f, 0.0f };

// scaling factor along x, y, z
static GLfloat g_scale[3] = { 1.0f, 1.0f, 1.0f };

void myTranslatef(GLfloat tx, GLfloat ty, GLfloat tz);
void myScalef(GLfloat sx, GLfloat sy, GLfloat sz);
void myRotatef(GLfloat theta, GLfloat kx, GLfloat ky, GLfloat kz);
void myOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
GLfloat near, GLfloat far);
void myPerspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);

void normalize(float v[3]) {
    float len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    for (int i = 0; i < 3; i++) v[i] /= len;
}

void cross(const float u[3], const float v[3], float result[3]) {
    result[0] = u[1] * v[2] - u[2] * v[1];
    result[1] = u[2] * v[0] - u[0] * v[2];
    result[2] = u[0] * v[1] - u[1] * v[0];
}

void myLookAt(float eyeX, float eyeY, float eyeZ,
              float centerX, float centerY, float centerZ,
              float upX, float upY, float upZ) {
    float forward[3], side[3], up[3];
    float m[16];

    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;
    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;

    normalize(forward);
    cross(forward, up, side);
    normalize(side);
    cross(side, forward, up);

    m[0] = side[0]; m[4] = side[1]; m[8] = side[2]; m[12] = 0.0;
    m[1] = up[0];   m[5] = up[1];   m[9] = up[2];   m[13] = 0.0;
    m[2] = -forward[0]; m[6] = -forward[1]; m[10] = -forward[2]; m[14] = 0.0;
    m[3] = 0.0;     m[7] = 0.0;     m[11] = 0.0;    m[15] = 1.0;

    glMultMatrixf(m);
    if (use_my_transforms) {
        myTranslatef(-eyeX, -eyeY, -eyeZ);
    } else {
        glTranslatef(-eyeX, -eyeY, -eyeZ);
    }
}

// Model-View transforms
void myTranslatef(GLfloat tx, GLfloat ty, GLfloat tz) {
    GLfloat m[16] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        tx, ty, tz, 1.0
    };
    glMultMatrixf(m);
}

void myRotatef(GLfloat theta, GLfloat kx, GLfloat ky, GLfloat kz) {
    GLfloat c = cos(theta * _PI / 180.0);
    GLfloat s = sin(theta * _PI / 180.0);
    GLfloat len = sqrt(kx * kx + ky * ky + kz * kz);
    if (len != 0.0f) {
        kx /= len;
        ky /= len;
        kz /= len;
    }

    GLfloat m[16] = {
        kx * kx * (1 - c) + c, ky * kx * (1 - c) + kz * s, kz * kx * (1 - c) - ky * s, 0.0,
        kx * ky * (1 - c) - kz * s, ky * ky * (1 - c) + c, kz * ky * (1 - c) + kx * s, 0.0,
        kx * kz * (1 - c) + ky * s, ky * kz * (1 - c) - kx * s, kz * kz * (1 - c) + c, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    glMultMatrixf(m);
}

void myScalef(GLfloat sx, GLfloat sy, GLfloat sz) {
    GLfloat m[16] = {
        sx, 0.0, 0.0, 0.0,
        0.0, sy, 0.0, 0.0,
        0.0, 0.0, sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    glMultMatrixf(m);
}

// Projection transforms
void myOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far) {
    GLfloat m[16] = {
        2.0 / (right - left), 0.0, 0.0, 0.0,
        0.0, 2.0 / (top - bottom), 0.0, 0.0,
        0.0, 0.0, -2.0 / (far - near), 0.0,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0
    };
    glMultMatrixf(m);
}

void myPerspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {
    GLfloat f = 1.0 / tan(fovy * _PI / 360.0);
    GLfloat m[16] = {
        f / aspect, 0.0, 0.0, 0.0,
        0.0, f, 0.0, 0.0,
        0.0, 0.0, (far + near) / (near - far), -1.0,
        0.0, 0.0, (2.0 * far * near) / (near - far), 0.0
    };
    glMultMatrixf(m);
}

static void drawIcosahedron(void) {
    GLfloat phi = (1.f + sqrtf(5.f)) * .5f;
    GLfloat a = 1.f;
    GLfloat b = 1.f / phi;

    GLfloat vertices[12][3] = { {0.f,b,-a}, {b,a,0.f}, {-b,a,0.f},
                                {0.f,b,a}, {0.f,-b,a}, {-a,0.f,b},
                                {0.f,-b,-a}, {a,0.f,-b}, {a,0.f,b},
                                {-a,0.f,-b},{b,-a,0.f},{-b,-a,0.f} };

    GLfloat color[20][3] = { {0.0f,0.0f,0.6f}, {0.0f,0.0f,0.8f}, {0.0f,0.0f,1.0f},
                             {0.f, 0.2f, 1.f}, {0.f,0.4f,1.f}, {0.f,0.6f,1.f}, {0.f,0.8f,1.f}, {0.f,1.f,1.f},
                             {0.2f,1.f,0.8f}, {0.4f,1.f,0.6f}, {0.6f,1.f,0.4f}, {0.8f,1.f,0.2f},
                             {1.f,1.f,0.f}, {1.f,0.8f,0.f}, {1.f,0.6f,0.f}, {1.f,0.4f,0.f}, {1.f,0.2f,0.f},
                             {1.f,0.f,0.f}, {0.8f,0.f,0.f}, {0.6f,0.f,0.f} };

    int faces[20][3] = { {2,1,0}, {1,2,3}, {5,4,3}, {4,8,3},
                         {7,6,0}, {6,9,0}, {11,10,4}, {10,11,6},
                         {9,5,2}, {5,9,11}, {8,7,1}, {7,8,10},
                         {2,5,3}, {8,1,3}, {9,2,0}, {1,7,0},
                         {11,9,6}, {7,10,6}, {5,11,4}, {10,8,4} };

    for (int i = 0; i < 20; ++i) {
        GLfloat* c = color[i];
        int* f = faces[i];
        glColor3f(c[0], c[1], c[2]);
        glBegin(GL_TRIANGLES);
        glVertex3f(vertices[f[0]][0], vertices[f[0]][1], vertices[f[0]][2]);
        glVertex3f(vertices[f[1]][0], vertices[f[1]][1], vertices[f[1]][2]);
        glVertex3f(vertices[f[2]][0], vertices[f[2]][1], vertices[f[2]][2]);
        glEnd();
    }
}

static void drawRecursiveIcosahedron(int level) {
    if (level == 0) return;
    drawIcosahedron();

    for (int i = 0; i < 2; i++) {
        float num = (i == 0) ? 1.0f : -1.0f;
        glPushMatrix();
        if (use_my_transforms) {
            myTranslatef(num * 1.0f, -1.5f, 0.0f);
            myScalef(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
        } else {
            glTranslatef(num * 1.0f, -1.5f, 0.0f);
            glScalef(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
        }
        drawRecursiveIcosahedron(level - 1);
        glPopMatrix();
    }
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (g_proj_mode == PERSP) {
        if (use_my_transforms) {
            myPerspective(45.0, (GLfloat)g_width / (GLfloat)g_height, 0.1, 20.0);
        } else {
            gluPerspective(45.0, (GLfloat)g_width / (GLfloat)g_height, 0.1, 20.0);
        }
    } else {
        if (use_my_transforms) {
            myOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
        } else {
            glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
        }
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    myLookAt(0.0, 0.0, 10.0,
             0.0, 0.0, 0.0,
             0.0, 1.0, 0.0);

    glPushMatrix();

    if (use_my_transforms) {
        myTranslatef(g_trans[0], g_trans[1], g_trans[2]);
        myRotatef(g_angle[0], 1.0f, 0.0f, 0.0f);
        myRotatef(g_angle[1], 0.0f, 1.0f, 0.0f);
        myRotatef(g_angle[2], 0.0f, 0.0f, 1.0f);
        myScalef(g_scale[0], g_scale[1], g_scale[2]);
    } else {
        glTranslatef(g_trans[0], g_trans[1], g_trans[2]);
        glRotatef(g_angle[0], 1.0f, 0.0f, 0.0f);
        glRotatef(g_angle[1], 0.0f, 1.0f, 0.0f);
        glRotatef(g_angle[2], 0.0f, 0.0f, 1.0f);
        glScalef(g_scale[0], g_scale[1], g_scale[2]);
    }

    drawRecursiveIcosahedron(recursive_level);

    glPopMatrix();

    glutSwapBuffers();
}

static void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    g_width = w;
    g_height = h;
}

// Increase the rotation angle by amt around ax
static void rotate(int ax, GLfloat amt) {
    g_angle[ax] += amt;
}

// Increase the translation by amt along ax
static void translate(int ax, GLfloat amt) {
    g_trans[ax] += amt;
}

// Multiply the scaling factor by amt along ax
static void scale(int ax, GLfloat amt) {
    g_scale[ax] *= amt;
}

static void keyboard(unsigned char k, int x, int y) {
    switch (k) {
        case 27:
            exit(EXIT_SUCCESS);
            break;

        // Allow to switch between OpenGL transformations and your implementations
        case 'u':
            use_my_transforms = !use_my_transforms;
            break;

        case 'p':
            g_proj_mode = (1 - g_proj_mode);
            break;

        case 't':
        case 'T':
            g_op_mode = TRANS;
            break;

        case 'r':
        case 'R':
            g_op_mode = ROT;
            break;

        case 's':
        case 'S':
            g_op_mode = SCALE;
            break;

        case 'n':
            if (recursive_level < 5) recursive_level++;
            break;

        case 'N':
            if (recursive_level > 0) recursive_level--;
            break;

        case 'x':
            if (g_op_mode == TRANS) translate(0, -0.5f);
            if (g_op_mode == ROT) rotate(0, -5.0f);
            if (g_op_mode == SCALE) scale(0, 0.9f);
            break;

        case 'X':
            if (g_op_mode == TRANS) translate(0, 0.5f);
            if (g_op_mode == ROT) rotate(0, 5.0f);
            if (g_op_mode == SCALE) scale(0, 1.1f);
            break;

        case 'y':
            if (g_op_mode == TRANS) translate(1, -0.5f);
            if (g_op_mode == ROT) rotate(1, -5.0f);
            if (g_op_mode == SCALE) scale(1, 0.9f);
            break;

        case 'Y':
            if (g_op_mode == TRANS) translate(1, 0.5f);
            if (g_op_mode == ROT) rotate(1, 5.0f);
            if (g_op_mode == SCALE) scale(1, 1.1f);
            break;

        case 'z':
            if (g_op_mode == TRANS) translate(2, -0.5f);
            if (g_op_mode == ROT) rotate(2, -5.0f);
            if (g_op_mode == SCALE) scale(2, 0.9f);
            break;

        case 'Z':
            if (g_op_mode == TRANS) translate(2, 0.5f);
            if (g_op_mode == ROT) rotate(2, 5.0f);
            if (g_op_mode == SCALE) scale(2, 1.1f);
            break;

        default:
            break;
    }

    glutPostRedisplay();
}

static void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Icosahedron");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}

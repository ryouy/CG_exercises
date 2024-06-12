#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

const int N = 10000;

float SmallTriangles[N][2];

float BigTriangles[3][2] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.5f, 1.0f}
};

/*  for better visualization
float BigTriangles[3][2] = {
    {0.05f, 0.05f},
    {0.95f, 0.05f},
    {0.5f, 0.95f}
};
*/


void LetsFractal(int N) {
    SmallTriangles[0][0] = 0.0f;
    SmallTriangles[0][1] = 0.0f;
    for (int i = 1; i < N; i++) {
        int random = rand() % 3;
        SmallTriangles[i][0] = (SmallTriangles[i - 1][0] + BigTriangles[random][0]) / 2;
        SmallTriangles[i][1] = (SmallTriangles[i - 1][1] + BigTriangles[random][1]) / 2;
    }
}

static void display(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Repaint the background with the color
    // specified by glClearColor()
    glClear(GL_COLOR_BUFFER_BIT);

    LetsFractal(N);

    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 3; i++) {
        glColor3f(0, 1, 0); // Set color to green
        glVertex2fv(BigTriangles[i]);
    }
    glEnd();

    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < N; i++) {
        glColor3f(1, 0, 0); // Set color to red
        glVertex2fv(SmallTriangles[i]);
    }
    glEnd();

    glutSwapBuffers();
}

static void initGL(void) {
    // Set the color for painting the background (White)
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Set an orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

static void keyHandle(unsigned char key, int x, int y) {
    // Exit when ESC is pressed
    if (key == 27) exit(0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Setup");

    initGL();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyHandle);
    glutMainLoop();
    return 0;
}

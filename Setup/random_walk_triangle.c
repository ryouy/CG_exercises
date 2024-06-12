#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

const int N = 100000;
float TriangleDots[N+2][2];
/*  for better visualization
float BigTriangles[3][2] = {
    {0.05f, 0.05f},
    {0.95f, 0.05f},
    {0.5f, 0.95f}
};
*/

void LetsFractal(int N) {
    TriangleDots[0][0] = 0.0f;
    TriangleDots[0][1] = 0.0f;
    TriangleDots[1][0] = 1.0f;
    TriangleDots[1][1] = 0.0f;
    TriangleDots[2][0] = 0.5f;
    TriangleDots[2][1] = 1.0f;

    for (int i = 3; i < N+2; i++) {
        int random = rand() % 3;
        TriangleDots[i][0] = (TriangleDots[i - 1][0] + TriangleDots[random][0]) / 2;
        TriangleDots[i][1] = (TriangleDots[i - 1][1] + TriangleDots[random][1]) / 2;
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
        glVertex2fv(TriangleDots[i]);
    }
    glEnd();

    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 3; i < N+2; i++) {
        glColor3f(1, 0, 0); // Set color to red
        glVertex2fv(TriangleDots[i]);
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

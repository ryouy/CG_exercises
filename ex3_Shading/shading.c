#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _PI 3.14159f
static int g_smooth = 0;

// Current material color
static GLfloat g_mat_amb[4];
static GLfloat g_mat_diff[4];
static GLfloat g_mat_spec[4];
static GLfloat g_mat_shiny;

// Light properties
#define MAX_NUM_LIGHTS 100 // Max allowed number of lights
static int g_num_lights = 5; // Current number of lights
static GLfloat g_light_amb[MAX_NUM_LIGHTS][4];
static GLfloat g_light_diff[MAX_NUM_LIGHTS][4];
static GLfloat g_light_spec[MAX_NUM_LIGHTS][4];
static GLfloat g_light_pos[MAX_NUM_LIGHTS][4];

// Use OpenGL shading or our software version
static int g_gl_shading = 1;
static int use_blinn = 0;
static int use_burley_diffuse = 0;
static int light_status[MAX_NUM_LIGHTS] = {1}; // Status of lights (on/off)

// Examples of material colors (sRGB)
static GLfloat SILVER[] = { 0.97f, 0.96f, 0.91f };
static GLfloat ALUMINIUM[] = { 0.91f, 0.92f, 0.92f };
static GLfloat TITANIUM[] = { 0.76f, 0.73f, 0.69f };
static GLfloat IRON[] = { 0.77f, 0.78f, 0.78f };
static GLfloat PLATINUM[] = { 0.83f, 0.81f, 0.78f };
static GLfloat GOLD[] = { 1.00f, 0.85f, 0.57f };
static GLfloat BRASS[] = { 0.98f, 0.90f, 0.59f };
static GLfloat COPPER[] = { 0.97f, 0.74f, 0.62f };

// Clamp the value u to be within the interval [low, high]
//
// Input:
// u is the value to be clamped
// low is the lower bound
// high is the higher bound
//
// Output:
// The function returns u if it is in [low, high],
// low if u<low,
// high if u>high.
//
static GLfloat clamp(GLfloat u, GLfloat low, GLfloat high) {
    if (u < low) return low;
    if (u > high) return high;
    return u;
}

// Matrix-vector product
// out = m * in
//
// Input:
// m is a 4x4 matrix, stored in column-order mode,
// in is a 4x1 vector,
//
// Output:
// out is a 4x1 vector
//
static void matVecProd(GLfloat* m, GLfloat* in, GLfloat* out) {
    out[0] = m[0] * in[0] + m[4] * in[1] + m[8] * in[2] + m[12] * in[3];
    out[1] = m[1] * in[0] + m[5] * in[1] + m[9] * in[2] + m[13] * in[3];
    out[2] = m[2] * in[0] + m[6] * in[1] + m[10] * in[2] + m[14] * in[3];
    out[3] = m[3] * in[0] + m[7] * in[1] + m[11] * in[2] + m[15] * in[3];
}

// Apply the Blinn-Phong illumination model
//
// Input:
// pe is a vertex position (in eye space)
// ne is the surface normal at pe (in eye space)
//
// Output:
// out_color is the intensity at pe
static void BlinnPhongModel(GLfloat* pe, GLfloat* ne, GLfloat* out_color) {
    out_color[0] = 0.f;
    out_color[1] = 0.f;
    out_color[2] = 0.f;

    int i;

    // Complete
    // Implement the Phong model here.
    // Follow the slides of lecture 4 (p. 4, 7, 15-17, 22).
    // Implement Phong (p. 15) and Blinn (p. 17). Allow the user to switch between the two implementations
    // via an additional key (modify the function keyboard().
    // Ignore the attenuation (p. 20).

    out_color[0] = fminf(fmaxf(out_color[0], 0.0f), 1.f);
    out_color[1] = fminf(fmaxf(out_color[1], 0.0f), 1.f);
    out_color[2] = fminf(fmaxf(out_color[2], 0.0f), 1.f);
}

// Define a sphere
//
// Input:
// r is the sphere radius
// nu and nv controls the sphere tessellation
//
static void solidSphere(GLfloat r, int nu, int nv) {
    int i, j;

    // Get the MV matrix
    GLfloat mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);

    for (i = 0; i < nu - 1; ++i) {
        GLfloat u0 = _PI * (GLfloat)i / (nu - 1) - 0.5f*_PI;
        GLfloat u1 = _PI * (GLfloat)(i + 1) / (nu - 1) - 0.5f*_PI;
        for (j = 0; j < nv - 1; ++j) {
            GLfloat v0 = 2.f*_PI*(GLfloat)j / (nv - 1);
            GLfloat v1 = 2.f*_PI*(GLfloat)(j + 1) / (nv - 1);

            GLfloat n00x = cosf(v0)*cosf(u0), n00y = sinf(v0)*cosf(u0), n00z = sinf(u0);
            GLfloat n01x = cosf(v1)*cosf(u0), n01y = sinf(v1)*cosf(u0), n01z = sinf(u0);
            GLfloat n10x = cosf(v0)*cosf(u1), n10y = sinf(v0)*cosf(u1), n10z = sinf(u1);
            GLfloat n11x = cosf(v1)*cosf(u1), n11y = sinf(v1)*cosf(u1), n11z = sinf(u1);

            GLfloat v00x = r * n00x, v00y = r * n00y, v00z = r * n00z;
            GLfloat v01x = r * n01x, v01y = r * n01y, v01z = r * n01z;
            GLfloat v10x = r * n10x, v10y = r * n10y, v10z = r * n10z;
            GLfloat v11x = r * n11x, v11y = r * n11y, v11z = r * n11z;

            if (g_gl_shading) {
                glBegin(GL_QUADS);
                glNormal3f(n00x, n00y, n00z);
                glVertex3f(v00x, v00y, v00z);
                glNormal3f(n01x, n01y, n01z);
                glVertex3f(v01x, v01y, v01z);
                glNormal3f(n11x, n11y, n11z);
                glVertex3f(v11x, v11y, v11z);
                glNormal3f(n10x, n10y, n10z);
                glVertex3f(v10x, v10y, v10z);
                glEnd();
            }
            else {
                GLfloat p00[] = { v00x,v00y,v00z,1.f };
                GLfloat n00[] = { n00x,n00y,n00z,0.f };
                GLfloat pe[4]; // in eye-space
                matVecProd(mv, p00, pe);
                GLfloat ne[4]; // in eye-space
                matVecProd(mv, n00, ne); // no scale so we can cheat
                GLfloat c00[3];
                BlinnPhongModel(pe, ne, c00);

                GLfloat p01[] = { v01x,v01y,v01z,1.f };
                GLfloat n01[] = { n01x,n01y,n01z,0.f };
                matVecProd(mv, p01, pe);
                matVecProd(mv, n01, ne);
                GLfloat c01[3];
                BlinnPhongModel(pe, ne, c01);

                GLfloat p10[] = { v10x,v10y,v10z,1.f };
                GLfloat n10[] = { n10x,n10y,n10z,0.f };
                matVecProd(mv, p10, pe);
                matVecProd(mv, n10, ne);
                GLfloat c10[3];
                BlinnPhongModel(pe, ne, c10);

                GLfloat p11[] = { v11x,v11y,v11z,1.f };
                GLfloat n11[] = { n11x,n11y,n11z,0.f };
                matVecProd(mv, p11, pe);
                matVecProd(mv, n11, ne);
                GLfloat c11[3];
                BlinnPhongModel(pe, ne, c11);

                glBegin(GL_QUADS);
                // Complete
                // Specify each vertex: v00, v01, v11, v10 as above
                // as well as its color c00, c01, c11, c10
                //
                //
                glEnd();
            }
        }
    }
}

static void drawSphere1() {
    GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 1.f };
    GLfloat diff[] = { 0.7f, 0.7f, 0.7f, 1.f };
    GLfloat spec[] = { 0.9f, 0.9f, 0.9f, 1.f };
    GLfloat shiny = 64.f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, shiny);

    g_mat_amb[0] = amb[0];
    g_mat_amb[1] = amb[1];
    g_mat_amb[2] = amb[2];
    g_mat_diff[0] = diff[0];
    g_mat_diff[1] = diff[1];
    g_mat_diff[2] = diff[2];
    g_mat_spec[0] = spec[0];
    g_mat_spec[1] = spec[1];
    g_mat_spec[2] = spec[2];
    g_mat_shiny = shiny;

    solidSphere(1.0, 64, 64);
}

static void drawSphere2() {
    GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 1.f };
    GLfloat diff[] = { 0.5f, 0.25f, 0.1f, 1.f };
    GLfloat spec[] = { 0.9f, 0.9f, 0.9f, 1.f };
    GLfloat shiny = 64.f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, shiny);

    g_mat_amb[0] = amb[0];
    g_mat_amb[1] = amb[1];
    g_mat_amb[2] = amb[2];
    g_mat_diff[0] = diff[0];
    g_mat_diff[1] = diff[1];
    g_mat_diff[2] = diff[2];
    g_mat_spec[0] = spec[0];
    g_mat_spec[1] = spec[1];
    g_mat_spec[2] = spec[2];
    g_mat_shiny = shiny;

    solidSphere(1.0, 64, 64);
}

static void drawFloor() {
    if (g_gl_shading) glDisable(GL_LIGHTING);
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex3f(10.f, -1.5f, 10.f);
    glVertex3f(10.f, -1.5f, -10.f);
    glVertex3f(-10.f, -1.5f, -10.f);
    glVertex3f(-10.f, -1.5f, 10.f);
    glEnd();
    if (g_gl_shading) glEnable(GL_LIGHTING);
}

static void setLight() {
    GLfloat amb0[] = { 1.f, 1.f, 1.f, 1.f };
    GLfloat diff0[] = { 1.f, 1.f, 1.f, 1.f };
    GLfloat spec0[] = { 1.f, 1.f, 1.f, 1.f };
    GLfloat pos0[] = { 0.f, 0.f, 0.f, 1.f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos0);
    glEnable(GL_LIGHT0);

    g_num_lights = 1;
    g_light_amb[0][0] = amb0[0];
    g_light_amb[0][1] = amb0[1];
    g_light_amb[0][2] = amb0[2];

    g_light_diff[0][0] = diff0[0];
    g_light_diff[0][1] = diff0[1];
    g_light_diff[0][2] = diff0[2];

    g_light_spec[0][0] = spec0[0];
    g_light_spec[0][1] = spec0[1];
    g_light_spec[0][2] = spec0[2];

    g_light_pos[0][0] = pos0[0];
    g_light_pos[0][1] = pos0[1];
    g_light_pos[0][2] = pos0[2];
    g_light_pos[0][3] = pos0[3];
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_smooth)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);

    glLoadIdentity();
    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glPushMatrix();

    drawFloor();

    // sphere 1
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, 0.0f);
    drawSphere1();
    glPopMatrix();

    // sphere 2
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, 0.0f);
    drawSphere2();
    glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
}

static void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (double)width / (double)height, 0.1, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;

    case 's':
        g_smooth = 1 - g_smooth;
        break;

    case 'g':
        g_gl_shading = 1 - g_gl_shading;
        if (g_gl_shading)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
        break;

    case 'b':
        use_blinn = 1 - use_blinn;
        break;

    case 'd':
        use_burley_diffuse = 1 - use_burley_diffuse;
        break;

    case 'l':
        // Add new light
        if (g_num_lights < MAX_NUM_LIGHTS) {
            g_num_lights++;
            GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 1.f };
            GLfloat diff[] = { 0.8f, 0.8f, 0.8f, 1.f };
            GLfloat spec[] = { 1.f, 1.f, 1.f, 1.f };
            GLfloat pos[] = { 5.0f, 5.0f, 5.0f, 1.f };

            g_light_amb[g_num_lights - 1][0] = amb[0];
            g_light_amb[g_num_lights - 1][1] = amb[1];
            g_light_amb[g_num_lights - 1][2] = amb[2];

            g_light_diff[g_num_lights - 1][0] = diff[0];
            g_light_diff[g_num_lights - 1][1] = diff[1];
            g_light_diff[g_num_lights - 1][2] = diff[2];

            g_light_spec[g_num_lights - 1][0] = spec[0];
            g_light_spec[g_num_lights - 1][1] = spec[1];
            g_light_spec[g_num_lights - 1][2] = spec[2];

            g_light_pos[g_num_lights - 1][0] = pos[0];
            g_light_pos[g_num_lights - 1][1] = pos[1];
            g_light_pos[g_num_lights - 1][2] = pos[2];
            g_light_pos[g_num_lights - 1][3] = pos[3];
        }
        break;

    case 'o':
        // Toggle lights on/off
        for (int i = 0; i < g_num_lights; i++) {
            light_status[i] = 1 - light_status[i];
            if (light_status[i])
                glEnable(GL_LIGHT0 + i);
            else
                glDisable(GL_LIGHT0 + i);
        }
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

static void init(void) {
    GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    setLight();

    glEnable(GL_LIGHTING);
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Shading");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}

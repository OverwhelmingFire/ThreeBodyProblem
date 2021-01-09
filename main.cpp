#ifdef FOR_WINDOWS
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl.h>
#include <glu.h>
#include <GL/freeglut.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <time.h>
#include "mvector.h"
#endif

#ifndef FOR_WINDOWS
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <time.h>
#include "mvector.h"
#endif

GLdouble rotationMatrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
double timeDelta = 0.00001;
double  indent = -10;
double scale = 1;

int animationSpeed = 1;
int windowWidth = 400;
int windowHeight = 400;

int currentPointX = 0;
int previousPointX = 0;
int currentPointY = 0;
int previousPointY = 0;

int counter = 0;
int iter = 0;

double G = 6.67428e-2;

mvector rA(0.1, -0.1, 0.0);
mvector rB(1.2, -2.2, 0.0);
mvector rC(1.0, -2.5, -1.0);
mvector vA(0.02, -0.0, 0.0);
mvector vB(0.0, 0.02, 0.0);
mvector vC(-0.01, -0.0, 0.02);

double traceA[2048][3];
double traceB[2048][3];
double traceC[2048][3];

mvector * stars[2048];
mvector * colors[2048];

void multMatrices(GLdouble m1[16], GLdouble m2[16]) {
    GLdouble temp[] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    for (int i = 0; i < 16; i++) {
        for (int k = 0; k < 4; k++) {
            temp[i] += m1[i/4*4+k]*m2[i%4+k*4];
        }
    }
    for (int i = 0; i < 16; i++) {
        m1[i] = temp[i];
    }
}

void physics() {
    mvector aA = (rC-rA)*G/(pow((rC-rA).abs(),3)) + (rB-rA)*G/(pow((rB-rA).abs(), 3));
    mvector aB = (rC-rB)*G/(pow((rC-rB).abs(),3)) + (rA-rB)*G/(pow((rA-rB).abs(), 3));
    mvector aC = (rA-rC)*G/(pow((rA-rC).abs(),3)) + (rB-rC)*G/(pow((rB-rC).abs(), 3));

    vA = vA + aA*timeDelta;
    vB = vB + aB*timeDelta;
    vC = vC + aC*timeDelta;

    rA = rA + vA*timeDelta;
    rB = rB + vB*timeDelta;
    rC = rC + vC*timeDelta;
}

void updateTrace() {
    traceA[iter][0] = rA.x;
    traceA[iter][1] = rA.y;
    traceA[iter][2] = rA.z;

    traceB[iter][0] = rB.x;
    traceB[iter][1] = rB.y;
    traceB[iter][2] = rB.z;

    traceC[iter][0] = rC.x;
    traceC[iter][1] = rC.y;
    traceC[iter][2] = rC.z;

    counter += 1;
    if (counter > 1.0/timeDelta/100) {
        counter = 0;
        iter += 1;
    }
    if (iter >= 2048) {
        iter = 0;
    }
}

void drawStars() {
    for (int i = 0; i < 2048; i++) {
        glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
        glColor3f(colors[i]->x, colors[i]->y, colors[i]->z);
        glVertex3d(stars[i]->x, stars[i]->y, stars[i]->z);
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

void drawAxes() {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3d(1.0,0.0,0.0);
    glVertex3d(0.0,0.0,0.0);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1,0,0);
    glVertex3d(1.0,0.0,0.0);
    for (int i = 0; i <= 30; i++) {
        glVertex3d(1-0.05, sin(-i/30.0*2*3.1415)*0.025, cos(-i/30.0*2*3.1415)*0.025);
    }
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0,1,0);
    glVertex3d(0.0,1.0,0.0);
    glVertex3d(0.0,0.0,0.0);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0,1,0);
    glVertex3d(0.0,1.0,0.0);
    for (int i = 0; i <= 30; i++) {
        glVertex3d(sin(-i/30.0*2*3.1415)*0.025, 1-0.05, cos(-i/30.0*2*3.1415)*0.025);
    }
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0,0,1);
    glVertex3d(0.0,0.0,1.0);
    glVertex3d(0.0,0.0,0.0);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0,0,1);
    glVertex3d(0.0,0.0,1.0);
    for (int i = 0; i <= 30; i++) {
        glVertex3d(sin(-i/30.0*2*3.1415)*0.025, cos(-i/30.0*2*3.1415)*0.025, 1-0.05);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawTrace() {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
    glColor3f(0,1,0);
    for (int i = iter-1; i >= 0; i--){
        glVertex3d(traceA[i][0],traceA[i][1], traceA[i][2]);
    }
    for (int i = 2048-1; i > iter; i--){
        glVertex3d(traceA[i][0],traceA[i][1], traceA[i][2]);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(1,1,0);
    for (int i = iter-1; i >= 0; i--){
        glVertex3d(traceB[i][0],traceB[i][1], traceB[i][2]);
    }
    for (int i = 2048-1; i > iter; i--){
        glVertex3d(traceB[i][0],traceB[i][1], traceB[i][2]);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(1,0,1);
    for (int i = iter-1; i >= 0; i--){
        glVertex3d(traceC[i][0],traceC[i][1], traceC[i][2]);
    }
    for (int i = 2048-1; i > iter; i--){
        glVertex3d(traceC[i][0],traceC[i][1], traceC[i][2]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();              
    glTranslatef(0, 0,  indent);  
    glScalef(scale,scale,scale);
	glMultMatrixd(rotationMatrix);
    glTranslatef(rA.x, rA.y, rA.z);
    glColor3f(0,1,0);
	glutSolidSphere(0.05, 50, 50);

    glLoadIdentity();              
    glTranslatef(0, 0,  indent);  
    glScalef(scale,scale,scale);
	glMultMatrixd(rotationMatrix);
    glTranslatef(rB.x, rB.y, rB.z);  
    glColor3f(1,1,0);
	glutSolidSphere(0.05, 50, 50);

    glLoadIdentity();              
    glTranslatef(0, 0,  indent);  
    glScalef(scale,scale,scale);
	glMultMatrixd(rotationMatrix);
    glTranslatef(rC.x, rC.y, rC.z);  
    glColor3f(1,0,1);
	glutSolidSphere(0.05, 50, 50);

	glLoadIdentity();              
    glTranslatef(0, 0,  indent);  
    glScalef(scale,scale,scale);
	glMultMatrixd(rotationMatrix);
    drawAxes();
    drawTrace();
    drawStars();

    glFlush();
    glutSwapBuffers();
}

void update(int value)
{
        for (int i = 0; i < animationSpeed; i++) {
            physics();
            updateTrace();
        }
        glutPostRedisplay();
        glutTimerFunc(0.1, update, 0);
}

void initRendering()
{
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_BLEND );

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);

        srand(time(0));

        rA.x = (rand()%10000-5000)/10000.0;
        rA.y = (rand()%10000-5000)/10000.0;
        rA.z = (rand()%10000-5000)/10000.0;
        rB.x = (rand()%10000-5000)/10000.0;
        rB.y = (rand()%10000-5000)/10000.0;
        rB.z = (rand()%10000-5000)/10000.0;
        rC.x = (rand()%10000-5000)/10000.0;
        rC.y = (rand()%10000-5000)/10000.0;
        rC.z = (rand()%10000-5000)/10000.0;

        vA.x = (rand()%10000-5000)/20000.0;
        vA.y = (rand()%10000-5000)/20000.0;
        vA.z = (rand()%10000-5000)/20000.0;
        vB.x = (rand()%10000-5000)/20000.0;
        vB.y = (rand()%10000-5000)/20000.0;
        vB.z = (rand()%10000-5000)/20000.0;
        vC.x = (rand()%10000-5000)/20000.0;
        vC.y = (rand()%10000-5000)/20000.0;
        vC.z = (rand()%10000-5000)/20000.0;

        for (int i = 0; i < 2048; i++) {
            double x = (rand() % 500 - 250)/250.0;
            double y = (rand() % 500 - 250)/250.0;
            double z = (rand() % 500 - 250)/250.0;
            stars[i] = new mvector(x, y, z);
            stars[i]->normalize();
            //stars[i]->out();
            (*stars[i]) = (*stars[i])*15;

            double r = (rand() % 90)/256.0 + 166/256.0;
            double g = (rand() % 90)/256.0 + 166/256.0;
            double b = (rand() % 90)/256.0 + 166/256.0;
            colors[i] = new mvector(r, g, b);
            //printf("x %f y %f\n", x, y);
        }

        for (int i = 0; i < 2048; i++) {
            traceA[i][0] = rA.x;
            traceA[i][1] = rA.y;
            traceA[i][2] = rA.z;
            traceB[i][0] = rB.x;
            traceB[i][1] = rB.y;
            traceB[i][2] = rB.z;
            traceC[i][0] = rC.x;
            traceC[i][1] = rC.y;
            traceC[i][2] = rC.z;
        }
}

void handleResize(int width, int height)
{
        windowHeight = height;
        windowWidth = width;
        if (height == 0) height = 1;                // To prevent divide by 0
        GLfloat aspect = (GLfloat)width / (GLfloat)height;

        // Set the viewport to cover the new window
        glViewport(0, 0, width, height);

        // Set the aspect ratio of the clipping volume to match the viewport
        glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
        glLoadIdentity();             // Reset
        // Enable perspective projection with fovy, aspect, zNear and zFar
        gluPerspective(60.0f, aspect, 0.1f, 100.0f);
}

void mouse(int button, int state, int x, int y)
{
    if (button == 3) {
        scale *= 1.1;
    }
    else if (button == 4) {
        scale /= 1.1;
    }
    else {
        previousPointX = currentPointX;
        previousPointY = currentPointY;
        currentPointX = x;
        currentPointY = y;
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') {
        animationSpeed += 1;
    }
    if (key == 's') {
        animationSpeed -= 1;
    }
}


void special(int key, int x, int y) {
}


void mouseMoving(int x, int y) {
    mvector rotationAxis(0.0, 0.0, 1.0);

    previousPointX = currentPointX;
    previousPointY = currentPointY;
    currentPointX = x;
    currentPointY = y;

    printf("Poscontrol x\t%f\ty\t%f\n", currentPointX-previousPointX, currentPointY-previousPointY);
    if (currentPointX-previousPointX == 0 && currentPointY-previousPointY==0) {
        return;
    }
    double mult = -1;
    mvector v((currentPointX-previousPointX)*mult, (currentPointY-previousPointY)*mult, 0.0);
    mvector rad(0, 0, 1);
    rotationAxis = rad*v;
    rotationAxis.normalize();
    double angle = -(v.abs())/(10*2*3.1415);


    rotationAxis.out();
    printf("Angle : %f\n", angle);
    GLdouble temp[] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    temp[0] = (rotationAxis.x*rotationAxis.x)*(1-cos(angle))+cos(angle);
    temp[1] = (rotationAxis.x*rotationAxis.y)*(1-cos(angle))-rotationAxis.z*sin(angle);
    temp[2] = (rotationAxis.x*rotationAxis.z)*(1-cos(angle))+rotationAxis.y*sin(angle);

    temp[4] = (rotationAxis.x*rotationAxis.y)*(1-cos(angle))+rotationAxis.z*sin(angle);
    temp[5] = (rotationAxis.y*rotationAxis.y)*(1-cos(angle))+cos(angle);
    temp[6] = (rotationAxis.y*rotationAxis.z)*(1-cos(angle))-rotationAxis.x*sin(angle);

    temp[8] = (rotationAxis.x*rotationAxis.z)*(1-cos(angle))-rotationAxis.y*sin(angle);
    temp[9] = (rotationAxis.y*rotationAxis.z)*(1-cos(angle))+rotationAxis.x*sin(angle);
    temp[10] = (rotationAxis.z*rotationAxis.z)*(1-cos(angle))+cos(angle);

    for (int i = 0; i < 16; i+=4) {
        for (int k = 0; k < 4; k++) {
            printf("%f ", temp[i+k]);
        }
        printf("\n");
    }
    printf("\n");

    multMatrices(rotationMatrix, temp);

    for (int i = 0; i < 16; i+=4) {
        for (int k = 0; k < 4; k++) {
            printf("%f ", rotationMatrix[i+k]);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Three Body Problem");
    initRendering();

    glutDisplayFunc(draw);
    glutReshapeFunc(handleResize);
    glutSpecialFunc(special);
    glutTimerFunc(25, update, 0);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMoving);

    glutMainLoop();
    return 0;
}
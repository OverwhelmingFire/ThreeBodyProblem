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
#endif

double change = -10;
double scale = 1;
int windowWidth = 400;
int windowHeight = 400;

double currentPointX = 0;
double previousPointX = 0;
double currentPointY = 0;
double previousPointY = 0;

int counter = 0;

int speed = 1;
double G = 6.67428e-2;

class mvector {
    public:
        double x, y, z;

        mvector(double _x, double _y, double _z) {
            x = _x;
            y = _y;
            z = _z;
        }

        mvector operator +(const mvector& a) {
            mvector result(a.x+this->x, a.y+this->y, a.z+this->z);
            return result;
        }

        mvector operator -(const mvector& a) {
            mvector result(this->x-a.x, this->y-a.y, this->z-a.z);
            return result;
        }

        mvector operator *(const double& d) {
            mvector result(this->x*d, this->y*d, this->z*d);
            return result;
        }

        mvector operator /(const double& d) {
            mvector result(this->x/d, this->y/d, this->z/d);
            return result;
        }

        mvector operator *(const mvector& a) {
            double x = this->y*a.z - this->z*a.y;
            double y = this->x*a.z - this->z*a.x;
            double z = this->x*a.y - this->y*a.x;
            mvector result(x, y, z);
            return result;
        }

        mvector operator ,(const mvector& a) {
            mvector result(this->x*a.x, this->y*a.y, this->z*a.z);
            return result;
        }

        double abs() {
            return sqrt(x*x + y*y + z*z);
        }

        void normalize() {
            double a = 1/abs();
            //printf("asd %f\n", a);
            this->x *= a;
            this->y *= a;
            this->z *= a;
        }

        void out() {
            printf("x\t%f\ty\t%f\tz\t%f\n", this->x, this->y, this->z);
        }
};

void multMatrices(GLdouble m1[16], GLdouble m2[16]) {
    GLdouble temp[] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    for (int i = 0; i < 16; i++) {
        for (int k = 0; k < 4; k++) {
            temp[i] += m1[i/4*4+k]*m2[i%4+k*4];
        }
    }
    for (int i = 0; i < 16; i++) {
        m1[i] = temp[i];
        printf("%f ", temp[i]);
    }
    printf("\n");
}

mvector axis(0.0, 0.0, 0.0);
double angle = 0;
double timeSpan = 0.00001;
GLdouble rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

mvector rA(0.1, -0.1, 0.0);
mvector rB(1.2, -2.2, 0.0);
mvector rC(1.0, -2.5, -1.0);
mvector vA(0.02, -0.0, 0.0);
mvector vB(0.0, 0.02, 0.0);
mvector vC(-0.01, -0.0, 0.02);


double trackA[2048][3];
double trackB[2048][3];
double trackC[2048][3];
int iter = 0;

mvector * stars[2048];
mvector * colors[2048];

void physics() {
    //printf("A\n");
    mvector aA = (rC-rA)*G/(pow((rC-rA).abs(),3)) + (rB-rA)*G/(pow((rB-rA).abs(), 3));
    //aA.out();
    mvector aB = (rC-rB)*G/(pow((rC-rB).abs(),3)) + (rA-rB)*G/(pow((rA-rB).abs(), 3));
    //aB.out();
    mvector aC = (rA-rC)*G/(pow((rA-rC).abs(),3)) + (rB-rC)*G/(pow((rB-rC).abs(), 3));
    //aC.out();

    //printf("V\n");
    vA = vA + aA*timeSpan;
    //vA.out();
    vB = vB + aB*timeSpan;
    //vB.out();
    vC = vC + aC*timeSpan;
    //vC.out();

    //printf("R\n");
    rA = rA + vA*timeSpan;
    //rA.out();
    rB = rB + vB*timeSpan;
    //rB.out();
    rC = rC + vC*timeSpan;
    //rC.out();

    trackA[iter][0] = rA.x;
    trackA[iter][1] = rA.y;
    trackA[iter][2] = rA.z;

    trackB[iter][0] = rB.x;
    trackB[iter][1] = rB.y;
    trackB[iter][2] = rB.z;

    trackC[iter][0] = rC.x;
    trackC[iter][1] = rC.y;
    trackC[iter][2] = rC.z;

    counter += 1;
    if (counter > 1.0/timeSpan/100) {
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

void drawTrack() {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
    glColor3f(0,1,0);
    for (int i = iter-1; i >= 0; i--){
        glVertex3d(trackA[i][0],trackA[i][1], trackA[i][2]);
    }
    for (int i = 2048-1; i > iter; i--){
        glVertex3d(trackA[i][0],trackA[i][1], trackA[i][2]);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(1,1,0);
    for (int i = iter-1; i >= 0; i--){
        glVertex3d(trackB[i][0],trackB[i][1], trackB[i][2]);
    }
    for (int i = 2048-1; i > iter; i--){
        glVertex3d(trackB[i][0],trackB[i][1], trackB[i][2]);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(1,0,1);
    for (int i = iter-1; i >= 0; i--){
        glVertex3d(trackC[i][0],trackC[i][1], trackC[i][2]);
    }
    for (int i = 2048-1; i > iter; i--){
        glVertex3d(trackC[i][0],trackC[i][1], trackC[i][2]);
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
    glTranslatef(0, 0, change);  
    glScalef(scale,scale,scale);
	//glRotatef(angle, axis.x, axis.y, axis.z);
	glMultMatrixd(rotate);
    glTranslatef(rA.x, rA.y, rA.z);
    glColor3f(0,1,0);
	glutSolidSphere(0.05, 50, 50);

    glLoadIdentity();              
    glTranslatef(0, 0, change);  
    glScalef(scale,scale,scale);
	//glRotatef(angle, axis.x, axis.y, axis.z);
	glMultMatrixd(rotate);
    glTranslatef(rB.x, rB.y, rB.z);  
    glColor3f(1,1,0);
	glutSolidSphere(0.05, 50, 50);

    glLoadIdentity();              
    glTranslatef(0, 0, change);  
    glScalef(scale,scale,scale);
	//glRotatef(angle, axis.x, axis.y, axis.z);
	glMultMatrixd(rotate);
    glTranslatef(rC.x, rC.y, rC.z);  
    glColor3f(1,0,1);
	glutSolidSphere(0.05, 50, 50);

	glLoadIdentity();              
    glTranslatef(0, 0, change);  
    glScalef(scale,scale,scale);
	//glRotatef(angle, axis.x, axis.y, axis.z);
	glMultMatrixd(rotate);
    drawAxes();
    drawTrack();

    glLoadIdentity();
    glTranslatef(0, 0, change);  
    glScalef(scale,scale,scale);
	//glRotatef(angle, axis.x, axis.y, axis.z);
	glMultMatrixd(rotate);
    drawStars();
    glFlush();
    glutSwapBuffers();
}

void update(int value)
{
        for (int i = 0; i < speed; i++) {
            physics();
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
            trackA[i][0] = rA.x;
            trackA[i][1] = rA.y;
            trackA[i][2] = rA.z;
            trackB[i][0] = rB.x;
            trackB[i][1] = rB.y;
            trackB[i][2] = rB.z;
            trackC[i][0] = rC.x;
            trackC[i][1] = rC.y;
            trackC[i][2] = rC.z;
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

/*

Now we create event handlers, for example, to handle mouse clicks, interaction with keyboard etc.

*/

void mouse(int button, int state, int x, int y)
{
    if (button == 3) {
        scale *= 1.2;
    }
    else if (button == 4) {
        scale /= 1.2;
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
        speed += 1;
    }
    if (key == 's') {
        speed -= 1;
    }
}


void special(int key, int x, int y) {
}


void mouseMoving(int x, int y) {
    axis.x = 0.0;
    axis.y = 0.0;
    axis.z = 1.0;

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
    axis = rad*v;
    axis.normalize();
    angle = -(v.abs())/(10*2*3.1415);


    axis.out();
    printf("Angle : %f\n", angle);
    GLdouble temp[] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    temp[0] = (axis.x*axis.x)*(1-cos(angle))+cos(angle);
    temp[1] = (axis.x*axis.y)*(1-cos(angle))-axis.z*sin(angle);
    temp[2] = (axis.x*axis.z)*(1-cos(angle))+axis.y*sin(angle);

    temp[4] = (axis.x*axis.y)*(1-cos(angle))+axis.z*sin(angle);
    temp[5] = (axis.y*axis.y)*(1-cos(angle))+cos(angle);
    temp[6] = (axis.y*axis.z)*(1-cos(angle))-axis.x*sin(angle);

    temp[8] = (axis.x*axis.z)*(1-cos(angle))-axis.y*sin(angle);
    temp[9] = (axis.y*axis.z)*(1-cos(angle))+axis.x*sin(angle);
    temp[10] = (axis.z*axis.z)*(1-cos(angle))+cos(angle);

    for (int i = 0; i < 16; i+=4) {
        for (int k = 0; k < 4; k++) {
            printf("%f ", temp[i+k]);
        }
        printf("\n");
    }
    printf("\n");

    multMatrices(rotate, temp);

    for (int i = 0; i < 16; i+=4) {
        for (int k = 0; k < 4; k++) {
            printf("%f ", rotate[i+k]);
            //rotate[i+k] = temp[i + k];
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
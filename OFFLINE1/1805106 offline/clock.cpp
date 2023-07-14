#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cmath>


const int delayClockUpdate = 20;
const int modH = 12 * 60 * 60 * 1000;
const int modM = 60 * 60 * 1000;
const int modS = 60 * 1000;
const int modMS = 1000;
int curMS;

const GLdouble scale = 0.6;
const GLdouble clockRadius = 0.6;
const GLdouble pendulumRadius = 0.12;
const GLdouble miniClockRadius = 0.15;
const GLdouble hourLength = clockRadius - 0.3;
const GLdouble minLength = clockRadius - 0.2;
const GLdouble secLength = clockRadius - 0.1;
const GLdouble milliLength = miniClockRadius - 0.03;
const GLdouble milliX = (clockRadius - 0.1) / 2;
const GLdouble milliY = -0.2;
const GLdouble pendulumLength = clockRadius * 2; //M_PI * M_PI * 9.80665;
const GLdouble angularFreq = M_PI;
const GLdouble maxAngle = 30;
const GLdouble width = 0.08;
const GLdouble clockSide = clockRadius + 0.2 + width / 2;
const GLdouble clockUp = clockRadius + 0.2 + width / 2;
const GLdouble clockDown = pendulumLength - 0.08 + width / 2;
const GLdouble clockDown2 = pendulumLength + pendulumRadius + 0.3 + width / 2;

const int clockSlices = 720;


void initClock(){
    std::time_t t = std::time(NULL);
    std::tm* now = std::localtime(&t);
    curMS = now -> tm_hour % 12 * modM;
    curMS += now -> tm_min * modS;
    curMS += now -> tm_sec * modMS;
}

void drawCircle(double radius, int segments) {
    glBegin(GL_POLYGON);{
        for (int i = 0; i < segments; i++) {
            double theta = i * 2.0 * M_PI / segments;
            glVertex2d(radius * cos(theta), radius * sin(theta));
        }
    }glEnd();
}
void drawLine(double a, double b){
    glBegin(GL_LINES);{
        glVertex2d(0, a);
        glVertex2d(0, b);
    }glEnd();
}

void drawClockBackGround(){
    glPushMatrix();
    glColor3d(0, 0.1, 0.2);                         //Border Color
    drawCircle(clockRadius + 0.02, clockSlices);
    glColor3d(0.2, 0.2, 0.2);                       //Clock Background Color
    
    drawCircle(clockRadius, clockSlices);

    glTranslated(milliX, milliY, 0);
    glColor3d(0, 0.1, 0.2);                         //Border Color
    drawCircle(miniClockRadius + 0.01, clockSlices);
    glColor3d(0.17, 0.17, 0.17);                       //Clock Background Color
    drawCircle(miniClockRadius, clockSlices);
    
    glColor3d(0.8, 0.8, 0.8);                       //Mark Color
    glLineWidth(0.5);
    for(int i = 0; i < 10; i++){
        drawLine(miniClockRadius - 0.02, miniClockRadius);
        glRotated(36, 0, 0, 1);
    }
    glTranslated(-milliX, -milliY, 0);
    glLineWidth(2);
    for(int i = 0; i < 60; i++){
        drawLine(clockRadius - 0.02, clockRadius);
        glRotated(6, 0, 0, 1);
    }

    for(int i = 0; i < 12; i++){
        drawLine(clockRadius - 0.05, clockRadius);
        glRotated(30, 0, 0, 1);
    }
    for(int i = 0; i < 4; i++){
        drawLine(clockRadius - 0.1, clockRadius);
        glRotated(90, 0, 0, 1);
    }
    glPopMatrix();
}

void drawHandQuad(GLdouble length, GLdouble hieght = 0.02){
    GLdouble len = length * 4 / 5;
    glColor3d(0.75, 0.75, 0.75);
    glBegin(GL_QUADS);{
        glVertex2d(0, 0);
        glVertex2d(hieght, len);
        glVertex2d(0, length);
        glVertex2d(-hieght, len);
    }glEnd();
}

void drawHour(){
    glPushMatrix();
    GLdouble angle = curMS * 30.0 / modM;
    glRotated(360 - angle,0, 0, 1);
    drawHandQuad(hourLength);
    glColor3d(0.8, 0.8, 0.8);
    glLineWidth(4);
    drawLine(0, hourLength);
    glLineWidth(2);
    glPopMatrix();
}
void drawMinute(){
    glPushMatrix();
    GLdouble angle = curMS % modM * 6.0 / modS;
    glRotated(360 - angle,0, 0, 1);
    glScaled(0.9, 1, 1);
    drawHandQuad(minLength);
    glScaled(1/0.9, 1, 1);
    glColor3d(0.8, 0.8, 0.8);
    glLineWidth(2);
    drawLine(0, minLength);

    glLineWidth(2);
    glPopMatrix();
}
void drawSecond(){
    glPushMatrix();
    // GLdouble angle = (curMS % modS * 6.0 / modMS);
    GLdouble angle = (curMS % modS / modMS * 6.0);
    glRotated(360 - angle,0, 0, 1);
    glScaled(0.8, 1, 1);
    drawHandQuad(secLength);
    glScaled(1/0.8, 1, 1);
    glColor3d(0.8, 0.8, 0.8);
    glLineWidth(1);
    drawLine(0, secLength);
    glLineWidth(4);
    glPopMatrix();
}
void drawMilliSecond(){
    glPushMatrix();
    glTranslated(milliX, milliY, 0);
    GLdouble angle = curMS % modMS * 0.36;
    glRotated(360 - angle,0, 0, 1);
    drawHandQuad(milliLength, 0.01);
    glColor3d(0.8, 0.8, 0.8);
    drawLine(0, milliLength);
    
    glPopMatrix();
}

void drawPendulum(){
    glPushMatrix();
    glColor3d(0.23, 0.25, 0.24);
    GLdouble angle = maxAngle * cos(curMS % modM * angularFreq / modMS + 0);
    glRotated(angle, 0, 0, 1);
    glLineWidth(10);
    drawLine(0, -pendulumLength);
    glTranslated(0, -pendulumLength, 0);
    drawCircle(pendulumRadius, clockSlices);
    // glColor3d(0, 0, 0);
    // drawCircle(pendulumRadius * 0.8, clockSlices);
    glPopMatrix();
}
void drawCenter(){
    glColor3d(0, 0.1, 0.2);
    drawCircle(0.05, 6);
}
void drawClockForeGround(){
    glColor3d(0.33, 0.35, 0.34);
    glBegin(GL_POLYGON);{
        glVertex2d(-clockSide, clockUp);
        glVertex2d(clockSide, clockUp);
        glVertex2d(clockSide, -clockDown);
        glVertex2d(0, -clockDown2);
        glVertex2d(-clockSide, -clockDown);
    }glEnd();
    glColor3d(0, 0, 0);
    glBegin(GL_POLYGON);{
        glVertex2d(-clockSide + width, clockUp - width);
        glVertex2d(clockSide  - width, clockUp - width);
        glVertex2d(clockSide - width, -clockDown + width);
        glVertex2d(0, -clockDown2 + width);
        glVertex2d(-clockSide + width, -clockDown + width);
    }glEnd();

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);             
    glLoadIdentity(); 
    glScaled(scale, scale, scale);
    glTranslated(0, 0.4, 0);
    glLineWidth(2);

    drawClockForeGround();
    drawPendulum();
    drawClockBackGround();
    drawMilliSecond();
    drawHour();
    drawMinute();
    drawSecond();
    
    drawCenter();
    
    glutSwapBuffers();
}


void updateClock(int value){
    glutTimerFunc(delayClockUpdate, updateClock, 0);

    //change time
    curMS += delayClockUpdate;
    if(curMS >= modH)curMS -= modH;

    glutPostRedisplay();
    
}
void reshape(GLsizei width, GLsizei height) { 
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);                  // Initialize GLUT
    glutInitWindowSize(640, 640);           // Set the window's initial width & height
    glutInitWindowPosition(100, 50);        // Position the window's initial top-left corner
    glutCreateWindow("CLOCK");              // Create a window with the given title
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    initClock();
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);               // Register display callback handler for window re-paint
    glutTimerFunc(delayClockUpdate, updateClock, 0);

    glutMainLoop();                         // Enter the infinitely event-processing loop
    return 0;
}
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include<bits/stdc++.h>
using namespace std;

struct point {
    GLdouble x, y, z;
};
point cross(point a, point b) {
  return {a.y*b.z - a.z*b.y,
     a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; }


GLdouble dot(point a, point b) {
  return a.x * b.x + a.y * b.y + a.z * b.z; }
GLdouble length(point a) { return sqrt(dot(a, a));}

// Global variables
struct point pos;   // position of the eye
struct point l;     // look/forward direction
struct point r;     // right direction
struct point u;     // up direction

GLdouble angleSide = 0;
GLdouble changeRatio = 0;  
/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling

    pos.x=4;pos.y=3.5;pos.z=4;

    l.x=-0.1;l.y=-0.08;l.z=-0.1;
    u.x=0;u.y=0.1;u.z=0;
    r.x=0.1;r.y=0;r.z=0;

}


//constants
const GLdouble  sphereRadius = 1;
const GLdouble diHedralAngle = 109.47;
const GLdouble triangleCenterAngle = 90 - diHedralAngle / 2;
const GLdouble triangleSideLength = 2 * sphereRadius / cos(M_PI / 180 * triangleCenterAngle);
const GLdouble triangleRadius = triangleSideLength / sqrt(3);
const GLdouble cos60 = cos(M_PI / 3);
const GLdouble sin60 = sin(M_PI / 3);
const GLdouble cylinderAngle = 180 - diHedralAngle;
const GLdouble cylinderRotateAngle = 45;
const GLdouble octaHedronHeight = triangleSideLength / 2 * tan(M_PI / 180 * diHedralAngle / 2);


void drawSphere(GLdouble angle = 90, int stacks = 360, int slices = 360) {
    struct point points[stacks+1][slices+1];
    angle = angle * M_PI / 180;
    GLdouble half = angle / 2;
    for (int j = 0; j <= stacks; j++) {
        double phi = -half + (j % stacks) * angle / stacks;
        struct point v1 = {0, cos(phi), -sin(phi)};
        for (int i = 0; i <= slices; i++) {
            double theta = -half + (i % slices) * angle / slices;
            struct point v2 = {-cos(theta), 0, -sin(theta)};
            point v = cross(v1, v2);
            GLdouble s = 1.0 / length(v);
            v.x *= s;
            v.y *= s;
            v.z *= s;
            points[j][i] = v;
        }
    }
    glBegin(GL_QUADS);
        for (int j = 0; j < stacks; j++) {
            for (int i = 0; i < slices; i++) {
                glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
                glVertex3f(points[j][i+1].x, points[j][i+1].y, points[j][i+1].z);

                glVertex3f(points[j+1][i+1].x, points[j+1][i+1].y, points[j+1][i+1].z);
                glVertex3f(points[j+1][i].x, points[j+1][i].y, points[j+1][i].z);
            }
        }
    glEnd();
}

void drawTriangle(){
    glBegin(GL_TRIANGLES);{
        glVertex3d(sin60, -cos60 , 0);
        glVertex3d(0, 1 , 0);
        glVertex3d(-sin60, -cos60 , 0);
    }glEnd();
}


void drawCylinder(GLdouble a = cylinderAngle, int segments = 720) {
    a = a * M_PI / 180;
    GLdouble tempx = sin(-a/2), tempz = cos(-a/2);
    GLdouble currx, currz;
    glBegin(GL_QUADS);
        for (int i = 1; i <= segments; i++) {
            double theta = -a / 2 + i * a / segments;
            currx = sin(theta);
            currz = cos(theta);

            glVertex3f(currx, 1.0/2, currz);
            glVertex3f(currx, -1.0/2, currz);

            glVertex3f(tempx, -1.0/2, tempz);
            glVertex3f(tempx, 1.0/2, tempz);

            tempx = currx;
            tempz = currz;
        }
    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix
    
    gluLookAt(pos.x,pos.y,pos.z,
              pos.x+l.x,pos.y+l.y,pos.z+l.z,
              u.x,u.y,u.z);

    glRotated(angleSide, 0, 1, 0);
    // glTranslated(0, octaHedronHeight, 0);
    
    //Drawing Triangles
    GLdouble newTriangleRadius = triangleRadius * (1 - changeRatio);
    GLdouble newTriangleSideLength = triangleSideLength * (1 - changeRatio);
    for(int j = 0; j < 2; j++){
        for(int i = 0; i < 4; i++){
            glColor3d((i % 2 == j), (i % 2 != j), 1);
            glPushMatrix();
            glRotated(-triangleCenterAngle, 1, 0, 0);
            glTranslated(0, 0, sphereRadius);
            glScaled(newTriangleRadius, newTriangleRadius, newTriangleRadius);
            drawTriangle();
            glPopMatrix();
            glRotated(90, 0, 1, 0);
        }

        glRotated(180, 1, 0, 0);

    }

    //Drawing Cylinder
    glRotated(cylinderRotateAngle, 0, 1, 0);
    GLdouble newCylinderRadius = sphereRadius * changeRatio;
    for(int j = 0; j < 2; j++){
        for(int i = 0; i < 4; i++){
            glColor3d(1, 1, 0);
            glPushMatrix();
            glRotated(-cylinderRotateAngle, 1, 0, 0);
            glTranslated(0, 0, newTriangleSideLength / 2);
            glScaled(newCylinderRadius, newTriangleSideLength, newCylinderRadius);
            drawCylinder();
            glPopMatrix();
            glRotated(90, 0, 1, 0);
        }

        glRotated(180, 1, 0, 0);
    }
    glRotated(-cylinderRotateAngle, 0, 1, 0);

    for(int i = 0; i < 4; i++){
        glColor3d(1, 1, 0);
        glPushMatrix();
        glTranslated(0, 0, newTriangleSideLength / 2);
        glRotated(90, 0, 0, 1);
        glScaled(newCylinderRadius, newTriangleSideLength, newCylinderRadius);
        drawCylinder();
        glPopMatrix();
        glRotated(90, 0, 1, 0);
    
    }


    //drawSphere
    GLdouble t1 = triangleRadius - newTriangleRadius;
    GLdouble newSphereRadius = sphereRadius * changeRatio;
    GLdouble halfCordLength = sphereRadius * sin(M_PI / 4);
    
    GLdouble centerDistance = octaHedronHeight * (1 - changeRatio);
    glRotated(45, 0, 1, 0);
    glRotated(90, 1, 0, 0);
    for(int i = 0; i < 2; i++){
        glColor3d(1, 0, 0);
        glPushMatrix();
        glTranslated(0, 0, centerDistance);
        glScaled(newSphereRadius, newSphereRadius, newSphereRadius);
        drawSphere();
        glPopMatrix();
        glRotated(180, 1, 0, 0);
    }
    glRotated(-90, 1, 0, 0);

    for(int i = 0; i < 4; i++){
        glColor3d(0, (i % 2), (i % 2 == 0));
        glPushMatrix();
        glTranslated(0, 0, centerDistance);
        glScaled(newSphereRadius, newSphereRadius, newSphereRadius);
        drawSphere();
        glPopMatrix();
        glRotated(90, 0, 1, 0);
    }
    glRotated(-90, 1, 0, 0);



    glutSwapBuffers();  // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void keyboardListener(unsigned char key, int xx,int yy){
    double rate = 0.01, scale = 0.04;
	switch(key){
		case 'w':
		    pos.x += u.x;
			pos.y += u.y;
			pos.z += u.z;

            l.x -= u.x * scale;
            l.y -= u.y * scale;
            l.z -= u.z * scale;
			break;
		case 's':
            pos.x -= u.x;
			pos.y -= u.y;
			pos.z -= u.z;

            l.x += u.x * scale;
            l.y += u.y * scale;
            l.z += u.z * scale;
			break;
        case 'd':
            angleSide += 1;
            if(angleSide >= 360)angleSide -= 360;
            break;
        case 'a':
            angleSide -= 1;
            if(angleSide < 0)angleSide += 360;
            break;
        
		case '1':
			r.x = r.x * cos(rate) + l.x * sin(rate);
			r.y = r.y * cos(rate) + l.y * sin(rate);
			r.z = r.z * cos(rate) + l.z * sin(rate);

			l.x = l.x * cos(rate) - r.x * sin(rate);
			l.y = l.y * cos(rate) - r.y * sin(rate);
			l.z = l.z * cos(rate) - r.z * sin(rate);
			break;

        case '2':
			r.x = r.x * cos(-rate) + l.x * sin(-rate);
			r.y = r.y * cos(-rate) + l.y * sin(-rate);
			r.z = r.z * cos(-rate) + l.z * sin(-rate);

			l.x = l.x * cos(-rate) - r.x * sin(-rate);
			l.y = l.y * cos(-rate) - r.y * sin(-rate);
			l.z = l.z * cos(-rate) - r.z * sin(-rate);
			break;

        case '3':
			l.x = l.x * cos(rate) + u.x * sin(rate);
			l.y = l.y * cos(rate) + u.y * sin(rate);
			l.z = l.z * cos(rate) + u.z * sin(rate);

			u.x = u.x * cos(rate) - l.x * sin(rate);
			u.y = u.y * cos(rate) - l.y * sin(rate);
			u.z = u.z * cos(rate) - l.z * sin(rate);
			break;

        case '4':
			l.x = l.x * cos(-rate) + u.x * sin(-rate);
			l.y = l.y * cos(-rate) + u.y * sin(-rate);
			l.z = l.z * cos(-rate) + u.z * sin(-rate);

			u.x = u.x * cos(-rate) - l.x * sin(-rate);
			u.y = u.y * cos(-rate) - l.y * sin(-rate);
			u.z = u.z * cos(-rate) - l.z * sin(-rate);
			break;

        case '5':
			u.x = u.x * cos(rate) + r.x * sin(rate);
			u.y = u.y * cos(rate) + r.y * sin(rate);
			u.z = u.z * cos(rate) + r.z * sin(rate);

			r.x = r.x * cos(rate) - u.x * sin(rate);
			r.y = r.y * cos(rate) - u.y * sin(rate);
			r.z = r.z * cos(rate) - u.z * sin(rate);
			break;

        case '6':
			u.x = u.x * cos(-rate) + r.x * sin(-rate);
			u.y = u.y * cos(-rate) + r.y * sin(-rate);
			u.z = u.z * cos(-rate) + r.z * sin(-rate);

			r.x = r.x * cos(-rate) - u.x * sin(-rate);
			r.y = r.y * cos(-rate) - u.y * sin(-rate);
			r.z = r.z * cos(-rate) - u.z * sin(-rate);
			break;

        case ',':
            changeRatio += 0.05;
            if(changeRatio > 1)changeRatio = 1;
            break;
        
        case '.':
            changeRatio -= 0.05;
            if(changeRatio < 0)changeRatio = 0;
            break;
		default:
			break;
	}
	glutPostRedisplay();
}


void specialKeyListener(int key, int x,int y)
{
	switch(key){
		case GLUT_KEY_UP:		//down arrow key
			pos.x += l.x;
			pos.y += l.y;
			pos.z += l.z;
			break;
		case GLUT_KEY_DOWN:		// up arrow key
			pos.x -= l.x;
			pos.y -= l.y;
			pos.z -= l.z;
			break;

		case GLUT_KEY_RIGHT:
			pos.x += r.x;
			pos.y += r.y;
			pos.z += r.z;
			break;
		case GLUT_KEY_LEFT :
			pos.x -= r.x;
			pos.y -= r.y;
			pos.z -= r.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos.x += u.x;
			pos.y += u.y;
			pos.z += u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
            pos.x -= u.x;
			pos.y -= u.y;
			pos.z -= u.z;
			break;


		default:
			break;
	}
	glutPostRedisplay();
}


/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                      // Initialize GLUT
    glutInitWindowSize(640, 640);               // Set the window's initial width & height
    glutInitWindowPosition(100, 50);             // Position the window's initial top-left corner
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color
    glutCreateWindow("Magic Cube");      // Create a window with the given title
    glutDisplayFunc(display);                   // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);           // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);         // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);        // Register callback handler for special-key event
    initGL();                                   // Our own OpenGL initialization
    glutMainLoop();                             // Enter the event-processing loop
    return 0;
}

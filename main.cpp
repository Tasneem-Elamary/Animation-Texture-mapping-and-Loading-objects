
#include <math.h>
#include <GL/glut.h>
#include "imageloader.h"
#include <stdio.h>
#include "glm.h"
#include <iostream>


GLfloat angle = 0.0;   /* in degrees */
GLfloat angle2 = 0.0;   /* in degrees */
static int body = 0, shoulderZ = 0, shoulderX = 0, elbow = 0, shoulder2Z = 0, shoulder2X = 0, elbow2 = 0, shoulder = 0, shoulder2 = 0, shoulder1 = 0, shouldery1 = 0;
static int fingerBase1 = 0, fingerUp1 = 0;
static int fingerBase2 = 0, fingerUp2 = 0;
static int fingerBase3 = 0, fingerUp3 = 0;
static int fingerBase4 = 0, fingerUp4 = 0;
static int fingerBase21 = 0, fingerUp21 = 0;
static int fingerBase22 = 0, fingerUp22 = 0;
static int fingerBase23 = 0, fingerUp23 = 0;
static int fingerBase24 = 0, fingerUp24 = 0;
static int RightLegz = 0, RightLegx = 0, RightKneez = 0, RightKneex = 0, RightFoot = 0;
static int LeftLegz = 0, LeftLegx = 0, LeftKneez = 0, LeftKneex = 0, LeftFoot = 0;
int moving, startx, starty;
#define PI 3.14

double eye[] = { 0, 0, 2 };
double center[] = { 0, 0, 0 };
double up[] = { 0.0, 1.0, 0.0 };
const char* modelname1 = "data/stove1.obj";
const char* modelname2 = "data/poele.obj";
//GLMmodel* pmodel2;

const char* modelname3 = "data/white_kitchen_chair.obj";


//ball
static float ballx = -0.4, bally = -1.3, ballz = 0;
//poele
static float xpoele = -.5;
static float ypoele = -.15;
static float zpoele = -1.25;

GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

// x , y, z, w

GLfloat light_position[] = { 0, 0 , 2.0, 1.0 };
GLfloat lightPos1[] = { -0.5, -5.0, -2.0, 1.0 };
float xBody, yBody, zBody, B_oriantation = 0;
void drawmodel(char* filename)
{
    GLMmodel* model = glmReadOBJ(filename);
    glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0);
    glmScale(model, .15);
    glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
}


//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image)
{
    GLuint textureId;
    glGenTextures(1, &textureId);			 //Make room for our texture
    glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,				  //Always GL_TEXTURE_2D
        0,							  //0 for now
        GL_RGB,					  //Format OpenGL uses for image
        image->width, image->height, //Width and height
        0,							  //The border of the image
        GL_RGB,					  //GL_RGB, because pixels are stored in RGB format
        GL_UNSIGNED_BYTE,			  //GL_UNSIGNED_BYTE, because pixels are stored
                                     //as unsigned numbers
        image->pixels);			  //The actual pixel data
    return textureId;						  //Returns the id of the texture
}

GLuint _textureId; //The id of the texture

//Initializes 3D rendering
//Initializes 3D rendering
void initRendering(const char* floorname, GLuint& textureID)
{
    Image* image = loadBMP(floorname);
    _textureId = loadTexture(image);
    delete image;
    // Turn on the power
    glEnable(GL_LIGHTING);
    // Flip light switch
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    // assign light parameters
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    // Material Properties
    GLfloat lightColor1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor1);
    glEnable(GL_NORMALIZE);
    //Enable smooth shading
    glShadeModel(GL_SMOOTH);
    // Enable Depth buffer
    glEnable(GL_DEPTH_TEST);
}

void crossProduct(double a[], double b[], double c[])
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(double a[])
{
    double norm;
    norm = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
    norm = sqrt(norm);
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}

void rotatePoint(double a[], double theta, double p[])
{

    double temp[3];
    temp[0] = p[0];
    temp[1] = p[1];
    temp[2] = p[2];

    temp[0] = -a[2] * p[1] + a[1] * p[2];
    temp[1] = a[2] * p[0] - a[0] * p[2];
    temp[2] = -a[1] * p[0] + a[0] * p[1];

    temp[0] *= sin(theta);
    temp[1] *= sin(theta);
    temp[2] *= sin(theta);

    temp[0] += (1 - cos(theta)) * (a[0] * a[0] * p[0] + a[0] * a[1] * p[1] + a[0] * a[2] * p[2]);
    temp[1] += (1 - cos(theta)) * (a[0] * a[1] * p[0] + a[1] * a[1] * p[1] + a[1] * a[2] * p[2]);
    temp[2] += (1 - cos(theta)) * (a[0] * a[2] * p[0] + a[1] * a[2] * p[1] + a[2] * a[2] * p[2]);

    temp[0] += cos(theta) * p[0];
    temp[1] += cos(theta) * p[1];
    temp[2] += cos(theta) * p[2];

    p[0] = temp[0];
    p[1] = temp[1];
    p[2] = temp[2];
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0, 1.0, 1.0, 20.0);
    //glMatrixMode(GL_MODELVIEW);
}

void reset()
{
    double e[] = { 0.0, 0.0, 2 };
    double c[] = { 0.0, 0.0, 1.0 };
    double u[] = { 0.0, 1.0, 0.0 };
    for (int i = 0; i < 3; i++)
    {
        eye[i] = e[i];
        center[i] = c[i];
        up[i] = u[i];
    }
}

void display(void)
{

    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Clear Depth and Color buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2, -1.5, 1);
    glTexCoord2f(3.0f, 0.0f); glVertex3f(2, -1.5, 1);
    glTexCoord2f(3.0f, 3.0f); glVertex3f(2, -1.5, -1);
    glTexCoord2f(0.0f, 3.0f); glVertex3f(-2, -1.5, -1);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    //model 1
    glPushMatrix();

    glRotatef(-90, 0.0, 1.0, 0.0);
    glTranslatef(-.5, -.75, -2);
    glScalef(5, 5, 5);
    drawmodel((char*)modelname1);

    glPopMatrix();

    //model 2
    glPushMatrix();

    glRotatef(-90, 0.0, 1.0, 0.0);
    glTranslatef(xpoele,ypoele, zpoele);
    glScalef(3, 3, 3);
    drawmodel((char*)modelname2);

    glPopMatrix();
    //MODEL3
    glPushMatrix();

    glRotatef(90, 0.0, 1.0, 0.0);
    glTranslatef(.5, -1, -1.5);
    glScalef(4, 7, 4);
    drawmodel((char*)modelname3);

    glPopMatrix();

    glTranslatef(xBody, yBody, zBody);
    glRotatef(B_oriantation, 0.0, 1.0, 0.0);
    glScalef(0.2, 0.2, 0.2);
    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);

    //body//
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef((GLfloat)body, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, 0.0);
    glPushMatrix();
    glScalef(1.5, 4.0, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    //head//
    glPushMatrix();
    glTranslatef(0.0, 2.6, 0.0);
    glutWireSphere(0.5, 10, 10);
    glPopMatrix();



    //left arm//
    glPushMatrix();
    glTranslatef(0.9, 1.85, 0.0);
    glRotatef((GLfloat)shoulder, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.55, 0.0);
    glPushMatrix();
    glScalef(0.3, 1.25, 0.3);
    glutWireCube(1.0);
    glPopMatrix();



    glTranslatef(0.0, -0.6, 0.0);
    glRotatef((GLfloat)elbow, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.65, 0.0);
    glPushMatrix();
    glScalef(0.3, 1.25, 0.3);
    glutWireCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glPushMatrix();
    glPushMatrix();
    glPushMatrix();
    glPopMatrix();
    glTranslatef(-0.1, -0.35, -0.1);
    glRotatef((GLfloat)fingerBase1, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    //Draw finger flang 1 
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp1, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    // draw finger 2
    glPopMatrix();
    glTranslatef(-0.1, -0.35, 0.0);
    glRotatef((GLfloat)fingerBase2, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();

    // draw finger 2
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp2, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();

    //draw finger 3
    glPopMatrix();
    glTranslatef(-0.1, -0.35, 0.1);
    glRotatef((GLfloat)fingerBase3, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1.0);
    glPopMatrix();

    // draw finger 3
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp3, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    //Draw finger flang 4 
    glPopMatrix();
    glTranslatef(0.1, -0.35, 0.0);
    glRotatef((GLfloat)fingerBase4, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    //Draw finger flang 4
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp4, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();

    glPopMatrix();


    //Right arm//
    glPushMatrix();
    glTranslatef(-0.9, 1.85, 0.0);
    glRotatef((GLfloat)shoulder2, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.55, 0.0);
    glPushMatrix();
    glScalef(0.3, 1.25, 0.3);
    glutWireCube(1.0);
    glPopMatrix();



    glTranslatef(0.0, -0.6, 0.0);
    glRotatef((GLfloat)elbow2, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.65, 0.0);
    glPushMatrix();
    glScalef(0.3, 1.25, 0.3);
    glutWireCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glPushMatrix();
    glPushMatrix();
    glPushMatrix();
    glPopMatrix();
    glTranslatef(0.1, -0.35, -0.1);
    glRotatef((GLfloat)fingerBase21, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    //Draw finger flang 1 
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp21, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    // draw finger 2
    glPopMatrix();
    glTranslatef(0.1, -0.35, 0.0);
    glRotatef((GLfloat)fingerBase22, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();

    // draw finger 2
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp22, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();

    //draw finger 3
    glPopMatrix();
    glTranslatef(0.1, -0.35, 0.1);
    glRotatef((GLfloat)fingerBase23, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1.0);
    glPopMatrix();

    // draw finger 3
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp23, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    //Draw finger flang 4 
    glPopMatrix();
    glTranslatef(-0.1, -0.35, 0.0);
    glRotatef((GLfloat)fingerBase24, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.4, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();


    //Draw finger flang 4
    glTranslatef(0.0, -0.1, 0.0);
    glRotatef((GLfloat)fingerUp24, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.15, 0.0);
    glPushMatrix();
    glScalef(0.1, 0.3, 0.1);
    glutWireCube(1);
    glPopMatrix();

    glPopMatrix();



    //right leg//
    glPushMatrix();

    glTranslatef(-0.55, -2.0, 0.0);
    glRotatef((GLfloat)RightLegz, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)RightLegx, 1.0, 0.0, 0.0);
    glTranslatef(0.05, -0.9, 0.0);

    glPushMatrix();
    glScalef(0.5, 1.7, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0.0, -0.8, 0.0);
    glRotatef((GLfloat)RightKneez, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)RightKneex, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.8, 0.0);

    glPushMatrix();
    glScalef(0.5, 1.7, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0.0, -1.0, 0.0);
    glRotatef((GLfloat)RightFoot, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, 0.0);
    /*glPushMatrix();*/
    glScalef(0.55, 0.5, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    //left leg//
    glPushMatrix();
    /*glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);*/
    glTranslatef(0.55, -2.0, 0.0);
    glRotatef((GLfloat)LeftLegz, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)LeftLegx, 1.0, 0.0, 0.0);
    glTranslatef(-0.05, -0.9, 0.0);
    glPushMatrix();
    glScalef(0.5, 1.7, 0.5);
    glutWireCube(1.0);

    glPopMatrix();

    glTranslatef(0.0, -0.8, 0.0);
    glRotatef((GLfloat)LeftKneez, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)LeftKneex, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.9, 0.0);
    glPushMatrix();
    glScalef(0.5, 1.7, 0.5);
    glutWireCube(1.0);

    glPopMatrix();

    glTranslatef(0.0, 0.0, 0.0);
    glRotatef((GLfloat)LeftFoot, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -1.0, 0.0);
    /* glPushMatrix();*/
    glScalef(0.55, 0.5, 1.0);
    glutSolidCube(1.0);

    glPopMatrix();


    glPopMatrix();
    glutSwapBuffers();
}

void floor_menu(int value)
{
    switch (value)
    {
    case '1':
        initRendering("images/f2.bmp", _textureId);
        break;
    case '2':
        initRendering("images/f3.bmp", _textureId);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
void main_menu(int value)
{
    if (value == 0)
    {
        printf("xbody:%d \n", xBody);
        printf("ybody:%d \n", elbow2);
        printf("zbody:%d \n", zBody);
        printf("zbody:%d \n", B_oriantation);

    }
    /* else if (value == 1)
     {
         for (int i = 0; i < 3, i++) {
             printf("eye[%d]-%f \n", i, eye[i]);
         }*/

}


void attachMenu()
{
    glutCreateMenu(floor_menu);
    glutAddMenuEntry("f1", 0);
    glutAddMenuEntry("f2", '1');
    glutAddMenuEntry("f3", '2');
    glutAttachMenu(GLUT_RIGHT_BUTTON);

}
void setPoses(int frame) {
    float poses[20][11] = {
         {0.0,-.25,0.0,0.0,0,0,0,0,0,0,-0.15},
         {0.0,-0.25,0.0,90.0,0,0,0,0,0,0,-0.15},
         {0.0,-0.25,0.0,90.0,60,0,-60,-30,0,0,-0.15},
         {-.5,-.25,0.0,90.0,-40,60,-60,-60,0,0,-0.15},
          {-.95,-.25,0.0,90.0,0,0,0,0,0,0,-0.15},
         {-1.25,-.25,0.0,90.0,-90,-90,90,90,0,0,-0.15},
         {-.95,-.25,0.0,90.0,0,0,0,0,0,0,-0.15},
         {-.5,-.25,0.0,90.0,0,-60,30,60,0,0,-0.15},
         {0,-.25,0.0,90.0,-60,40,60,60,0,0,-0.15},
        {.5,-.25,0.0,90.0,0,-60,30,60,0,0,-0.15},
        {.70,-.25,0.0,90.0,0,0,0,0,0,0,-0.15},
         {.70,-.25,0.0,90.0,0,0,0,0,-90,-90,-.15},
                  {.70,-.25,0.0,90.0,0,0,0,0,-100,-100,0},
                           {.70,-.25,0.0,90.0,0,0,0,0,-90,-90,-.15},




                  {0.0,-.25,0.0,0.0,0,0,0,0,0,0,-.15},
                                    {0.0,.5,0.0,0.0,0,0,-0,0,45,-45,-.15},
                                    {0.0,-.25,0.0,0.0,0,0,0,0,0,0,-.15},
                                    {0.0,.5,0.0,0.0,0,0,-0,0,45,-45,-.15},
                                    {0.0,-.25,0.0,0.0,0,0,0,0,0,0,-.15},
                                    {0.0,.5,0.0,0.0,0,0,-0,0,45,-45,-.15}













    };

    xBody = poses[frame][0];
    yBody = poses[frame][1];
    zBody = poses[frame][2];
    B_oriantation = poses[frame][3];
    RightLegx = poses[frame][4];
    LeftLegx = poses[frame][5];
    RightKneex = poses[frame][6];
    LeftKneex = poses[frame][7];
    elbow = poses[frame][8];
    elbow2 = poses[frame][9];
    ypoele = poses[frame][10];






    //elbow = poses[frame][4];
    //elbow2 = poses[frame][5];


}
static int f = 0;
void timer(int value)
{
    f = f % 20;
    setPoses(f);
    f++;
    glutPostRedisplay();
    glutTimerFunc(800, timer, 0);

}



void lookRight()
{
    rotatePoint(up, PI / 20, eye);
}

void lookLeft()
{
    rotatePoint(up, -PI / 20, eye);
}

// Rotation about horizontal direction

void lookUp()
{
    double horizontal[3];
    double look[] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    crossProduct(up, look, horizontal);
    normalize(horizontal);
    rotatePoint(horizontal, PI / 20, eye);
    rotatePoint(horizontal, PI / 20, up);
}

void lookDown()
{
    double horizontal[3];
    double look[] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    crossProduct(up, look, horizontal);
    normalize(horizontal);
    rotatePoint(horizontal, -PI / 20, eye);
    rotatePoint(horizontal, -PI / 20, up);
}

// Forward and Backward
void moveForward()
{
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];
    double speed = 0.1;
    eye[0] += direction[0] * speed;
    eye[1] += direction[1] * speed;
    eye[2] += direction[2] * speed;

    center[0] += direction[0] * speed;
    center[1] += direction[1] * speed;
    center[2] += direction[2] * speed;
}

void moveBackword()
{
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];
    double speed = 0.1;
    eye[0] -= direction[0] * speed;
    eye[1] -= direction[1] * speed;
    eye[2] -= direction[2] * speed;

    center[0] -= direction[0] * speed;
    center[1] -= direction[1] * speed;
    center[2] -= direction[2] * speed;

}
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        lookLeft();
        break;
    case GLUT_KEY_RIGHT:
        lookRight();
        break;
    case GLUT_KEY_UP:
        lookUp();
        break;
    case GLUT_KEY_DOWN:
        lookDown();
        break;
    }
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '+':
        moveForward();
        glutPostRedisplay();
        break;
    case '-':
        moveBackword();
        glutPostRedisplay();
        break;
    case '0':
        reset();
        glutPostRedisplay();
        break;
    case'1':
        xBody += 1;
        glutPostRedisplay();
        break;
    case'2':
        xBody -= 1;
        glutPostRedisplay();
        break;

    case 's':
        if (shoulder < 150) {
            shoulder = (shoulder + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'S':
        if (shoulder > 0) {
            shoulder = (shoulder - 5) % 360;
            glutPostRedisplay();
        }
        break;
        // Elbow
    case 'e':
        if (elbow < 90) {
            elbow = (elbow + 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'E':
        if (elbow > -90) {
            elbow = (elbow - 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'A':
        if (shoulder2 < 0) {
            shoulder2 = (shoulder2 + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'a':
        if (shoulder2 > -150) {
            shoulder2 = (shoulder2 - 5) % 360;
            glutPostRedisplay();
        }
        break;
        // Elbow
    case 'W':
        if (elbow2 < 90) {
            elbow2 = (elbow2 + 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'w':
        if (elbow2 > -90) {
            elbow2 = (elbow2 - 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'x':
        if (RightLegx > -90) {
            RightLegx = (RightLegx - 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'X':
        if (RightLegx < 90) {
            RightLegx = (RightLegx + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'z':
        if (RightKneex > -20) {
            RightKneex = (RightKneex - 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'Z':
        if (RightKneex < 90) {
            RightKneex = (RightKneex + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'D':
        if (RightLegz < 90) {
            RightLegz = (RightLegz + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'd':
        if (RightLegz > -90) {
            RightLegz = (RightLegz - 5) % 360;
            glutPostRedisplay();
        }
        break;

    case 'R':
        if (RightKneez < 30) {
            RightKneez = (RightKneez + 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'r':
        if (RightKneez > -30) {
            RightKneez = (RightKneez - 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'f':
        if (LeftLegz < 90) {
            LeftLegz = (LeftLegz + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'F':
        if (LeftLegz > -90) {
            LeftLegz = (LeftLegz - 5) % 360;
            glutPostRedisplay();
        }
        break;

    case 't':
        if (LeftKneez < 50) {
            LeftKneez = (LeftKneez + 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'T':
        if (LeftKneez > -50) {
            LeftKneez = (LeftKneez - 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'c':
        if (LeftLegx > -90) {
            LeftLegx = (LeftLegx - 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'C':
        if (LeftLegx < 90) {
            LeftLegx = (LeftLegx + 5) % 360;
            glutPostRedisplay();
        }
        break;

    case 'v':
        if (LeftKneex > -20) {
            LeftKneex = (LeftKneex - 5) % 180;
            glutPostRedisplay();
        }
        break;
    case 'V':
        if (LeftKneex < 90) {
            LeftKneex = (LeftKneex + 5) % 180;
            glutPostRedisplay();
        }
        break;


    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            starty = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
    }
}


static void motion(int x, int y)
{
    if (moving) {
        angle = angle + (x - startx);
        angle2 = angle2 + (y - starty);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutCreateWindow("Animation");
    init();
    attachMenu();
    glutTimerFunc(0, timer, 0);
    initRendering("images/f1.bmp", _textureId);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}
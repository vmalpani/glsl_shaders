/* Simple opengl demo program. 
 *
 * Author: Papoj Thamjaroenporn
 *         Changxi Zheng
 * Spring 2013
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <GL/glew.h>
#if defined(_WIN32)
#   include <GL/wglew.h>
#endif

#if defined(__APPLE__) || defined(MACOSX)
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

#include "GLScreenCapturer.h"
#include "trackball.h"
#include "shader.h"
#include "GLSLProgram.h"

using namespace std;

#define BUFFER_LENGTH 64
#define brickImageWidth 512
#define brickImageHeight 256
#define ImageWidth 1024
#define ImageHeight 1024

GLfloat camRotX, camRotY, camPosX, camPosY, camPosZ;
GLint viewport[4];
GLuint color_tex, norm_tex;
GLdouble modelview[16];
GLdouble projection[16];

GLuint pickedObj = -1;
char titleString[150];
char disp_txt[50] = "";
char toon_txt[50] = "TOON SHADING";
char gouraud_txt[50] = "GOURAUD SHADING";
char phong_txt[50] = "PHONG SHADING";
char blinn_phong_txt[50] = "BLINN-PHONG SHADING ";
char checker_txt[50] = "CHECKER TEXTURE";
char norm_txt[50] = "NORMAL(BUMP) MAPPING ";

bool isTeapot1_selected = false;
bool isTeapot2_selected = false;
bool TOON = false;
bool PHONG = false;
bool GOURAUD = false;
bool CHECKER = false;
bool BLINN_PHONG = false;
bool BUMP_MAPPING = true;
bool TEXT_STYLE = false;
bool LIGHT_0 = true;
bool LIGHT_1 = false;
bool LIGHT_2 = false;

int num_lights = 1;

// Lights & Materials
GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat position[] = {0.0, 0.0, 2.0, 1.0};
GLfloat position1[] = {-2.0, -2.0, 2.0, 1.0};
GLfloat position2[] = {2.0, 2.0, 2.0, 1.0};
GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};

static GLScreenCapturer screenshot("screenshot-%d.ppm");
static GLSLProgram*     shaderProg = NULL;

using namespace std;

void initLights(void)
{
    glEnable(GL_LIGHTING);
    
    if (num_lights == 1){
        glEnable(GL_LIGHT0);
    
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
    }
    else if (num_lights == 2){
        glEnable(GL_LIGHT0);
        
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        
        glEnable(GL_LIGHT1);
        
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT1, GL_POSITION, position1);
    }
    else if (num_lights == 3){
        glEnable(GL_LIGHT0);
        
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        
        glEnable(GL_LIGHT1);
        
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT1, GL_POSITION, position1);
        
        glEnable(GL_LIGHT2);
        
        glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT2, GL_POSITION, position2);
    }
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void setupRC()
{
    tbInit(GLUT_RIGHT_BUTTON);
    tbAnimate(GL_TRUE);
    
    // Place Camera
    camRotX = 350.0f;
    camRotY = 680.0f;
    camPosX = 0.0f;
    camPosY = 0.0f;
    camPosZ = -10.5f;
    
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    initLights();
}

void setCamera( void )
{
    glTranslatef(0, 0, camPosZ);
    glRotatef(camRotX, 1, 0, 0);
    glRotatef(camRotY, 0, 1, 0);
}


// helper function called to render text on openGL window
// reference: OpenGL - A Primer by E. Angel
void Sprint( GLfloat x, GLfloat y, GLfloat z, char *st, bool style)
{
    int l,i;
    l=strlen( st );
    glRasterPos3f( x, y, z);
    for( i=0; i < l; i++)
    {
        if (style)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st[i]); // Print a character on the screen
        else
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, st[i]); // Print a character on the screen
    }
}

// helper function to load bmp files as textures
// reference: swiftless openGL tutorial
GLuint LoadTexture( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data;
    FILE * file = fopen(filename, "r");
    
    if ( file == NULL ) {
        std::cout<<"Katta!"<<std::endl;
        return 0;
    }
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );
    
    glGenTextures( 1, &texture ); //generate the texture with
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE ); //set texture environment parameters
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    //repeat texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
    
    //generate texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,GL_BGR, GL_UNSIGNED_BYTE, data);
    free(data);
    return texture;
}

void drawSelectableTeapots( void )
{
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);
    
    GLfloat selectedColor[] = {0, 1, 0, 1};
    GLfloat unselectedColor[] = {1, 0, 0, 1};

    // Initialize the name stack
    glInitNames();
    glPushName(0);
    
    shaderProg->enable();
    shaderProg->set_uniform_3f("lightDir", 2.f, -1.f, -0.3f);
    // Draw two teapots next to each other in z axis
    glPushMatrix();
    {
    
        if( isTeapot1_selected )
            glMaterialfv(GL_FRONT, GL_DIFFUSE, selectedColor);
        else
            glMaterialfv(GL_FRONT, GL_DIFFUSE, unselectedColor);
        glLoadName(0);
        glutSolidTeapot(2.5);

        if( isTeapot2_selected )
            glMaterialfv(GL_FRONT, GL_DIFFUSE, selectedColor);
        else
            glMaterialfv(GL_FRONT, GL_DIFFUSE, unselectedColor);
        glLoadName(1);
        glTranslatef(0,0,5);
        glutSolidTeapot(1.5);
    }
    glPopMatrix();
    shaderProg->disable();
    
    glColor4fv(currentColor);

}

float ex=0,ey=0,ez=-7,cx=0,cy=0.5,cz=0,ux=0,uy=1,uz=0;
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if(BUMP_MAPPING)
    {
        Sprint(0.85, 0.8, -2.75, norm_txt, TEXT_STYLE);
        GLfloat clearColor[] = {0, 0, 0, 1};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, clearColor);
        glEnable( GL_TEXTURE_2D );
        glEnable(GL_BLEND);
        shaderProg->bind_texture("colorTex", color_tex, GL_TEXTURE_2D, 0);
        shaderProg->bind_texture("normalTex", norm_tex, GL_TEXTURE_2D, 1);

        glBegin(GL_QUADS);
        glNormal3f(0.0,0.0,1.0);
        glTexCoord2f(0,0);
        glVertex3f(-1,1,-4);
        glTexCoord2f(0,1);
        glVertex3f(-1,-1,-4);
        glTexCoord2f(1,1);
        glVertex3f(1,-1,-4);
        glTexCoord2f(1,0);
        glVertex3f(1,1,-4);
        glEnd();
    }
    else{
    glPushMatrix();
    {
        if (TOON)               Sprint(0.85, 0.8, -2.75, toon_txt, TEXT_STYLE);
        else if(GOURAUD)        Sprint(0.85, 0.8, -2.75, gouraud_txt, TEXT_STYLE);
        else if (PHONG)         Sprint(0.85, 0.8, -2.75, phong_txt, TEXT_STYLE);
        else if (BLINN_PHONG)   Sprint(0.85, 0.8, -2.75, blinn_phong_txt, TEXT_STYLE);
        else if (CHECKER)       Sprint(0.85, 0.8, -2.75, checker_txt, TEXT_STYLE);
        else                    Sprint(0.85, 0.8, -2.75, disp_txt, TEXT_STYLE);
        setCamera();
        tbMatrix();
        drawSelectableTeapots();
        
        // Retrieve current matrice before they popped.
        glGetDoublev( GL_MODELVIEW_MATRIX, modelview );        // Retrieve The Modelview Matrix
        glGetDoublev( GL_PROJECTION_MATRIX, projection );    // Retrieve The Projection Matrix
        glGetIntegerv( GL_VIEWPORT, viewport );                // Retrieves The Viewport Values (X, Y, Width, Height)
    }
    glPopMatrix();
    }

    glFlush();
    // End Drawing calls
    glutSwapBuffers();
}

void reshape( int w, int h )
{
    tbReshape(w, h);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set the clipping volume
    gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void keyboard( unsigned char key, int x, int y )
{
    switch(key)
    {
    case 'p':
        TOON = false;
        PHONG = true;
        GOURAUD = false;
        BLINN_PHONG = false;
        CHECKER = false;
        BUMP_MAPPING = false;
        delete shaderProg;
        shaderProg = new GLSLProgram(phongVS, phongFS);
        cout<<"In Phong"<<endl;
        break;
            
    case 'b':
        TOON = false;
        PHONG = false;
        GOURAUD = false;
        BLINN_PHONG = true;
        CHECKER = false;
        BUMP_MAPPING = false;
        delete shaderProg;
        shaderProg = new GLSLProgram(blinnphongVS, blinnphongFS);
        cout<<"In Blinn-Phong"<<endl;
        break;
            
    case 'g':
        TOON = false;
        PHONG = false;
        GOURAUD = true;
        BLINN_PHONG = false;
        CHECKER = false;
        BUMP_MAPPING = false;
        delete shaderProg;
        shaderProg = new GLSLProgram(gouraudVS, gouraudFS);
        cout<<"In Gouraud"<<endl;
        break;
            
    case 'c':
        TOON = false;
        PHONG = false;
        GOURAUD = false;
        BLINN_PHONG = false;
        CHECKER = true;
        BUMP_MAPPING = false;
        delete shaderProg;
        shaderProg = new GLSLProgram(checkerVS, checkerFS);
        cout<<"In Checker"<<endl;
        break;
        
    case 't':
        TOON = true;
        PHONG = false;
        GOURAUD = false;
        BLINN_PHONG = false;
        CHECKER = false;
        BUMP_MAPPING = false;
        delete shaderProg;
        shaderProg = new GLSLProgram(toonVS, toonFS);
        cout<<"In Toon"<<endl;
        break;
            
    case 'n':
        TOON = false;
        PHONG = false;
        GOURAUD = false;
        BLINN_PHONG = false;
        CHECKER = false;
        BUMP_MAPPING = true;
        delete shaderProg;
        shaderProg = new GLSLProgram(toonVS, toonFS);
        cout<<"In Bump Mapping"<<endl;
        break;

    case '1':
        num_lights = 1;
        LIGHT_0 = !LIGHT_0;
        initLights();
        cout<<"Light 0 turned on"<<endl;
        break;
            
    case '2':
        num_lights = 2;
        LIGHT_1 = !LIGHT_1;
        initLights();
        cout<<"Light 0 and Light 1 turned on"<<endl;
        break;

    case '3':
        num_lights = 3;
        LIGHT_2 = !LIGHT_2;
        initLights();
        cout<<"Light 0, Light 1 and Light 2 turned on"<<endl;
        break;
            
    case 27: // Escape key
        exit(0);
        break;
            
    case 'r':
        printf("save current screen\n");
        screenshot.capture();
        break;
    }
    shaderProg -> enable();
    GLint loc = glGetUniformLocation(shaderProg->prog_, "num_lights");
    glUniform1i(loc, num_lights);
    glutPostRedisplay();
    shaderProg -> disable();
}

void processSelection(int xPos, int yPos)
{
    GLfloat fAspect;
    
    // Space for selection buffer
    static GLuint selectBuff[BUFFER_LENGTH];
    
    // Hit counter and viewport storage
    GLint hits, viewport[4];
    
    // Setup selection buffer
    glSelectBuffer(BUFFER_LENGTH, selectBuff);
    
    // Get the viewport
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // Switch to projection and save the matrix
    glMatrixMode(GL_PROJECTION);
    
    glPushMatrix();
    {
        // Change render mode
        glRenderMode(GL_SELECT);
        
        // Establish new clipping volume to be unit cube around
        // mouse cursor point (xPos, yPos) and extending two pixels
        // in the vertical and horizontal direction
        glLoadIdentity();
        gluPickMatrix(xPos, viewport[3] - yPos + viewport[1], 0.1,0.1, viewport);
        
        // Apply perspective matrix 
        fAspect = (float)viewport[2] / (float)viewport[3];
        gluPerspective(45.0f, fAspect, 1.0, 425.0);
        
        
        // Render only those needed for selection
        if(BUMP_MAPPING)
        {
            GLfloat clearColor[] = {0, 0, 0, 1};
            glMaterialfv(GL_FRONT, GL_DIFFUSE, clearColor);
            glEnable( GL_TEXTURE_2D );
            glEnable(GL_BLEND);
            shaderProg->bind_texture("colorTex", color_tex, GL_TEXTURE_2D, 0);
            shaderProg->bind_texture("normalTex", norm_tex, GL_TEXTURE_2D, 1);

            glBegin(GL_QUADS);
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2f(0,0);
            glVertex3f(-1,-1,-4);
            glTexCoord2f(0,1);
            glVertex3f(-1,1,-4);
            glTexCoord2f(1,1);
            glVertex3f(1,1,-4);
            glTexCoord2f(1,0);
            glVertex3f(1,-1,-4);
            glEnd();
        }
        else{
        glPushMatrix();
            {
                setCamera();
                tbMatrixForSelection();

                drawSelectableTeapots();
            }
            glPopMatrix();
        }
        
        // Collect the hits
        hits = glRenderMode(GL_RENDER);
        
        isTeapot1_selected = false;
        isTeapot2_selected = false;
        
        // If hit(s) occurred, display the info.
        if(hits != 0)
        {
        
            // Save current picked object.
            // Take only the nearest selection
            pickedObj = selectBuff[3];
            
            sprintf (titleString, "You clicked on %d", pickedObj);
            glutSetWindowTitle(titleString);
            
            if (pickedObj == 0) {
                isTeapot1_selected = true;
            }
            
            if (pickedObj == 1) {
                isTeapot2_selected = true;
            }
            
        }
        else
            glutSetWindowTitle("Nothing was clicked on!");
        
        
        // Restore the projection matrix
        glMatrixMode(GL_PROJECTION);
    }
    glPopMatrix();
    
    // Go back to modelview for normal rendering
    glMatrixMode(GL_MODELVIEW);
    
    glutPostRedisplay();
}

void mouse( int button, int state, int x, int y)
{
    tbMouse(button, state, x, y);
    
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        processSelection(x, y);
    
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        pickedObj = -1;
        glutPostRedisplay();
    }
}

void motion(int x, int y)
{
    tbMotion(x, y);
    
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
    
    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
    glutPostRedisplay();

}

static void setupShaders()
{
    printf("MSG: Initialize GLSL Shaders ...\n");

    glewInit();
    if ( !glewIsSupported("GL_VERSION_2_0 GL_ARB_multitexture GL_EXT_framebuffer_object") ) 
    {
        fprintf(stderr, "Required OpenGL extensions missing\n");
        exit(2);
    }
    
    shaderProg = new GLSLProgram(phongVS,phongFS);
}

int main (int argc, char *argv[])
{
    int win_width = 960;
    int win_height = 540;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize( win_width, win_height );

    glutCreateWindow( "Opengl demo" );
    setupShaders();
    setupRC();
    color_tex=LoadTexture( "color_map.bmp", ImageWidth, ImageHeight);
    norm_tex=LoadTexture( "normal_map.bmp", ImageWidth, ImageHeight);
    
//    tex=LoadTexture( "bricks.bmp", brickImageWidth, brickImageHeight);
//    norm_tex=LoadTexture( "norm_bricks.bmp", brickImageWidth, brickImageHeight);

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );
    glutMotionFunc( motion );

    glutMainLoop();
    delete shaderProg;
}

//  COMP510: Computer Graphics
//  Assignment 1
//  Programmed by Mohammad Laghari (0059487)
//  Email: mlaghari16@ku.edu.tr
//  Instructor: Dr. Yucel Yemez

#include "include/Angel.h"
#include "InitShader.cpp"
#include <stdio.h>
#include <fstream>

using namespace std;

// Variables for increasing/decreasing animation speed
#define GROWING_FACTOR_MAX 1
#define GROWING_FACTOR_MIN 0.001
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
double GROWING_RATE = 0.1;

typedef vec4 color4;
typedef vec4 point4;

// Variables to cater for changing objects during animations
// Each variable is used to change the number of vertices for separate objects
const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int numVerticesInUse = NumVertices;
int numVerticesSphere, numVerticesCar;
int numTriangles_car, numTriangles_sphere;

// Cube data-structures
point4 cube[NumVertices];
color4 colors[NumVertices];

// Variable used for color of object
// This color will be applied in the shader
vec4  shapeColor = vec4(1.0,0.0,0.0,0.0);
// Menu level variables (Right-click) menus
static int rightClickMenu, drawingMode, colorSelector, mainMenuOpener, rotateMode, background;
// Enum classes for various purposes
// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
enum{ BLACK=0, RED=1, YELLOW=2, GREEN=3, BLUE=4, MAGENTA=5, WHITE=6, CYAN=7, PINK=8};
// Scale Factor
double FACTOR = 0.01;
// Variable growth factor for animation
double GROWING_FACTOR = 0.1;
bool SCALE = false;
// Variable used for rotation
GLfloat  Theta[NumAxes] = { 0.0, 00.0, 00.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection, shaderColor, vPosition, vCar, vSphere;
int Index = 0;

//----------------------------------------------------------------------------

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 vertex_colors[9] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),       // black
    color4( 1.0, 0.0, 0.0, 1.0 ),       // red
    color4( 1.0, 1.0, 0.0, 1.0 ),       // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),       // green
    color4( 0.0, 0.0, 1.0, 1.0 ),       // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),       // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),       // white
    color4( 0.0, 1.0, 1.0, 1.0 ),       // cyan
    color4( 1.0, 0.08, 0.58, 1.0 )     // pink
};

//----------------------------------------------------------------------------

/* Initialize the cube */
void quad( int a, int b, int c, int d ) {
    cube[Index] = vertices[a]; Index++;
    cube[Index] = vertices[b]; Index++;
    cube[Index] = vertices[c]; Index++;
    cube[Index] = vertices[a]; Index++;
    cube[Index] = vertices[c]; Index++;
    cube[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

/* Create Cube */
void colorcube() {
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init() {
    /* Loading shapes into environment */
    colorcube();

    // Reading Car.off to generate shape
    int numVerticesOFF, numTriangles, edges, numVertex, v1, v2, v3;
    GLdouble x, y, z, minX, maxX, minY, maxY, minZ, maxZ;
    ifstream file;
    file.open("input_files/car.off");
    point4 temp;
    point4 *coordinates_car, *coordinates_car_tmp;
    point4 *shape_car;
    string input;
    file >> input;
    if (input == "OFF") {
        file >> numVerticesOFF >> numTriangles >> edges;
        coordinates_car = new point4[numVerticesOFF];
        coordinates_car_tmp = new point4[numVerticesOFF];
        // storing the coordinates for Car.off
        // finding min, max
        for (int i = 0 ; i < numVerticesOFF ; i++) {
            file >> x >> y >> z;
            if (x > maxX) 
                maxX = x;
            if (x < minX) 
                minX = x;
            if (y > maxY) 
                maxY = y;
            if (y < minY) 
                minY = y;
            if (z > maxZ) 
                maxZ = z;
            if (z < minZ) 
                minZ = z;
            temp = point4(x, y, z, 1.0);
            coordinates_car_tmp[i] = temp;
        }
        GLfloat xTmp, yTmp, zTmp;
        // Normalizing the coordinates between [-1,+1]
        for (int i = 0 ; i < numVerticesOFF ; i++) {
            xTmp = 2.0*(coordinates_car_tmp[i].x-minX)/(maxX-minX) - 1.0;
            yTmp = 2.0*(coordinates_car_tmp[i].y-minY)/(maxY-minY) - 1.0;
            zTmp = 2.0*(coordinates_car_tmp[i].z-minZ)/(maxZ-minZ) - 1.0;
            temp = point4(xTmp, yTmp, zTmp, 1.0);
            coordinates_car[i] = temp;
        }
        shape_car = new point4[numTriangles*3];
        // aligninig the vertices for the object (using previously loaded coordinates)
        for (int i = 0 ; i < numTriangles ; i++) {
            file >> numVertex >> v1 >> v2 >> v3;
            if (numVertex == 3) {
                shape_car[0+(i*3)] = coordinates_car[v1];
                shape_car[1+(i*3)] = coordinates_car[v2];
                shape_car[2+(i*3)] = coordinates_car[v3];
            }
        }
    }
    file.close();
    delete coordinates_car, coordinates_car_tmp;
    numTriangles_car = numTriangles;

    // Reading Sphere.off to generate shape
    file.open("input_files/sphere.off");
    point4 *coordinates_sphere;
    point4 *shape_sphere;

    file >> input;
    if (input == "OFF") {
        file >> numVerticesOFF >> numTriangles >> edges;
        coordinates_sphere = new point4[numVerticesOFF];
        // storing the coordinates for Car.off
        for (int i = 0 ; i < numVerticesOFF ; i++) {
            file >> x >> y >> z;
            temp = point4(x, y, z, 1.0);
            coordinates_sphere[i] = temp;
        }
        shape_sphere = new point4[numTriangles*3];
        // aligninig the vertices for the object (using previously loaded coordinates)
        for (int i = 0 ; i < numTriangles ; i++) {
            file >> numVertex >> v1 >> v2 >> v3;
            if (numVertex == 3) {
                shape_sphere[0+(i*3)] = coordinates_sphere[v1];
                shape_sphere[1+(i*3)] = coordinates_sphere[v2];
                shape_sphere[2+(i*3)] = coordinates_sphere[v3];
            }
        }
    }
    file.close();
    delete coordinates_sphere;
    numTriangles_sphere = numTriangles;

    // Create a vertex array object
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // Creating buffer to hold 3 objects Cube + Car + Sphere
    glBufferData(GL_ARRAY_BUFFER, (sizeof(cube) + (sizeof(point4)*3*numTriangles_car) +  
                        (sizeof(point4)*3*numTriangles_sphere)), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube), cube);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube), (sizeof(point4)*3*numTriangles_car), shape_car);
    glBufferSubData(GL_ARRAY_BUFFER, (sizeof(point4)*3*numTriangles_car), 
                    (sizeof(point4)*3*numTriangles_sphere), shape_sphere);
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader("shaders/vshader.glsl", "shaders/fshader.glsl");
    
    // vPosition will hold the objects
    vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");
    shaderColor = glGetUniformLocation(program, "shaderColor");
    
    glUseProgram(program);

    mat4  projection;
    projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
    glEnable( GL_DEPTH_TEST );
    
    // Background Color
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

// Function invoked when the size of the window is changed
// This allows the shape to keep its aspect ratio.
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    mat4  projection;
    if(width <= height) {
        projection = Ortho(-1.0, 1.0, -1.0*((GLfloat)height/(GLfloat)width), 1.0*((GLfloat)height/(GLfloat)width), -1.0, 1.0);
    } else {
        projection = Ortho(-1.0*((GLfloat)width/(GLfloat)height), 1.0*((GLfloat)width/(GLfloat)height), -1.0, 1.0, -1.0, 1.0);
    }
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
    glUniform4fv(shaderColor, 1, shapeColor);
}


//----------------------------------------------------------------------------

// This function displays the current object in the animation screen
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Generate tha model-view matrix
    const vec3 displacement( 0.0, 0.0, 0.0 );
    mat4  model_view = ( Scale(FACTOR, FACTOR, FACTOR) * Translate( displacement ) * 
            RotateX( Theta[Xaxis] ) *
            RotateY( Theta[Yaxis] ) *
            RotateZ( Theta[Zaxis] ) );  // Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    glUniform4fv(shaderColor, 1, shapeColor);
    glDrawArrays(GL_TRIANGLES, 0, numVerticesInUse);   
    glutSwapBuffers();   
}

//----------------------------------------------------------------------------

// Switches between Wireframe and Solid modes
void wireFrame_Solid(int option) {
    if (option == 4) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
    } else if (option == 5) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

//----------------------------------------------------------------------------

// Changes the rotation of the object to be rendered
void rotationEnable(int option) {
    if (option == 15) {
        Theta[Xaxis] += 0.0;
        Theta[Yaxis] += 30.0;
        Theta[Zaxis] += 40.0;
    } else if (option == 16) {
        Theta[Xaxis] = 0.0;
        Theta[Yaxis] = 0.0;
        Theta[Zaxis] = 0.0;
    } else if (option == 20) {
        Theta[Xaxis] -= 0.0;
        Theta[Yaxis] -= 30.0;
        Theta[Zaxis] -= 40.0;
    }
}

//----------------------------------------------------------------------------

// HeartBeat function acting as idle function
void heartBeat( void ){
    if (SCALE == false) {
        FACTOR += GROWING_RATE;
    } else {
        FACTOR -= GROWING_RATE;
    }

    if (FACTOR >= 1.0) {
        SCALE = true;
    } else if (FACTOR <= 0.1) {
        SCALE = false;
    }
    glutPostRedisplay();
}


//----------------------------------------------------------------------------

// This function controls the speed up and speed down of the animation 
void arrowKeys(int key, int x, int y) {
    if (key == GLUT_KEY_DOWN) {
        GROWING_RATE = GROWING_RATE/2;
        if(GROWING_RATE <= GROWING_FACTOR_MIN) {
            GROWING_RATE = GROWING_FACTOR_MIN;
        }
    } else if (key == GLUT_KEY_UP) {
        GROWING_RATE = GROWING_RATE*2;             
        if(GROWING_RATE >= GROWING_FACTOR_MAX) {
            GROWING_RATE = GROWING_FACTOR_MAX;
        }
    }  
}

//----------------------------------------------------------------------------

// Controls the color of the object to be drawn
void colorMenu(int option){
    if (option == 6) {
        shapeColor = vertex_colors[BLACK];  
    } else if (option == 7) {
        shapeColor = vertex_colors[RED];  
    } else if (option == 8) {
        shapeColor = vertex_colors[YELLOW];  
    } else if (option == 9) {
        shapeColor = vertex_colors[GREEN];  
    } else if (option == 10) {
        shapeColor = vertex_colors[BLUE];  
    } else if (option == 11) {
        shapeColor = vertex_colors[MAGENTA];  
    } else if (option == 12) {
        shapeColor = vertex_colors[WHITE];  
    } else if (option == 13) {
        shapeColor = vertex_colors[CYAN];  
    } else if (option == 14) {
        shapeColor = vertex_colors[PINK];
    }
}

//----------------------------------------------------------------------------

// Initializes the menu
void mainMenu(int option) {
}

//----------------------------------------------------------------------------

// This function switches between buffers so that the display function 
// can switch between objects to be drawn on the animation window
void shapeToDraw(int option) {
    if (option == 1) {              // Cube
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        numVerticesInUse = NumVertices;
    } else if (option == 2) {       // Car
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube)));
        numVerticesInUse = numTriangles_car*3;
    } else if (option == 3) {       // Sphere
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*3*numTriangles_car));
        numVerticesInUse = numTriangles_sphere*3;
    }
}

//----------------------------------------------------------------------------

// Prints the help menu
void help(void) {
    printf("***************************   COMP510: Computer Graphics   ****************************\n");
    printf("***************************  By: Mohammad Laghari (0059487)  **************************\n");
    printf("Right click to access popup menu\n");
    printf("From the options, you can select\n");
    printf("  1. Object to render (Cube, Car, Sphere)\n");
    printf("  2. Mode in which object is to be drawn (Wireframe or Solid)\n");
    printf("  3. Which color to render object in\n");
    printf("  4. Rotate the object (30 degrees in Yaxis and 40 degrees in Zaxis at a time, about Xaxis)\n");
    printf("  5. Change the background color (Black, White and Gray)\n");
    printf("Press i or I key to go to the initial position\n");
    printf("Press h or H key for help\n");
    printf("Press q or Q key to exit application\n\n");
    printf("**************************************************************************************\n");
}

//----------------------------------------------------------------------------

// Changes the color of the background using glClearColor()
void backgroundColor(int option) {
    if (option == 17) {         // Black
        glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    } else if (option == 18) {  // White
        glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
    } else if (option == 19) {  // Gray
        glClearColor( 0.5, 0.5, 0.5, 1.0 ); 
    }
}

//----------------------------------------------------------------------------

// Call-backs for the RightClick Menu
void rightClick() {
    rightClickMenu = glutCreateMenu(shapeToDraw);
    glutAddMenuEntry("Cube",1);
    glutAddMenuEntry("Car",2);
    glutAddMenuEntry("Sphere",3);

    drawingMode = glutCreateMenu(wireFrame_Solid);
    glutAddMenuEntry("Wireframe",4);
    glutAddMenuEntry("Solid",5);

    colorSelector = glutCreateMenu(colorMenu);
    glutAddMenuEntry("Red",7);
    glutAddMenuEntry("Black",6);
    glutAddMenuEntry("Yellow",8);
    glutAddMenuEntry("Green",9);     
    glutAddMenuEntry("Blue",10); 
    glutAddMenuEntry("Magenta",11);   
    glutAddMenuEntry("White",12);     
    glutAddMenuEntry("Cyan",13);   
    glutAddMenuEntry("Pink",14);

    rotateMode = glutCreateMenu(rotationEnable);
    glutAddMenuEntry("Rotate Clockwise",15);
    glutAddMenuEntry("Rotate Anti-Clockwise",20);
    glutAddMenuEntry("Disable",16);

    background = glutCreateMenu(backgroundColor);
    glutAddMenuEntry("Black",17);
    glutAddMenuEntry("White",18);
    glutAddMenuEntry("Gray",19);


    mainMenuOpener = glutCreateMenu(mainMenu);  
    glutAddSubMenu("Pick Object",rightClickMenu);  
    glutAddSubMenu("Pick Mode", drawingMode);
    glutAddSubMenu("Pick Color", colorSelector);
    glutAddSubMenu("Rotation", rotateMode);
    glutAddSubMenu("Background Color", background);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//----------------------------------------------------------------------------

// Initializes the scene back to the original position where the animation began
void initialize(void) {
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    numVerticesInUse = NumVertices;
    shapeColor = vertex_colors[RED];
    GROWING_RATE = 0.1;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    Theta[Xaxis] = 0.0;
    Theta[Yaxis] = 0.0;
    Theta[Zaxis] = 0.0;
    glClearColor(1.0, 1.0, 1.0, 1.0); 
}

//----------------------------------------------------------------------------

// Callbacks for keyboard shortcuts (q,i,h)
void keyboard( unsigned char key,int x, int y ) {
    if (key == 'q' | key == 'Q') {
        exit(0);
    } else if (key == 'i' | key == 'I') {
        initialize();
    } else if (key == 'h' | key == 'H') {
        help();
    }
}

//----------------------------------------------------------------------------

int main( int argc, char **argv ) {
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("COMP510 Computer Graphics: Assignment 1");
    help();
    rightClick();
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)); 
    glewExperimental = GL_TRUE;
    if(glewInit()) {
      printf("GLEW error");
      exit(0);
    }
    init();

    glutDisplayFunc(display);
    glutSpecialFunc(arrowKeys);
    glutReshapeFunc(reshape);
    glutIdleFunc(heartBeat);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}

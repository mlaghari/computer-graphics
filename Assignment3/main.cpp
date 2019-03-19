//  COMP510: Computer Graphics
//  Assignment 3
//  Programmed by Mohammad Laghari (0059487)
//  Email: mlaghari16@ku.edu.tr
//  Instructor: Dr. Yucel Yemez

#include <fstream> 
#include <iostream>

#include "InitShader.cpp"

using namespace std;

int WINDOW_WIDTH = 512;
int WINDOW_HEIGHT = 512;
double SCALE_FACTOR = 1.0;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef Angel::vec2  point2;

// For right click menu
GLint menuProjectionMode, menuDrawingMode, menuBackColor, menuShading, menuPhong;
GLint menuLightMovement, menuLightSwitch, menuMaterialType, menuLightColor;
enum {OrthographicProjectionMode = 0, PerspectiveProjectionMode = 1};
enum {Off = 0, On = 1};
enum {Wireframe = 0, Gouraud = 1, Phong = 2, ModifiedPhong = 3, Texture = 4};
enum {Plastic = 5, Metallic = 10};
enum {LightNotMoving = 0, LightMoving = 1};
enum {PointLightOn = 0, PointLightOff = 1};
enum {DirectionalLightOn = 2, DirectionalLightOff = 3};

int PROJECTION_TYPE_GLOBAL = OrthographicProjectionMode;
int DRAWING_MODE_GLOBAL = Gouraud;
int SECONDARY_MODE_GLOBAL = Off;
int Zclose = 1, Zaway = 5;
vec3 displacement( 0, 0, 0 );

GLdouble xA = -1, yA = 1, zA = -1;
GLdouble xB = 1, yB = -1, zB = 1;

double rotX = 0.0, rotY = 0.0, rotZ = 0.0;

// Axes details
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int Axis = Xaxis;
GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };

GLuint program, vPosition, vNormal, vCoords;
GLuint ModelView, ModelView2, Projection;
GLuint DrawingMode, Secondary_Mode;
GLuint AmbientProduct, DiffuseProduct, SpecularProduct, Light1Position, Light2Position, Shininess;
GLuint LightToggle1, LightToggle2, customTexture;

// variables for loading from OFFX file
int numVerticesOFF, numTriangles;
point4 *vertices, *faceVertex;

point2 *texture, *v_coords;
vec3 *normal, *faceNormal;
int *faceIndex;

// Sizes of texture and texture location
int sizeX, sizeY;
GLubyte *textureImage;

point4 LightPosition1( -1.0, 1.0, 0.0, 1.0 );
point4 LightPosition2( -1.0*-1.0, 1.0*-1.0, 0.0+1.0, 0.0 );
color4 LightColor( 1.0, 1.0, 1.0, 1.0 );

int LightsMovement = LightNotMoving, LightIsOn1 = 1, LightIsOn2 = 1;

color4 material_ambient( 0.1, 0.1, 0.1, 1.0 );
color4 material_diffuse( 0.4, 0.4, 0.4, 1.0 );
color4 material_specular( 1.0, 1.0, 1.0, 1.0 );
float  material_shininess = Metallic;

color4 ambientP = LightColor * material_ambient;
color4 diffuseP = LightColor * material_diffuse;
color4 specularP = LightColor * material_specular;

//----------------------------------------------------------------------------

// OpenGL initialization
void init() {

    // Loading data from file
    ifstream file;
    file.open("shapeX.offx");

    string input;
    int edges;
    file >> input;
    if (input == "OFFX") {
        file >> numVerticesOFF >> numTriangles >> edges;
        vertices = new point4[numVerticesOFF];
        texture = new point2[numVerticesOFF];
        normal = new vec3[numVerticesOFF];

        faceIndex = new int[numTriangles*3];
        faceVertex = new point4[numTriangles*3];
        faceNormal = new vec3[numTriangles*3];
        v_coords = new point2[numTriangles*3];

        GLdouble x, y, z;
        for(int i = 0 ; i < numVerticesOFF ; i++) {
            file >> x >> y >> z;
            vertices[i] = point4(x, y, z, 1.0);
        }

        int v1, v2, v3;
        for(int j = 0 ; j < numTriangles ; j++) {
            file >> edges >> v1 >> v2 >> v3;

            faceIndex[(j*3)+0] = v1;
            faceIndex[(j*3)+1] = v2;
            faceIndex[(j*3)+2] = v3;

            faceVertex[(j*3)+0] = vertices[v1];
            faceVertex[(j*3)+1] = vertices[v2];
            faceVertex[(j*3)+2] = vertices[v3];
        }

        for(int i = 0 ; i < numVerticesOFF ; i++) {
            file >> input >> x >> y;
            texture[i] = point2(x, y);
        }

        for(int i = 0 ; i < numVerticesOFF ; i++) {
            file >> input >> x >> y >> z;
            normal[i] = vec3(x, y, z);
        }

        for(int j = 0 ; j < numTriangles*3 ; j++) {
            faceNormal[j] = normal[faceIndex[j]];
            v_coords[j] = texture[faceIndex[j]];
        }
    }
    file.close();


    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData( GL_ARRAY_BUFFER, (sizeof(point4)*3*numTriangles)+(sizeof(vec3)*3*numTriangles)+(sizeof(point2)*3*numTriangles), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*3*numTriangles, faceVertex );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*3*numTriangles, sizeof(vec3)*3*numTriangles, faceNormal );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*3*numTriangles+sizeof(vec3)*3*numTriangles, sizeof(point2)*3*numTriangles, v_coords );


    program = InitShader("shaders/vshader.glsl", "shaders/fshader.glsl");
    glUseProgram(program);

    vPosition = glGetAttribLocation(program, "vPosition");
    vNormal = glGetAttribLocation(program, "vNormal");
    vCoords = glGetAttribLocation(program, "vCoords");
    AmbientProduct = glGetUniformLocation(program, "AmbientProduct");
    DiffuseProduct = glGetUniformLocation(program, "DiffuseProduct");
    SpecularProduct = glGetUniformLocation(program, "SpecularProduct");
    Light1Position = glGetUniformLocation(program, "Light1Position");
    Light2Position = glGetUniformLocation(program, "Light2Position");
    LightToggle1 = glGetUniformLocation(program, "L1_Toggle");
    LightToggle2 = glGetUniformLocation(program, "L2_Toggle");
    Shininess = glGetUniformLocation(program, "Shininess");
    DrawingMode = glGetUniformLocation(program, "DRAWING_MODE_GLOBAL");
    Secondary_Mode = glGetUniformLocation(program, "SECONDARY_MODE_GLOBAL");
    
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*3*numTriangles));
    glEnableVertexAttribArray(vCoords);
    glVertexAttribPointer(vCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*3*numTriangles+sizeof(vec3)*3*numTriangles));
    glUniform4fv(AmbientProduct, 1, ambientP);
    glUniform4fv(DiffuseProduct, 1, diffuseP);
    glUniform4fv(SpecularProduct, 1, specularP);
    glUniform4fv(Light1Position, 1, LightPosition1);
    glUniform4fv(Light2Position, 1, LightPosition2);
    glUniform1i(LightToggle1, LightIsOn1);
    glUniform1i(LightToggle2, LightIsOn2);
    glUniform1f(Shininess, material_shininess);
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glUniform1i(Secondary_Mode, SECONDARY_MODE_GLOBAL);

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "ModelView");
    ModelView2 = glGetUniformLocation(program, "ModelView2");
    Projection = glGetUniformLocation(program, "Projection");


    glGenTextures(1, &customTexture );
    glBindTexture(GL_TEXTURE_2D, customTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);


    glEnable(GL_DEPTH_TEST);		
    glEnable(GL_CULL_FACE); 

    // Default background
    glClearColor(1.0, 1.0, 1.0, 1.0);
   
}

//----------------------------------------------------------------------------

// Drawing modes toggle
// void

//----------------------------------------------------------------------------

// Light Movement;
void moveLight(int offset, int axis) {
    if (axis == Xaxis) {
        LightPosition1 = RotateX(offset) * LightPosition1;
        LightPosition2 = RotateX(offset) * LightPosition2;
        glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);       
        glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);
    } else if (axis == Yaxis) {
        LightPosition1 = RotateY(offset) * LightPosition1;
        LightPosition2 = RotateY(offset) * LightPosition2;
        glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);       
        glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);
    } else if (axis == Zaxis) {
        LightPosition1 = RotateZ(offset) * LightPosition1;
        LightPosition2 = RotateZ(offset) * LightPosition2;
        glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);       
        glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);
    }
}

//----------------------------------------------------------------------------

// This function displays the current object in the animation screen
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 model_view = (Translate( displacement ) *
                        Scale(1.0*SCALE_FACTOR,1.0*SCALE_FACTOR,1.0*SCALE_FACTOR) *			 
                        RotateX( Theta[Xaxis] + rotX ) *
                        RotateY( Theta[Yaxis] + rotY ) *
                        RotateZ( Theta[Zaxis] + rotZ )  );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    mat4  model_viewl = (Scale(1.0*SCALE_FACTOR,1.0*SCALE_FACTOR,1.0*SCALE_FACTOR) *			 
                        RotateX( Theta[Xaxis] + rotX ) *
                        RotateY( Theta[Yaxis] + rotY ) *
                        RotateZ( Theta[Zaxis] + rotZ )  );

    glUniformMatrix4fv( ModelView2, 1, GL_TRUE, model_viewl );


    if (DRAWING_MODE_GLOBAL == Wireframe) {
        for(int i = 0 ; i < numTriangles*3 ; i+=3) {
            glDrawArrays(GL_LINE_LOOP, i, 3);
        }
    } else if (DRAWING_MODE_GLOBAL == Gouraud) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    } else if (DRAWING_MODE_GLOBAL == Phong) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    } else if (DRAWING_MODE_GLOBAL == ModifiedPhong) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    } else if (DRAWING_MODE_GLOBAL == Texture) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    }

    glutSwapBuffers();	
}

//----------------------------------------------------------------------------

// Prints the help menu
void help() {
    printf("***************************   COMP510: Computer Graphics   ****************************\n");
    printf("***************************  By: Mohammad Laghari (0059487)  **************************\n");
    printf("Right click to access popup menu\n");
    printf("Press i key to go to the initial position\n");
    printf("Press z or Z key for zoom in and zoom out\n");
    printf("Press up or down arrow keys for rotation in x-axis\n");
    printf("Press a or s keys for rotation in y-axis\n");
    printf("Press left or right arrow keys for rotation in z-axis\n");
    printf("Press b, g or w for Black, Grey or White background color respectively\n");
    printf("Press h or H key for help\n");
    printf("Press q or Q key to exit application\n\n");
    printf("**************************************************************************************\n");
}

//----------------------------------------------------------------------------

// Handles the arrow key functionality
void rotationKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        rotZ += 2;
        if(LightsMovement == LightMoving) {
            moveLight(2, Zaxis);
        }
    } else if (key == GLUT_KEY_RIGHT) {
        rotZ -= 2;
        if(LightsMovement == LightMoving) {
            moveLight(-2, Zaxis);
        }
    } else if (key == GLUT_KEY_UP) {
        rotX += 2;
        if(LightsMovement == LightMoving) {
            moveLight(2, Xaxis);
        }
    } else if (key == GLUT_KEY_DOWN) {
        rotX -= 2;
        if(LightsMovement == LightMoving) {
            moveLight(-2, Xaxis);
        }
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Handles the keyboard keys functionality
void keyboard(unsigned char key, int x, int y) {
    if (key == 'z') {
        SCALE_FACTOR -= SCALE_FACTOR*0.1;
    } else if (key == 'Z') {
        SCALE_FACTOR += SCALE_FACTOR*0.1;
    } else if (key == 'i') {
        rotX = 0.0;
        rotY = 0.0;
        rotZ = 0.0;         
        SCALE_FACTOR = 1.0;
    } else if (key == 'b') {
        glClearColor(0.0, 0.0, 0.0, 1.0);
    } else if (key == 'w') {
        glClearColor(1.0, 1.0, 1.0, 1.0);
    } else if (key == 'g') {
        glClearColor(0.5, 0.5, 0.5, 1.0);
    } else if (key == 'h') {
        help();
    } else if (key == 'q') {
        exit(0);
    } else if (key == 'a') {
        rotY -= 2;
        if(LightsMovement == LightMoving) {
            moveLight(-2, Yaxis);
        }
    } else if (key == 's') {
        rotY += 2;
        if (LightsMovement == LightMoving) {
            moveLight(2, Yaxis);
        }
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Read the texture file provided as texture.ppm in input_files
void readPPM(void) {
    FILE *fd;
    int k, n, m, t;
    char c;
    char b[100];
    float s;
    int red, green, blue;

    fd = fopen("texture.ppm", "r");
    t = fscanf(fd, "%[^\n] ", b);

    if (b[0] != 'P'|| b[1] != '3') {
        exit(0);
    }

    t = fscanf(fd, "%c", &c);
    while(c == '#') {
        t = fscanf(fd, "%[^\n] ", b);
        printf("%s\n", b);
        t = fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);

    t = fscanf(fd, "%d %d %d", &n, &m, &k);
    sizeX = n;
    sizeY = m;

    textureImage = new GLubyte[sizeX*sizeY*3];

    for (int i = n-1 ; i >= 0 ; i--) {
        for (int j = 0 ; j < m ; j++) {
            t = fscanf(fd, "%d %d %d", &red, &green, &blue);
            textureImage[(i*sizeY*3)+(j*3)+0] = (GLubyte)red;
            textureImage[(i*sizeY*3)+(j*3)+1] = (GLubyte)green;
            textureImage[(i*sizeY*3)+(j*3)+2] = (GLubyte)blue;
        }
    }

    fclose(fd);
}

//----------------------------------------------------------------------------

// Callbacks for main menu
void menuMain(int id) {
}

//----------------------------------------------------------------------------

// Toggles the material type between Metallic and Plastic of the object
void materialType(int id) {
    material_shininess = id;
    glUniform1f( Shininess, material_shininess );

    if (id == Metallic) {
        material_ambient = color4( 0.1, 0.1, 0.1, 1.0 );
        material_diffuse = color4( 0.4, 0.4, 0.4, 1.0 );
        material_specular = color4( 0.7, 0.7, 0.7, 1.0 );
    } else if (id == Plastic) {
        material_ambient = color4( 0.01, 0.01, 0.01, 1.0 );
        material_diffuse = color4( 0.4, 0.4, 0.4, 1.0 );
        material_specular = color4( 0.5, 0.5, 0.5, 1.0 );
    }

    ambientP = LightColor * material_ambient;
    diffuseP = LightColor * material_diffuse;
    specularP = LightColor * material_specular;

    glUniform4fv(AmbientProduct, 1, ambientP);
    glUniform4fv(DiffuseProduct, 1, diffuseP);    
    glUniform4fv(SpecularProduct, 1, specularP);

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Toggles the material type between Phong Reflection and Modified Phong Reflection
void phongShadingToggle(int id) {
    DRAWING_MODE_GLOBAL = id;
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Toggles the shading option between Phong Shading and Gouraud Shading
void shadingToggle(int id) {
    DRAWING_MODE_GLOBAL = id;
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Toggles the projection type between Orthographic and Perspective
void projectionType(int id) {
    PROJECTION_TYPE_GLOBAL = id;

    mat4 projection;
    GLfloat aspect = GLfloat(WINDOW_WIDTH)/WINDOW_HEIGHT;
    if(PROJECTION_TYPE_GLOBAL != OrthographicProjectionMode) {
        displacement = vec3(0, 0, -2.5);
        projection = Perspective(45, aspect, Zclose, Zaway);
    } else {
        displacement = vec3(0,0,0);
        if(WINDOW_WIDTH > WINDOW_HEIGHT) {
            projection = Ortho(xA*aspect, yA*aspect, zA, xB, yB, zB);
        } else {
            projection = Ortho(xA, yA, zA/aspect, xB/aspect, yB, zB);
        }
    }

    glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);  	  
    glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);

    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Toggles the light being fixed or moving with object
void lightProperty(int id) {
    LightsMovement = id;      
}

//----------------------------------------------------------------------------

// Toggles light on or off in point and directional light
void lightSwitch(int id) {
    if (id == DirectionalLightOn) {
        LightIsOn2 = 1;
    } else if (id == DirectionalLightOff) {
        LightIsOn2 = 0;
    } else if (id == PointLightOn) {
        LightIsOn1 = 1;
    } else if (id == PointLightOff) {
        LightIsOn1 = 0;
    }

    glUniform1i(LightToggle1, LightIsOn1);
    glUniform1i(LightToggle2, LightIsOn2);
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Selects between Wireframe, Shading and Texture
void drawingMode(int id) {
    DRAWING_MODE_GLOBAL = id;
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Function invoked when the size of the window is changed
// This allows the shape to keep its aspect ratio.
void reshape( int width, int height ) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, width, height);
    mat4 projection;
    GLfloat aspect = GLfloat(width)/height;
    if(PROJECTION_TYPE_GLOBAL != OrthographicProjectionMode) {
        projection = Perspective(45, aspect, Zclose, Zaway);
    } else {
        if(width <= height) {
            projection = Ortho(xA, yA, zA/aspect, xB/aspect, yB, zB);
        } else {
            projection = Ortho(xA*aspect, yA*aspect, zA, xB, yB, zB);
        }
    }

    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}


//----------------------------------------------------------------------------

// Call-backs for the RightClick Menu
void rightClick() {
    menuProjectionMode = glutCreateMenu(projectionType);
    glutAddMenuEntry("Orthographic" , OrthographicProjectionMode);
    glutAddMenuEntry("Perspective" , PerspectiveProjectionMode);

    menuPhong = glutCreateMenu(phongShadingToggle);
    glutAddMenuEntry("Regular Phong" , Phong);
    glutAddMenuEntry("Modified Phong" , ModifiedPhong);

    menuMaterialType = glutCreateMenu(materialType);
    glutAddMenuEntry("Metallic" , Metallic);
    glutAddMenuEntry("Plastic" , Plastic);

    menuShading = glutCreateMenu(shadingToggle);
    glutAddMenuEntry("Gouraud" , Gouraud);
    glutAddSubMenu("Phong" , menuPhong);

    menuDrawingMode = glutCreateMenu(drawingMode);
    glutAddMenuEntry("Wireframe" , Wireframe);
    glutAddSubMenu("Shading" , menuShading);
    glutAddMenuEntry("Texture" , Texture);

    menuLightMovement = glutCreateMenu(lightProperty);
    glutAddMenuEntry("Move with Object" , LightMoving);
    glutAddMenuEntry("Don't move with Object" , LightNotMoving);

    menuLightSwitch = glutCreateMenu(lightSwitch);
    glutAddMenuEntry("Directional On" , DirectionalLightOn);
    glutAddMenuEntry("Point On" , PointLightOn);
    glutAddMenuEntry("Directional Off" , DirectionalLightOff);
    glutAddMenuEntry("Point Off" , PointLightOff);


    glutCreateMenu(menuMain);
    glutAddSubMenu("Projection Toggle", menuProjectionMode);
    glutAddSubMenu("Drawing Mode Toggle", menuDrawingMode);
    glutAddSubMenu("Light Movement", menuLightMovement);
    glutAddSubMenu("Light Toggle", menuLightSwitch);
    glutAddSubMenu("Material Toggle", menuMaterialType);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//-----------------------------------------------------------------------
// main program
//-----------------------------------------------------------------------

int main(int agrc, char** agrv) {
    readPPM();
    help();
    help();help();help();
    glutInit(&agrc, agrv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("COMP510 Computer Graphics: Assignment 3");

    glutInitContextProfile( GLUT_CORE_PROFILE );
    glewExperimental = GL_TRUE;
    if(glewInit()) {
      printf("GLEW error");
      exit(0);
    }
    rightClick();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(rotationKeys);
    glutReshapeFunc(reshape);
    
    glutMainLoop();
    return 0; 
}

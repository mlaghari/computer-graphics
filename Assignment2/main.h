#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include "InitShader.cpp"

using namespace std;

/* CubeHistory is the class that stores the 
   state of the cube at any point. This helps in
   accessing the history moves made to the Rubik's Cube.
   */
class CubeHistory {
	public:
		int Axis;
		int SubCube;
		int RotationDirection;
		CubeHistory(int A, int B, int C) {
			SubCube = A;
			Axis = B;
			RotationDirection = C;
		}
};

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

/* Rubik's Cube specifications */
const int numCubes = 27;
const int cubeFaces = 6;
const int faceVertices = 6;
const int numCubeVertices = faceVertices*cubeFaces*numCubes;

/* Animation Speed */
const int animationSpeed = 10;

/* Vector to store the history of moves */
vector<CubeHistory> cubeMoves;   

/* Default position of the cube */
double rotateX = 135.0;
double rotateZ = -45.0;
double scaleFactor = 1.0;

enum { axisX = 0, axisY = 1, axisZ = 2, numAxis = 3 };
float Theta[numAxis] = { 0.0, 0.0, 0.0 };

mat4 rotationMatrix[numCubes];
color4 colorSelector[numCubes];
color4 colors[numCubeVertices];
point4 vertices[numCubeVertices];


/* Cube position */
int currCubePos[3][3][3];
int futureCubePos[3][3][3];

/* Cube rotation-level variables; specify the current orientation of the cube */
int rotationAxis = axisY; 
int currentBlock = 1;
int rotatePhase = 0;

/* Uniform location*/
int ModelView;
int Projection;
int edge;
int currentCube;
int vRotation[numCubes];
/*Variable for the shader*/
int selectCube;
int selectFace;
int cubeColor[numCubes];


int cubeSelector(int x, int y, int button);
int performRotation(int currX, int currY, int currZ, int nextX, int nextY, int nextZ, int key);
int rotateAlong(int axis, int kj, int lj, int button);
void cubeCreator(point4 *cube, color4 *color);
void cubeTranslation(point4 *cube, vec3 displacement);
void rubiksCube(void);
void randomInitialization(int numRotations);
void rubiksCubeRotation(int direction, int block, int axis);
void backgroundColor(int option);
void initialState();
void previousMove();

//-----------------------------------------------------------------------------

/* Cube vertices and color */
void cubeCreator(point4 *cube, color4 *color) {
	point4 vertices[8];

	vertices[0] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[6] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	for (int i = 0 ; i < 6 ; i++) {
		color[i] = color4(1.0,0.0,0.0,1.0);
		color[i+6] = color4(1.0,1.0,0.0,1.0);
		color[i+12] = color4(0.0,1.0,0.0,1.0);
		color[i+18] = color4(0.0,1.0,1.0,1.0);
		color[i+24] = color4(0.0,0.0,1.0,1.0);
		color[i+30] = color4(1.0,0.0,1.0,1.0);
	}

	cube[0] = vertices[0];
	cube[1] = vertices[1];
	cube[2] = vertices[2];
	cube[3] = vertices[2];
	cube[4] = vertices[3];
	cube[5] = vertices[0];

	cube[6] = vertices[4];
	cube[7] = vertices[5];
	cube[8] = vertices[6];
	cube[9] = vertices[6];
	cube[10] = vertices[7];
	cube[11] = vertices[4];

	cube[12] = vertices[0];
	cube[13] = vertices[3];
	cube[14] = vertices[7];
	cube[15] = vertices[7];
	cube[16] = vertices[4];
	cube[17] = vertices[0];

	cube[18] = vertices[1];
	cube[19] = vertices[2];
	cube[20] = vertices[6];
	cube[21] = vertices[6];
	cube[22] = vertices[5];
	cube[23] = vertices[1];

	cube[24] = vertices[0];
	cube[25] = vertices[1];
	cube[26] = vertices[5];
	cube[27] = vertices[5];
	cube[28] = vertices[4];
	cube[29] = vertices[0];

	cube[30] = vertices[3];
	cube[31] = vertices[2];
	cube[32] = vertices[6];
	cube[33] = vertices[6];
	cube[34] = vertices[7];
	cube[35] = vertices[3];
}
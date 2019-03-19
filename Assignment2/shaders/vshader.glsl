#version 130

in vec4 vPosition, vColor;

uniform mat4 ModelView, Projection;

uniform mat4 vRotation[27];
uniform vec4 cubeColor[27];
uniform int currentCube, edge, selectFace, selectCube;

out vec4 color;

void positionChanger() {
    if(selectFace == 1) {
        gl_Position = Projection * ModelView * vPosition;
    } else {
        gl_Position = Projection * ModelView * vRotation[currentCube] * vPosition;
    }
}

void colorChanger() {
    if (selectCube == 1) {
        color = cubeColor[currentCube];
    } else if(edge == 1) {
        color = vec4(0.0,0.0,0.0,1.0);
    } else {
        color = vColor;
    }
}

void main()  {
    positionChanger();
    colorChanger();    
} 

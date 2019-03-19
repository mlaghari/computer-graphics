#version 130

in vec4 vPosition;
uniform vec4 shaderColor;
out vec4 color;

uniform mat4 ModelView;
uniform mat4 Projection;

void main() {
    gl_Position = Projection * ModelView * vPosition;
    color = shaderColor;
}

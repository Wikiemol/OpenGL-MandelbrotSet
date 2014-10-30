#version 130

in vec2 position;
in vec2 color;

out vec2 fragPosition;
void main() { 
    gl_Position.xy = position;
    gl_Position.z = 0;
    gl_Position.w = 1;

    fragPosition = position;
}

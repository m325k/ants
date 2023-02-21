in vec3 vertex;

smooth out vec4 color;
smooth out vec2 texCoord;
    
void main(void) {
    gl_Position = vec4(0.5, 0.5, 0.5, 1) * vec4(vertex, 1.0);
    color = gl_Color;
    texCoord = gl_MultiTexCoord0.xy;
}
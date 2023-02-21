out vec4 resultColor;
uniform sampler2D image;
smooth in vec4 color;
smooth in vec2 texCoord;

void main(void) {
    vec4 col = texture(image, texCoord);
    col.z = 0.0;
    resultColor = col;//vec4(0, oracle, 1 - oracle, 1);
}
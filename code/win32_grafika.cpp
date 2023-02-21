#include <windows.h>
#include <gl/GL.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <string>
#include "opengl_defs.cpp"

#define internal static
#define global_variable static

//#define DEBUG_FPS

struct vec2 {
    float x;
    float y;
};

struct vec4 {
    float x;
    float y;
    float z;
    float a;
};

global_variable const int textureWidth = 2000, textureHeight = 1000;
global_variable const int NUM_OF_AGENTS = 100000;
global_variable vec2 positions[NUM_OF_AGENTS];
global_variable vec2 velocities[NUM_OF_AGENTS];
global_variable vec4 trail_map[textureWidth * textureHeight];
GLfloat emptyData[textureWidth * textureHeight * sizeof(GLfloat)] = { 0 };
global_variable GLuint ssboPosition;
global_variable GLuint ssboVelocity;
global_variable GLuint ssboTrail;

static bool running;
static int BitmapWidth;
static int BitmapHeight;
static BITMAPINFO BitmapInfo;
static HBITMAP Bitmap;
static void* BitmapMemory;
static double t;
static bool reverse;
global_variable GLuint shaderProgramID;
global_variable GLuint computeShaderProgramID;
global_variable GLuint trailShaderProgramID;
global_variable bool isGLinitialized = false;
global_variable GLint oracleLocation;
global_variable GLuint textureID;


void Resize(int width, int height) {
    BitmapWidth = width;
    BitmapHeight = height;

    if(BitmapMemory) {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;


    int size = BitmapWidth * BitmapHeight * 4;
    BitmapMemory = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
}


std::vector<vec2> GetBlackPixels(const std::string& path) {
    // Open the image file
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        // Error opening file
        // Handle the error here
        return {};
    }

    // Read the BMP file header to determine the width, height, and number of channels
    int width, height, bits;
    file.seekg(18);
    file.read((char*)&width, sizeof(int));
    file.read((char*)&height, sizeof(int));
    file.seekg(28);
    file.read((char*)&bits, sizeof(int));

    // Allocate a buffer to store the pixel data
    unsigned char* data = (unsigned char*)malloc(width * height * bits);
    if (data == NULL) {
        // Error allocating buffer
        // Handle the error here
        file.close();
        return {};
    }

    // Read the pixel data into the buffer
    file.seekg(54);
    file.read((char*)data, width * height * bits);

    // Create a vector to store the black pixels
    std::vector<vec2> black_pixels;

    int channels = bits / 8;

    int padding = (4 - width % 4) % 4;
    // Iterate over the pixels in the image and check for black pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate the index of the current pixel in the data array
            int index = (y * (width + padding) + x) * channels;

            // Check if the pixel is black
            if (data[index] == 0 && data[index + 1] == 0 && data[index + 2] == 0) {
                // Pixel is black, add its x and y coordinates to the vector
                black_pixels.push_back({ (float)x / width, (float)y / height });
            }
        }
    }

    // Free the image data
    free(data);

    // Close the file
    file.close();

    // Return the vector of black pixels
    return black_pixels;
}

void InitGame(unsigned int time) {
    srand(time);

    //anchors
    std::vector<vec2> black_pixels = GetBlackPixels("../res/image.bmp");
    int cutoff = black_pixels.size();
    for (int i = 0; i < cutoff;i++) {
        positions[i] = black_pixels[i];
        velocities[i] = { 0, 0 };
    }

    //free
    for (int i = cutoff; i < NUM_OF_AGENTS; i++) {
        positions[i] = {rand() / (float) RAND_MAX, rand() / (float)RAND_MAX};
        velocities[i] = { rand() / (float)RAND_MAX / 200 + 0.0001f, rand() / (float)RAND_MAX / 200 + 0.0001f};
    }
}

void UpdateGame(double dt) {
    if (reverse) {
        t -= 2 * dt;
    }
    else {
        t += 2 * dt;
        return;
    }

    if(isGLinitialized) {
        //CLEAR TEXTURE
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, &emptyData[0]);
        
        // COMPUTE SHADER
        glUseProgram(computeShaderProgramID);
        glUniform1f(oracleLocation, (float)t);
        glDispatchCompute((GLuint) NUM_OF_AGENTS, (GLuint) 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        // TRAIL SHADER
        glUseProgram(trailShaderProgramID);
        glDispatchCompute((GLuint) textureWidth, (GLuint) textureHeight, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}

void DisplayWindow(HDC deviceContext) {
    if(isGLinitialized) {
        glUseProgram(shaderProgramID);
    }
    glViewport(0, 0, BitmapWidth, BitmapHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    
    float edge = 1.8f;
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-edge, -edge);

    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(edge, -edge);

    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2d(edge, edge);

    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2d(-edge, edge);
    glEnd();


    SwapBuffers(deviceContext);
}


LRESULT MainWindowCallback(
  HWND window,
  UINT message,
  WPARAM wParam,
  LPARAM lParam
) {
    LRESULT result = 0;
    switch(message) {
        case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            Resize(width, height);
            DisplayWindow(GetDC(window));
        } break;
        case WM_DESTROY:
            running = false;
        break;
        case WM_CLOSE:
            running = false;
        break;
        case WM_ACTIVATEAPP:
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            DisplayWindow(deviceContext);
            EndPaint(window, &paint);
        } break;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            UINT32 VKCode = wParam;

            if(VKCode == VK_MENU) {
                reverse ^= true;
            }
        } break;
        default:
            result = DefWindowProc(window, message, wParam, lParam); 
        break;
    }


    return result;
}

internal void SetClearColor() {
    glClearColor(1.0f, 0.3f, 0.0f, 0.0f);
}

char* ReadFile(const std::string& path) {
    // Declare an ifstream object and open the file
    std::ifstream file(path);
    if (!file.is_open()) {
        // File could not be opened
        // Handle the error here
        return NULL;
    }

    // Read the contents of the file into a string
    std::string contents;
    std::string line;
    while (std::getline(file, line)) {
        contents += line + "\n";
    }
    file.close();

    char* buffer = (char*)malloc(contents.size() + 1);
    if (buffer == NULL) {
        return NULL;
    }

    strcpy(buffer, contents.c_str());

    // Return the buffer as a C-style string
    return buffer;
}

internal void InjectCustomShaders() {
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glGenBuffers(1, &ssboPosition);
    glGenBuffers(1, &ssboVelocity);
    glGenBuffers(1, &ssboTrail);

    // STORAGE BUFFER position
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPosition);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2) * NUM_OF_AGENTS, &positions[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboPosition);

    // STORAGE BUFFER velocity
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboVelocity);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2) * NUM_OF_AGENTS, &velocities[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssboVelocity);

    // STORAGE BUFFER trail
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTrail);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * textureWidth * textureHeight, &trail_map, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboTrail);
        
    char* headerCode = R"325(
    #version 130
    
    )325";

    char* vertexCode = ReadFile("../res/vertex_shader.glsl");
    char* fragmentCode = ReadFile("../res/fragment_shader.glsl");
    char* computeCode = ReadFile("../res/compute_shader.glsl");
    char* trailCode = ReadFile("../res/trail_shader.glsl");

    shaderProgramID = OpenGLCreateShaderProgram(headerCode, vertexCode, fragmentCode);
    computeShaderProgramID = OpenGLCreateComputeShader(computeCode);
    trailShaderProgramID = OpenGLCreateComputeShader(trailCode);
    
    oracleLocation = glGetUniformLocation(computeShaderProgramID, "oracle");

    free(vertexCode);
    free(fragmentCode);
    free(computeCode);
    free(trailCode);
}

int CALLBACK WinMain(
   HINSTANCE instance,
   HINSTANCE prevInstance,
   LPSTR     commandLine,
   int       showCommand
) {
    WNDCLASS WindowClass = {};
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = instance;
    WindowClass.lpszClassName = "GrafikaClass";
    LARGE_INTEGER performanceFreq;
    QueryPerformanceFrequency(&performanceFreq);
    float targetedTime = 1000.0f / 60.0f;
    timeBeginPeriod(1); //set sleep resolution to 1ms
    
    if(RegisterClass(&WindowClass)) {
        HWND window =  CreateWindowEx(
             0,
             WindowClass.lpszClassName,
             "Grafika",
             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
             CW_USEDEFAULT,
             CW_USEDEFAULT,
             CW_USEDEFAULT,
             CW_USEDEFAULT,
             0,
             0,
             instance,
             0
        );

        HDC deviceContext = GetDC(window);

        if(window) {
            LARGE_INTEGER lastTime;
            QueryPerformanceCounter(&lastTime);
            InitGame((unsigned int)lastTime.QuadPart);
            InitOpenGL(window);
            InjectCustomShaders();
            isGLinitialized = true;
            
            SetClearColor();

            BOOL bRet;
            MSG message;
            running = true;

            while(running) {
                bRet = PeekMessage(&message, window, 0, 0, PM_REMOVE);
                if(bRet > 0) {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }

                LARGE_INTEGER currTime;
                QueryPerformanceCounter(&currTime);
                float elapsedTime = 1000.0f * (currTime.QuadPart - lastTime.QuadPart) / (float) performanceFreq.QuadPart;

                while(elapsedTime <= targetedTime) {
                    Sleep(targetedTime - elapsedTime);
                    
                    QueryPerformanceCounter(&currTime);
                    elapsedTime = 1000.0f * (currTime.QuadPart - lastTime.QuadPart) / (float) performanceFreq.QuadPart;
                }

#ifdef DEBUG_FPS
                char buffer[256];
                wsprintf(buffer, "Frame took: %dms\n", (int)elapsedTime);
                OutputDebugString(buffer);
#endif
                UpdateGame(elapsedTime / (double) 1000);
                DisplayWindow(deviceContext);

                lastTime = currTime;
            }
        } else {
            //FAIL
        }
    } else {
        //FAIL
    }
    return 0;
}

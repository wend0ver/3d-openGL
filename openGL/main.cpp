#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <ctime>
#include <cmath>
#include <list>

// Window dimensions
const int windowWidth = 1080;
const int windowHeight = 1080;

// Player size and speed
const float playerSize = 25.0f;
const float playerSpeed = 200.0f;

// Camera position
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 5.0f;

// Camera rot
float camerarotX = 0.0f;
float camerarotY = 0.0f;
float camerarotZ = 5.0f;

// Mesh class
class Mesh {
public:
    std::array<float, 3> location;
    std::array<float, 3> size;

    Mesh(std::array<float, 3> loc, std::array<float, 3> sz) : location(loc), size(sz) {}

    void draw() {
        float x = location[0];
        float y = location[1];
        float z = location[2];
        float width = size[0];
        float height = size[1];
        float depth = size[2];

        glBegin(GL_QUADS);

        // Front face
        glVertex3f(x, y, z);
        glVertex3f(x + width, y, z);
        glVertex3f(x + width, y + height, z);
        glVertex3f(x, y + height, z);

        // Back face
        glVertex3f(x, y, z + depth);
        glVertex3f(x + width, y, z + depth);
        glVertex3f(x + width, y + height, z + depth);
        glVertex3f(x, y + height, z + depth);

        // Top face
        glVertex3f(x, y + height, z);
        glVertex3f(x + width, y + height, z);
        glVertex3f(x + width, y + height, z + depth);
        glVertex3f(x, y + height, z + depth);

        // Bottom face
        glVertex3f(x, y, z);
        glVertex3f(x + width, y, z);
        glVertex3f(x + width, y, z + depth);
        glVertex3f(x, y, z + depth);

        // Right face
        glVertex3f(x + width, y, z);
        glVertex3f(x + width, y + height, z);
        glVertex3f(x + width, y + height, z + depth);
        glVertex3f(x + width, y, z + depth);

        // Left face
        glVertex3f(x, y, z);
        glVertex3f(x, y + height, z);
        glVertex3f(x, y + height, z + depth);
        glVertex3f(x, y, z + depth);

        glEnd();
    }
};

// Where meshes are stored
std::list<Mesh> meshes;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        std::cout << "Mouse position: (" << mouseX << ", " << mouseY << ")" << std::endl;
    }
}

void setPerspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / tan(fov * 3.14159265358979323846f / 360.0f);
    float zDiff = near - far;

    float projection[16] = {
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (far + near) / zDiff, -1,
        0, 0, (2 * far * near) / zDiff, 0
    };

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection);
}

void lookAt(float eyeX, float eyeY, float eyeZ, float rotX, float rotY, float rotZ) {
    // Convert rotation angles to radians
    float pitch = rotX * 3.14159265358979323846f / 180.0f;
    float yaw = rotY * 3.14159265358979323846f / 180.0f;
    float roll = rotZ * 3.14159265358979323846f / 180.0f;

    // Compute forward vector based on yaw and pitch
    float forwardX = cos(yaw) * cos(pitch);
    float forwardY = sin(pitch);
    float forwardZ = sin(yaw) * cos(pitch);

    float forwardLength = sqrt(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
    forwardX /= forwardLength;
    forwardY /= forwardLength;
    forwardZ /= forwardLength;

    // Up vector remains constant as we rotate around Y axis
    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;

    // Compute right vector
    float sideX = forwardY * upZ - forwardZ * upY;
    float sideY = forwardZ * upX - forwardX * upZ;
    float sideZ = forwardX * upY - forwardY * upX;

    float sideLength = sqrt(sideX * sideX + sideY * sideY + sideZ * sideZ);
    sideX /= sideLength;
    sideY /= sideLength;
    sideZ /= sideLength;

    // Recompute up vector
    upX = sideY * forwardZ - sideZ * forwardY;
    upY = sideZ * forwardX - sideX * forwardZ;
    upZ = sideX * forwardY - sideY * forwardX;

    float matrix[16] = { 0 };

    matrix[0] = sideX;
    matrix[4] = sideY;
    matrix[8] = sideZ;

    matrix[1] = upX;
    matrix[5] = upY;
    matrix[9] = upZ;

    matrix[2] = -forwardX;
    matrix[6] = -forwardY;
    matrix[10] = -forwardZ;

    matrix[15] = 1.0f;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matrix);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}


int main()
{
    GLFWwindow* window;

    // Initialize random seed
    srand(static_cast<unsigned int>(time(0)));

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "3D Mesh Rendering", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Set the mouse button callback
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    //   Add To List           Location                  Size
    //meshes.push_back({ { -1.0f, 3.5f, -2.5f }, { 5.0f, 15.0f, 5.0f } });

    // Adds a mesh to the list
    meshes.push_back({ { -1.0f, 3.5f, -2.5f }, { 5.0f, 15.0f, 5.0f } });

    meshes.push_back({ { 0.0f, 0.0f, 0.0f }, { 50.0f, 0.1f, 50.0f } });

    // Initialize time
    double lastTime = glfwGetTime();
    double deltaTime;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        // Calculate delta time
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        /* Render here */
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera position based on WASD keys
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraX += playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraX -= playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraZ += playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraZ -= playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            cameraY += playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            cameraY -= playerSpeed * deltaTime;
        }


        // Update camera position based on WASD keys
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            camerarotX += playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            camerarotX -= playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
            camerarotZ += playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            camerarotZ -= playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
            camerarotY += playerSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            camerarotY -= playerSpeed * deltaTime;
        }

        float renderDistance = 1000.0f;

        // Set the perspective projection
        setPerspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, renderDistance);

        // Set the view transformation based on the camera position
        lookAt(cameraX, cameraY, cameraZ, camerarotX, camerarotY, camerarotZ);


        // Set the color for the mesh
        glColor3f(
            255.0f/ 255.0f,
            0.0f / 255.0f,
            0.0f / 255.0f
        );

        for (auto it = meshes.begin(); it != meshes.end(); ++it) {
            it->draw();
        }


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

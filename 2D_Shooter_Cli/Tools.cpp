#include "Tools.h"

#include "lib.h"

void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
    int32_t max = width > height ? width : height;
    int32_t midX = (width - max) / 2;
    int32_t midY = (height - max) / 2;
    glViewport(midX, midY, max, max);
}

GLFWwindow* init_glfw(
    const uint32_t WIDTH,
    const uint32_t HEIGHT,
    const string title
) {
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        cout << "Failed to initialize GLAD" << endl;
        return NULL;
    }

    int Width, Height;
    glfwGetFramebufferSize(window, &Width, &Height);
    int32_t max = Width > Height ? Width : Height;
    int32_t midX = (Width - max) / 2;
    int32_t midY = (Height - max) / 2;
    glViewport(midX, midY, max, max);

    return window;
}

void make_Vertex_Attribs (
    vector<uint32_t> &VAOs,
    vector<uint32_t> &VBOs,
    vector<uint32_t> &EBOs,
    uint32_t psize,
    uint32_t bsize,
    vector<Chara> players,
    vector<Bullet> bullets,
    vector<vector<float>> vertices,
    vector<vector<uint32_t>> indices
) {
    glGenVertexArrays(psize + bsize, VAOs.data());
    glGenBuffers(psize + bsize, VBOs.data());
    glGenBuffers(psize + bsize, EBOs.data());

    for (int i = 0; i < psize; i++) {
        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(float), vertices[i].data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(uint32_t), indices[i].data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glUseProgram(players[i].get_shaderProgram());
        glBindVertexArray(VAOs[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    for (int i = psize; i < psize + bsize; i++) {
        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(float), vertices[i].data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(uint32_t), indices[i].data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glUseProgram(bullets[i].get_shaderProgram());
        glBindVertexArray(VAOs[i]);
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
    }
}

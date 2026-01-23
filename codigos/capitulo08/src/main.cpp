#include <glad/glad.h>

#include "modules/camera.hpp"
#include "modules/shader.hpp"
#include "modules/stb_image.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <string>

void callback_redimensionamento(GLFWwindow *janela, int largura, int altura);
void processaInput(GLFWwindow *window);
std::string readTextFile(const std::string &file_path);
void callback_mouse(GLFWwindow *janela, double posX, double posY);
void callback_scroll(GLFWwindow *window, double offsetX, double offsetY);
unsigned int loadTexture(char const *path);

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool primeiroInputMouse = true;
float ultimoX = 400, ultimoY = 300;

float dt = 0.0f;
float ultimoTempo = 0.0f;

glm::vec3 luzPos(2.0f, 1.0f, 4.0f);

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(800, 600, "Estado da Arte", NULL, NULL);
  if (!window) {
    std::cout << "Erro ao criar janela do GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Falha ao carregar funções do GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, 800, 600);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetFramebufferSizeCallback(window, callback_redimensionamento);
  glfwSetCursorPosCallback(window, callback_mouse);
  glfwSetScrollCallback(window, callback_scroll);
  glEnable(GL_DEPTH_TEST);

  Shader meuShaderInsano("shaders/vertex.vert", "shaders/fragment.frag");
  Shader shaderLuz("shaders/light.vert", "shaders/light.frag");

  float vertices[] = {
      // posicoes          // normais           // coordenadas de textura
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  unsigned int VBO;
  glGenBuffers(1, &VBO);

  // VAO do cubo com textura
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // VAO do cubo de iluminação
  unsigned int VAOluz;
  glGenVertexArrays(1, &VAOluz);
  glBindVertexArray(VAOluz);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texturas
  unsigned int mapaDifuso = loadTexture("textures/rock_diffuse_map.png");
  unsigned int mapaARM = loadTexture("textures/rock_ARM_map.png");

  meuShaderInsano.use();
  meuShaderInsano.setInt("material.difuso", 0);
  meuShaderInsano.setInt("material.ARM", 1);

  while (!glfwWindowShouldClose(window)) {
    float tempoAtual = glfwGetTime();
    dt = tempoAtual - ultimoTempo;
    ultimoTempo = tempoAtual;
    processaInput(window);

    glClearColor(0.1f, 0.64f, 0.64f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    meuShaderInsano.use();
    meuShaderInsano.setVec3("visaoPos", camera.Posicao);
    meuShaderInsano.setVec3("luz.posicao", luzPos);
    meuShaderInsano.setVec3("luz.ambiente", glm::vec3(0.2f, 0.2f, 0.2f));
    meuShaderInsano.setVec3("luz.difuso", glm::vec3(0.5f, 0.5f, 0.5f));
    meuShaderInsano.setVec3("luz.especular", glm::vec3(1.0f, 1.0f, 1.0f));
    meuShaderInsano.setFloat("material.brilhosidade", 60.0f);

    glm::mat4 model = glm::mat4(1.0);
    model = glm::scale(model, glm::vec3(2.0));
    glm::mat4 view;
    view = camera.MatrizView();
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f,
                                  0.1f, 100.0f);

    meuShaderInsano.setMat4("model", model);
    meuShaderInsano.setMat4("view", view);
    meuShaderInsano.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapaDifuso);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mapaARM);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    shaderLuz.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, luzPos);
    model = glm::scale(model, glm::vec3(0.2f));
    shaderLuz.setMat4("model", model);
    shaderLuz.setMat4("view", view);
    shaderLuz.setMat4("projection", projection);

    glBindVertexArray(VAOluz);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void callback_redimensionamento(GLFWwindow *janela, int largura, int altura) {
  glViewport(0, 0, largura, altura);
}

void processaInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessaTeclado(FRENTE, dt);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessaTeclado(TRAS, dt);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessaTeclado(ESQUERDA, dt);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessaTeclado(DIREITA, dt);
}

std::string readTextFile(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << file_path << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  file.close();

  return buffer.str();
}

void callback_mouse(GLFWwindow *janela, double posXin, double posYin) {
  float posX = static_cast<float>(posXin);
  float posY = static_cast<float>(posYin);

  if (primeiroInputMouse) {
    ultimoX = posX;
    ultimoY = posY;
    primeiroInputMouse = false;
  }

  float offsetX = posX - ultimoX;
  float offsetY = ultimoY - posY;

  ultimoX = posX;
  ultimoY = posY;

  camera.ProcessaMovimentoMouse(offsetX, offsetY);
}

void callback_scroll(GLFWwindow *window, double offsetX, double offsetY) {
  camera.ProcessaScroll(static_cast<float>(offsetY));
}

unsigned int loadTexture(char const *path) {
  unsigned int IDtexura;
  glGenTextures(1, &IDtexura);

  int largura, altura, nrComponentes;
  unsigned char *dados = stbi_load(path, &largura, &altura, &nrComponentes, 0);
  if (dados) {
    GLenum formato;
    if (nrComponentes == 1)
      formato = GL_RED;
    else if (nrComponentes == 3)
      formato = GL_RGB;
    else if (nrComponentes == 4)
      formato = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, IDtexura);
    glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato,
                 GL_UNSIGNED_BYTE, dados);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(dados);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(dados);
  }

  return IDtexura;
}

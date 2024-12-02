#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "camera.hpp"
//#include "ground.hpp"

struct Vertex {
  glm::vec3 position;

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onUpdate() override;
  
  bool isBunnyClicked(float mouseX, float mouseY);


private:
  glm::ivec2 m_viewportSize{};
  glm::mat4 m_whiteBunnyModel{1.0f}; // Matriz de transformação do coelho branco

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  Camera m_camera;
  float m_dollySpeed{};
  float m_truckSpeed{};
  float m_panSpeed{};
  
  
  
  bool isSphereClicked(glm::vec3 sphereCenter, float radius, float mouseX, float mouseY); 
  //bool rayIntersectsModel(glm::vec3 const &ray, glm::mat4 const &modelMatrix);
  //bool rayBoxIntersection(glm::vec3 const &rayWorld,
  //                      glm::vec3 const &boxMin, glm::vec3 const &boxMax);
  //Ground m_ground;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
};

#endif
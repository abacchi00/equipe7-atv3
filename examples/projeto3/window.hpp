#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "model.hpp"
#include "trackball.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;


private:
  glm::ivec2 m_viewportSize{};

  Model m_model;
  int m_trianglesToDraw{};

  TrackBall m_trackBall;
  float m_zoom{};

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  

  glm::vec3 selectedColor{1.0f, 1.0f, 1.0f}; // Cor inicial: vermelho

  //std::optional<int>getClickedFace(int mouseX, int mouseY);

  //bool intersectsFace(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
  std::optional<int>getClosestFace();
  // bool intersectsFace(parametros e tipos aqui);

  GLuint m_program{};
};

#endif
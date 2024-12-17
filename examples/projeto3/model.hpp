#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "abcgOpenGL.hpp"

struct Vertex {
  glm::vec3 position{};
  glm::vec3 color{}; // Novo atributo para cor

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Model {
public:
  void loadObj(bool standardize = true);
  //void loadObj(std::string_view path, bool standardize = true);
  void render(int numTriangles = -1) const;
  void setupVAO(GLuint program);
  void destroy() const;
  void updateFaceColor(int faceIndex, glm::vec3 color);

  [[nodiscard]] int getNumTriangles() const {
    return gsl::narrow<int>(m_indices.size()) / 3;
  }

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};



  void createBuffers();
  void standardize();
};

#endif
#include "model.hpp"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    auto const h2{std::hash<glm::vec3>()(vertex.color)};
    return h1 ^ (h2 << 1); // Combina os hashes de posição e cor
  }
};

void Model::createBuffers() {
  // Delete previous buffers
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);

  // VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::loadObj(bool standardize) {
  m_vertices = {
      // Face frontal (vermelha)
      {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}, // Inferior esquerdo
      {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}, // Inferior direito
      {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}, // Superior direito
      {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}, // Superior esquerdo
      // Face traseira (verde)
      {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      // Face esquerda (azul)
      {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
      {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
      {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      // Face direita (amarela)
      {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      // Face inferior (ciano)
      {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
      {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
      // Face superior (magenta)
      {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
      {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
      {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
  };

  m_indices = {
      0, 1, 2, 0, 2, 3,       // Frente
      4, 5, 6, 4, 6, 7,       // Trás
      8, 9, 10, 8, 10, 11,    // Esquerda
      12, 13, 14, 12, 14, 15, // Direita
      16, 17, 18, 16, 18, 19, // Inferior
      20, 21, 22, 20, 22, 23  // Superior
  };

  if (standardize) {
    Model::standardize();
  }

  createBuffers();
}

void Model::setupVAO(GLuint program) {
  abcg::glDeleteVertexArrays(1, &m_VAO);

  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  auto const colorAttribute{abcg::glGetAttribLocation(program, "inColor")};
  if (colorAttribute >= 0) {
    abcg::glEnableVertexAttribArray(colorAttribute);
    abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offsetof(Vertex, color)));
  }

  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Model::render(int numTriangles) const {
  abcg::glBindVertexArray(m_VAO);

  auto const numIndices{(numTriangles < 0) ? m_indices.size()
                                           : numTriangles * 3};

  abcg::glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);
}

void Model::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{1.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Model::destroy() const {
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}


void Model::updateFaceColor(int faceIndex, glm::vec3 color) {
  // Cada face possui 6 índices (dois triângulos)
  auto startIndex = faceIndex * 6;

  // Atualize a cor dos vértices associados à face
  for (size_t i = 0; i < 6; ++i) {
    auto vertexIndex = m_indices.at(startIndex + i);
    m_vertices.at(vertexIndex).color = color;
  }

  // Atualize o VBO para refletir as mudanças
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(), m_vertices.data());
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
}

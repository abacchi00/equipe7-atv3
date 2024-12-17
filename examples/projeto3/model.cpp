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


// #include "model.hpp"

// #include <unordered_map>

// // Explicit specialization of std::hash for Vertex
// template <> struct std::hash<Vertex> {
//   size_t operator()(Vertex const &vertex) const noexcept {
//     auto const h1{std::hash<glm::vec3>()(vertex.position)};
//     return h1;
//   }
// };

// void Model::createBuffers() {
//   // Delete previous buffers
//   abcg::glDeleteBuffers(1, &m_EBO);
//   abcg::glDeleteBuffers(1, &m_VBO);

//   // VBO
//   abcg::glGenBuffers(1, &m_VBO);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//   abcg::glBufferData(GL_ARRAY_BUFFER,
//                      sizeof(m_vertices.at(0)) * m_vertices.size(),
//                      m_vertices.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // EBO
//   abcg::glGenBuffers(1, &m_EBO);
//   abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//   abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//                      sizeof(m_indices.at(0)) * m_indices.size(),
//                      m_indices.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }


// void Model::loadObj(bool standardize) {
//   m_vertices = {
//       // Face frontal
//       {{-0.5f, -0.5f,  0.5f}}, {{0.5f, -0.5f,  0.5f}}, {{0.5f,  0.5f,  0.5f}}, {{-0.5f,  0.5f,  0.5f}},
//       // Face traseira
//       {{-0.5f, -0.5f, -0.5f}}, {{0.5f, -0.5f, -0.5f}}, {{0.5f,  0.5f, -0.5f}}, {{-0.5f,  0.5f, -0.5f}}
//   };

//   m_indices = {
//       // Frente
//       0, 1, 2, 0, 2, 3,
//       // Trás
//       4, 5, 6, 4, 6, 7,
//       // Esquerda
//       4, 0, 3, 4, 3, 7,
//       // Direita
//       1, 5, 6, 1, 6, 2,
//       // Inferior
//       4, 5, 1, 4, 1, 0,
//       // Superior
//       3, 2, 6, 3, 6, 7
//   };

//   if (standardize) {
//     Model::standardize();
//   }

//   createBuffers();
// }

// // void Model::loadObj(std::string_view path, bool standardize) {
// //   tinyobj::ObjReader reader;

// //   if (!reader.ParseFromFile(path.data())) {
// //     if (!reader.Error().empty()) {
// //       throw abcg::RuntimeError(
// //           fmt::format("Failed to load model {} ({})", path, reader.Error()));
// //     }
// //     throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
// //   }

// //   if (!reader.Warning().empty()) {
// //     fmt::print("Warning: {}\n", reader.Warning());
// //   }

// //   auto const &attrib{reader.GetAttrib()};
// //   auto const &shapes{reader.GetShapes()};

// //   m_vertices.clear();
// //   m_indices.clear();

// //   // A key:value map with key=Vertex and value=index
// //   std::unordered_map<Vertex, GLuint> hash{};

// //   // Loop over shapes
// //   for (auto const &shape : shapes) {
// //     // Loop over indices
// //     for (auto const offset : iter::range(shape.mesh.indices.size())) {
// //       // Access to vertex
// //       auto const index{shape.mesh.indices.at(offset)};

// //       // Vertex position
// //       auto const startIndex{3 * index.vertex_index};
// //       auto const vx{attrib.vertices.at(startIndex + 0)};
// //       auto const vy{attrib.vertices.at(startIndex + 1)};
// //       auto const vz{attrib.vertices.at(startIndex + 2)};

// //       Vertex const vertex{.position = {vx, vy, vz}};

// //       // If hash doesn't contain this vertex
// //       if (!hash.contains(vertex)) {
// //         // Add this index (size of m_vertices)
// //         hash[vertex] = m_vertices.size();
// //         // Add this vertex
// //         m_vertices.push_back(vertex);
// //       }

// //       m_indices.push_back(hash[vertex]);
// //     }
// //   }

// //   if (standardize) {
// //     Model::standardize();
// //   }

// //   createBuffers();
// // }

void Model::render(int numTriangles) const {
  abcg::glBindVertexArray(m_VAO);

  auto const numIndices{(numTriangles < 0) ? m_indices.size()
                                           : numTriangles * 3};

  abcg::glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);
}

// void Model::setupVAO(GLuint program) {
//   // Release previous VAO
//   abcg::glDeleteVertexArrays(1, &m_VAO);

//   // Create VAO
//   abcg::glGenVertexArrays(1, &m_VAO);
//   abcg::glBindVertexArray(m_VAO);

//   // Bind EBO and VBO
//   abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

//   // Bind vertex attributes
//   auto const positionAttribute{
//       abcg::glGetAttribLocation(program, "inPosition")};
//   if (positionAttribute >= 0) {
//     abcg::glEnableVertexAttribArray(positionAttribute);
//     abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
//                                 sizeof(Vertex), nullptr);
//   }

//   // End of binding
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
//   abcg::glBindVertexArray(0);
// }

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

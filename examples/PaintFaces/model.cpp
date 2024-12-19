#include "model.hpp"
#include <iostream>
#include <filesystem>
#include <unordered_map>


std::vector<std::pair<int, int>> Model::getFaceTrianglePairs() const {
  std::vector<std::pair<int, int>> facePairs;
  for (int i = 0; i < m_indices.size(); i += 6) {
    facePairs.emplace_back(i, i + 3);
  }
  return facePairs;
}



void Model::loadFaceTexture(int faceIndex, std::string_view path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Texture file does not exist: " << path << "\n";
        return;
    }

    // textura da face
    abcg::glDeleteTextures(1, &m_faceTextures[faceIndex]);
    m_faceTextures[faceIndex] = abcg::loadOpenGLTexture({.path = path});

    if (m_faceTextures[faceIndex] == 0) {
        std::cerr << "Falha ao carregar textura. Face " << faceIndex << " arquivo " << path << "\n";
    } else {
        std::cout << "Textura carregada com sucesso na face " << faceIndex << " arquivo " << path << "\n";
    }

    // Atualizar par de triangulos
    auto const facePairs = getFaceTrianglePairs();
    if (faceIndex < static_cast<int>(facePairs.size())) {
        int startIndex = facePairs[faceIndex].first;
        int endIndex = facePairs[faceIndex].second;

        // Atualizar VBO para par de triangulos que formam a face
        for (int i = startIndex; i <= endIndex; ++i) {
            abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            abcg::glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(), m_vertices.data());
            abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}



// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    auto const h2{std::hash<glm::vec3>()(vertex.normal)};
    auto const h3{std::hash<glm::vec2>()(vertex.texCoord)};
    return abcg::hashCombine(h1, h2, h3);
  }
};

void Model::computeNormals() {
  // Clear previous vertex normals
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::vec3(0.0f);
  }


  for (std::size_t offset = 0; offset < m_indices.size(); offset += 3){
    // Get face vertices
    auto &a{m_vertices.at(m_indices.at(offset + 0))};
    auto &b{m_vertices.at(m_indices.at(offset + 1))};
    auto &c{m_vertices.at(m_indices.at(offset + 2))};

    // Compute normal
    auto const edge1{b.position - a.position};
    auto const edge2{c.position - b.position};
    auto const normal{glm::cross(edge1, edge2)};

    // Accumulate on vertices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // Normalize
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  m_hasNormals = true;
}

void Model::computeTangents() {

  std::vector bitangents(m_vertices.size(), glm::vec3(0));

  // Compute face tangents and bitangents

  for (std::size_t offset = 0; offset < m_indices.size(); offset += 3){
    // Get face indices
    auto const i1{m_indices.at(offset + 0)};
    auto const i2{m_indices.at(offset + 1)};
    auto const i3{m_indices.at(offset + 2)};

    // Get face vertices
    auto &v1{m_vertices.at(i1)};
    auto &v2{m_vertices.at(i2)};
    auto &v3{m_vertices.at(i3)};

    auto const e1{v2.position - v1.position};
    auto const e2{v3.position - v1.position};
    auto const delta1{v2.texCoord - v1.texCoord};
    auto const delta2{v3.texCoord - v1.texCoord};

    glm::mat2 M;
    M[0][0] = delta2.t;
    M[0][1] = -delta1.t;
    M[1][0] = -delta2.s;
    M[1][1] = delta1.s;
    M *= (1.0f / (delta1.s * delta2.t - delta2.s * delta1.t));

    auto const tangent{glm::vec4(M[0][0] * e1.x + M[0][1] * e2.x,
                                 M[0][0] * e1.y + M[0][1] * e2.y,
                                 M[0][0] * e1.z + M[0][1] * e2.z, 0.0f)};

    auto const bitangent{glm::vec3(M[1][0] * e1.x + M[1][1] * e2.x,
                                   M[1][0] * e1.y + M[1][1] * e2.y,
                                   M[1][0] * e1.z + M[1][1] * e2.z)};

    // Accumulate on vertices
    v1.tangent += tangent;
    v2.tangent += tangent;
    v3.tangent += tangent;

    bitangents.at(i1) += bitangent;
    bitangents.at(i2) += bitangent;
    bitangents.at(i3) += bitangent;
  }

  for (auto &&[i, vertex] : iter::enumerate(m_vertices)) {
    auto const &n{vertex.normal};
    auto const &t{glm::vec3(vertex.tangent)};

    // Orthogonalize t with respect to n
    auto const tangent{t - n * glm::dot(n, t)};
    vertex.tangent = glm::vec4(glm::normalize(tangent), 0);

    // Compute handedness of re-orthogonalized basis
    auto const b{glm::cross(n, t)};
    auto const handedness{glm::dot(b, bitangents.at(i))};
    vertex.tangent.w = (handedness < 0.0f) ? -1.0f : 1.0f;
  }
}

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

void Model::loadDiffuseTexture(std::string_view path) {
  if (!std::filesystem::exists(path))
    return;

  abcg::glDeleteTextures(1, &m_diffuseTexture);
  m_diffuseTexture = abcg::loadOpenGLTexture({.path = path});
}

void Model::loadNormalTexture(std::string_view path) {
  if (!std::filesystem::exists(path))
    return;

  abcg::glDeleteTextures(1, &m_normalTexture);
  m_normalTexture = abcg::loadOpenGLTexture({.path = path});
}

void Model::loadObj(std::string_view path, bool standardize) {
  auto const basePath{std::filesystem::path{path}.parent_path().string() + "/"};

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath; // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attrib{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};
  auto const &materials{reader.GetMaterials()};

  m_vertices.clear();
  m_indices.clear();

  m_hasNormals = false;
  m_hasTexCoords = false;

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices

    for (std::size_t offset = 0; offset < shape.mesh.indices.size(); ++offset){
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Position
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0),
                         attrib.vertices.at(startIndex + 1),
                         attrib.vertices.at(startIndex + 2)};

      // Normal
      glm::vec3 normal{};
      if (index.normal_index >= 0) {
        m_hasNormals = true;
        auto const normalStartIndex{3 * index.normal_index};
        normal = {attrib.normals.at(normalStartIndex + 0),
                  attrib.normals.at(normalStartIndex + 1),
                  attrib.normals.at(normalStartIndex + 2)};
      }

      // Texture coordinates
      glm::vec2 texCoord{};
      if (index.texcoord_index >= 0) {
        m_hasTexCoords = true;
        auto const texCoordsStartIndex{2 * index.texcoord_index};
        texCoord = {attrib.texcoords.at(texCoordsStartIndex + 0),
                    attrib.texcoords.at(texCoordsStartIndex + 1)};
      }

      Vertex const vertex{
          .position = position, .normal = normal, .texCoord = texCoord};

      if (index.texcoord_index < 0) {
       
        std::cerr << "Faltam coordenadas para textura " << offset / 3 << "\n";
      }


      
      m_indices.push_back(m_vertices.size());
      m_vertices.push_back(vertex);


     
    }
  }

  // Use properties of first material, if available
  if (!materials.empty()) {
    auto const &mat{materials.at(0)}; // First material
    m_Ka = {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1};
    m_Kd = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1};
    m_Ks = {mat.specular[0], mat.specular[1], mat.specular[2], 1};
    m_shininess = mat.shininess;

    if (!mat.diffuse_texname.empty())
      loadDiffuseTexture(basePath + mat.diffuse_texname);

    if (!mat.normal_texname.empty()) {
      loadNormalTexture(basePath + mat.normal_texname);
    } else if (!mat.bump_texname.empty()) {
      loadNormalTexture(basePath + mat.bump_texname);
    }
  } else {
    // Default values
    m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_shininess = 25.0f;
  }

  if (standardize) {
    Model::standardize();
  }

  if (!m_hasNormals) {
    computeNormals();
  }

  if (m_hasTexCoords) {
    computeTangents();
  }

  createBuffers();
}


void Model::render(int numTriangles) const {
    abcg::glBindVertexArray(m_VAO);

    int indicesPerFace = 6; // Cada face com 2 triangulos
    int totalFaces = m_indices.size() / indicesPerFace;

    for (int face = 0; face < totalFaces; ++face) {
        abcg::glActiveTexture(GL_TEXTURE0);

        // Vinculando a textura da face ou a textura padrão
        if (face < 6 && m_faceTextures[face] != 0) {
            abcg::glBindTexture(GL_TEXTURE_2D, m_faceTextures[face]);
            //std::cout << "Bind textura para a face " << face << "\n";
        } else {
            abcg::glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
            //std::cout << "Bind textura padrao para a face face " << face << "\n";
        }

        glDrawElements(GL_TRIANGLES, indicesPerFace, GL_UNSIGNED_INT,
                       reinterpret_cast<void *>(face * indicesPerFace * sizeof(GLuint)));
    }

    abcg::glBindVertexArray(0);
}



void Model::setupVAO(GLuint program) {
  // Release previous VAO
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // Bind vertex attributes
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  auto const normalAttribute{abcg::glGetAttribLocation(program, "inNormal")};
  if (normalAttribute >= 0) {
    abcg::glEnableVertexAttribArray(normalAttribute);
    auto const offset{offsetof(Vertex, normal)};
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

  auto const texCoordAttribute{
      abcg::glGetAttribLocation(program, "inTexCoord")};
  if (texCoordAttribute >= 0) {
    abcg::glEnableVertexAttribArray(texCoordAttribute);
    auto const offset{offsetof(Vertex, texCoord)};
    abcg::glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

  auto const tangentCoordAttribute{
      abcg::glGetAttribLocation(program, "inTangent")};
  if (tangentCoordAttribute >= 0) {
    abcg::glEnableVertexAttribArray(tangentCoordAttribute);
    auto const offset{offsetof(Vertex, tangent)};
    abcg::glVertexAttribPointer(tangentCoordAttribute, 4, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Model::standardize() {


  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{2.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Model::destroy() {
  abcg::glDeleteTextures(1, &m_normalTexture);
  abcg::glDeleteTextures(1, &m_diffuseTexture);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

Model::Model() {
  // Inicializa o array de texturas para cada face com 0
  std::fill(std::begin(m_faceTextures), std::end(m_faceTextures), 0);
}
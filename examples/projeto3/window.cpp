#include "window.hpp"


void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBall.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mousePress(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mouseRelease(mousePosition);
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }

  if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
    // Obtenha a face clicada (exemplo de verificação simplificada)
    // auto clickedFace = getClickedFace(event.button.x, event.button.y);
    // if (clickedFace.has_value()) {
    //   m_model.updateFaceColor(clickedFace.value(), selectedColor);
    // }

      auto closestFace = getClosestFace();
        if (closestFace.has_value()) {
          m_model.updateFaceColor(closestFace.value(), selectedColor);
  }
}
}



std::optional<int> Window::getClosestFace() {
  // Posição da câmera no espaço do mundo (pode ser ajustada conforme necessário)
  glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.0f + m_zoom);

  std::optional<int> closestFaceIndex = std::nullopt;
  float closestDistance = std::numeric_limits<float>::max();

  // Iterar pelas faces do modelo
  for (std::size_t i = 0; i < m_model.m_vertices.size(); i += 4) {
    // Calcular o centro da face como a média dos vértices
    glm::vec3 center = (m_model.m_vertices[i].position +
                        m_model.m_vertices[i + 1].position +
                        m_model.m_vertices[i + 2].position +
                        m_model.m_vertices[i + 3].position) /
                       4.0f;

    // Transformar o centro usando a matriz do modelo
    glm::vec4 transformedCenter = m_modelMatrix * glm::vec4(center, 1.0f);

    // Calcular a distância do centro transformado à câmera
    float distance = glm::distance(glm::vec3(transformedCenter), cameraPosition);

    if (distance < closestDistance) {
      closestDistance = distance;
      closestFaceIndex = i / 4; // Cada face tem 4 vértices
    }
  }

  return closestFaceIndex;
}

// std::optional<int> Window::getClickedFace(int mouseX, int mouseY) {

//    // Verifique a interseção com as faces do cubo
//   for (std::size_t i = 0; i < m_model.m_vertices.size(); ++i) {
//     const auto &face = m_model.m_vertices[i];

//       //if (intersectsFace(parametros aqui)) {
//       if (true) {
//         return i;  // Retorna o índice da face clicada
//       }
//   }

//     return std::nullopt; // Nenhuma face clicada
// }

  // // Converta mouseX e mouseY para coordenadas normalizadas da tela
  // float normalizedX = (2.0f * mouseX) / m_viewportSize.x - 1.0f;
  // float normalizedY = 1.0f - (2.0f * mouseY) / m_viewportSize.y;

  // // Construa o raio no espaço do mundo
  // glm::vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);
  // glm::vec4 rayEye = glm::inverse(m_projMatrix) * rayClip;
  // rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

  // glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(m_viewMatrix) * rayEye));

  // // Origem do raio (câmera)
  // glm::vec3 rayOrigin = glm::vec3(glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

  // for (int faceIndex = 0; faceIndex < 6; ++faceIndex) {
  //   // Cada face tem 6 índices (2 triângulos)
  //   int startIndex = faceIndex * 6;
  //   auto &v0 = m_model.m_vertices[m_model.m_indices[startIndex + 0]].position;
  //   auto &v1 = m_model.m_vertices[m_model.m_indices[startIndex + 1]].position;
  //   auto &v2 = m_model.m_vertices[m_model.m_indices[startIndex + 2]].position;

  //   // Testa interseção com o primeiro triângulo
  //   if (intersectsFace(rayOrigin, rayWorld, v0, v1, v2)) {
  //     return faceIndex;
  //   }

  //   auto &v3 = m_model.m_vertices[m_model.m_indices[startIndex + 3]].position;

  //   // Testa interseção com o segundo triângulo
  //   if (intersectsFace(rayOrigin, rayWorld, v0, v2, v3)) {
  //     return faceIndex;
  //   }
  // }

  // return std::nullopt; // Nenhuma face clicada
  // // Verifique a interseção com as faces do cubo
  // for (std::size_t i = 0; i < m_model.m_vertices.size(); ++i) {
  //   const auto &face = m_model.m_vertices[i];

  //     //if (intersectsFace(parametros aqui)) {
  //     if (true) {
  //       return i;  // Retorna o índice da face clicada
  //     }
  // }

  //   return std::nullopt; // Nenhuma face clicada

//}

// std::vector<int> Window::findClosestVertices(glm::vec2 mousePos, const std::vector<glm::vec3>& vertices, const glm::mat4& viewProjMatrix) {
//     std::vector<std::pair<float, int>> distances;

//     for (int i = 0; i < vertices.size(); ++i) {
//         // Projeta o vértice no espaço da tela
//         glm::vec4 clipSpace = viewProjMatrix * glm::vec4(vertices[i], 1.0);
//         glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w; // Normaliza para espaço NDC
//         glm::vec2 screenPos = (ndc.xy * 0.5f + 0.5f) * glm::vec2(windowWidth, windowHeight); // Converte para espaço de tela

//         // Calcula a distância ao ponto do mouse
//         float distance = glm::length(screenPos - mousePos);
//         distances.emplace_back(distance, i);
//     }

//     // Ordena os vértices pela distância
//     std::sort(distances.begin(), distances.end());

//     // Retorna os índices dos quatro vértices mais próximos
//     std::vector<int> closestIndices;
//     for (int i = 0; i < 4; ++i) {
//         closestIndices.push_back(distances[i].second);
//     }

//     return closestIndices;
// }


// bool Window::intersectsFace(glm::vec3 rayOrigin, glm::vec3 rayDir,
//                             glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
//   // Teste de interseção raio-triângulo usando o método Möller-Trumbore
//   const float EPSILON = 0.0000001f;
//   glm::vec3 edge1 = v1 - v0;
//   glm::vec3 edge2 = v2 - v0;
//   glm::vec3 h = glm::cross(rayDir, edge2);
//   float a = glm::dot(edge1, h);
//   if (a > -EPSILON && a < EPSILON)
//     return false; // O raio é paralelo ao triângulo

//   float f = 1.0f / a;
//   glm::vec3 s = rayOrigin - v0;
//   float u = f * glm::dot(s, h);
//   if (u < 0.0f || u > 1.0f)
//     return false;

//   glm::vec3 q = glm::cross(s, edge1);
//   float v = f * glm::dot(rayDir, q);
//   if (v < 0.0f || u + v > 1.0f)
//     return false;

//   // Verifica se o ponto de interseção está na direção positiva do raio
//   float t = f * glm::dot(edge2, q);
//   return t > EPSILON;
// }





std::array<glm::vec3, 4> colors{
    glm::vec3{1.0f, 0.0f, 0.0f},  // Vermelho
    glm::vec3{0.0f, 1.0f, 0.0f},  // Verde
    glm::vec3{0.0f, 0.0f, 1.0f},  // Azul
    glm::vec3{1.0f, 1.0f, 0.0f}   // Amarelo
};

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj();
  m_model.setupVAO(m_program);

  m_trianglesToDraw = m_model.getNumTriangles();
}

void Window::onUpdate() {
  m_modelMatrix = m_trackBall.getRotation();

  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  //auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);

  // Set uniform variables for the current model
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  //abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White

  m_model.render(m_trianglesToDraw);

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  // Create window for slider
  {
    ImGui::SetNextWindowPos(ImVec2(5, m_viewportSize.y - 94));
    ImGui::SetNextWindowSize(ImVec2(m_viewportSize.x - 10, -1));
    ImGui::Begin("Slider window", nullptr, ImGuiWindowFlags_NoDecoration);

    // Create a slider to control the number of rendered triangles
    {
      // Slider will fill the space of the window
      ImGui::PushItemWidth(m_viewportSize.x - 25);
      ImGui::SliderInt("", &m_trianglesToDraw, 0, m_model.getNumTriangles(),
                       "%d triangles");
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }

  // Create a window for the other widgets
  {
    auto const widgetSize{ImVec2(222, 90)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    static bool faceCulling{};
    ImGui::Checkbox("Back-face culling", &faceCulling);

    if (faceCulling) {
      abcg::glEnable(GL_CULL_FACE);
    } else {
      abcg::glDisable(GL_CULL_FACE);
    }

    // CW/CCW combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> const comboItems{"CCW", "CW"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Front face",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      if (currentIndex == 0) {
        abcg::glFrontFace(GL_CCW);
      } else {
        abcg::glFrontFace(GL_CW);
      }
    }
    

    // Projection combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      if (currentIndex == 0) {
        auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                          gsl::narrow<float>(m_viewportSize.y)};
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
      } else {
        m_projMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }

    ImGui::End();
  }

  {
    ImGui::Begin("Escolha de Cor");
    ImGui::Text("Selecione uma cor:");

    for (size_t i = 0; i < colors.size(); ++i) {
      ImGui::PushID(static_cast<int>(i)); // Garante IDs únicos
      if (ImGui::ColorButton("##color", ImVec4{colors[i].r, colors[i].g, colors[i].b, 1.0f})) {
        selectedColor = colors[i]; // Atualiza a cor selecionada
      }
      ImGui::SameLine(); // Coloca os botões na mesma linha
      ImGui::Text("Cor %zu", i + 1); // Rotulo para cada cor
      ImGui::PopID();
    }

    ImGui::End();
  }

}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_trackBall.resizeViewport(size);
}

void Window::onDestroy() {
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}
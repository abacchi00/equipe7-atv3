#include "window.hpp"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    m_keyState[event.key.keysym.sym] = true; // Mark the key as pressed
  }

  if (event.type == SDL_KEYUP) {
    m_keyState[event.key.keysym.sym] = false; // Mark the key as released
  }
}

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  // abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");

  // Load model
  loadModelFromFile(assetsPath + "bunny.obj");

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attributes{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      auto const vx{attributes.vertices.at(startIndex + 0)};
      auto const vy{attributes.vertices.at(startIndex + 1)};
      auto const vz{attributes.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

      // If map doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void Window::onPaint() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glBindVertexArray(m_VAO);

  // Draw white bunny
  glm::mat4 model{1.0f};
 model = glm::translate(model, glm::vec3(-1.25f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.3f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Store the bunny's model matrix for raycasting
  m_whiteBunnyModel = model;


  // Draw yellow bunny
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(-0.75f, 0.6f, -0.0f));
  model = glm::scale(model, glm::vec3(0.3f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);


  // Draw green bunny
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(0.0f, 1.0f, -0.0f));
  model = glm::scale(model, glm::vec3(0.3f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 0.0f, 0.8f, 0.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);


  // Draw blue bunny
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(0.75f, 0.6f, 0.0f));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.3f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 0.0f, 0.8f, 1.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);


  // Draw red bunny
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(1.25f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.3f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.25f, 0.25f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

// hidden lego

  // Draw hidden bunny
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(-0.7f, 1.5f, -0.5f));
  model = glm::scale(model, glm::vec3(0.3f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, m_bunnyColor.r, m_bunnyColor.g, m_bunnyColor.b, m_bunnyColor.a);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);


  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Window::showCameraPositionUI() {
  // Obtém a matriz de visão invertida e calcula a posição da câmera
  glm::mat4 viewMatrix = m_camera.getViewMatrix();
  glm::vec3 cameraPosition = glm::vec3(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2]);

  // Criar uma janela para mostrar as coordenadas XYZ da câmera
  ImGui::SetNextWindowSize(ImVec2(105, 135)); // Define o tamanho da janela
  ImGui::SetNextWindowPos(ImVec2(490, 5));  // Define a posição da janela

  ImGui::Begin("Camera Coordinates", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

  ImGui::Text("Camera Position:");
  ImGui::Text("X: %.2f", cameraPosition.x);
  ImGui::Text("Y: %.2f", cameraPosition.y);
  ImGui::Text("Z: %.2f", cameraPosition.z);

  ImGui::End();
};

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  static std::unordered_map<std::string, bool> objectVisibility{
      {"whiteBunny", false},
      {"yellowBunny", false},
      {"greenBunny", false},
      {"blueBunny", false},
      {"redBunny", false}, 
      {"hiddenBunny", false},};
  static std::string clickedMessage{""}; // Armazena a mensagem do objeto clicado

  showCameraPositionUI();

  // Detectar cliques do mouse nos objetos bunny usando raycasting
  if (ImGui::IsMouseClicked(0)) { // Botão esquerdo do mouse
    auto const [mouseX, mouseY] = ImGui::GetMousePos();

    // Verificar clique no bunny branco
    if (isSphereClicked(glm::vec3(-1.5f, 0.0f, 0.0f), 0.3f, mouseX, mouseY)) {
      objectVisibility["whiteBunny"] = !objectVisibility["whiteBunny"];
      clickedMessage = objectVisibility["whiteBunny"] ? "Hmm, I cannot see them from here... Maybe my friend on the right, the red one, saw something. They might have spotted them while avoiding space rocks or chasing aliens! You should ask them!" : "";
    }
    else if (isSphereClicked(glm::vec3(-0.75f, 0.6f, -0.0f), 0.3f, mouseX, mouseY)) {
      objectVisibility["yellowBunny"] = !objectVisibility["yellowBunny"];
      clickedMessage = objectVisibility["yellowBunny"] ? "I have not seen anything yet, but maybe the green one up top has a better view. He could be busy catching a comet or chasing space dust, but maybe they saw something!" : "";
    }
    // Verificar clique no bunny amarelo
    else if (isSphereClicked(glm::vec3(0.0f, 1.0f, -0.0f), 0.3f, mouseX, mouseY)) {
      objectVisibility["greenBunny"] = !objectVisibility["greenBunny"];
      clickedMessage = objectVisibility["greenBunny"] ? "I think I saw something... or maybe it was just a star falling. Who knows in this big space! You might want to check around me and yellow. We are floating higher than a rocket—could be something hiding up here!" : "";
    }
    // Verificar clique no bunny azul
    else if (isSphereClicked(glm::vec3(0.75f, 0.6f, 0.0f), 0.3f, mouseX, mouseY)) {
      objectVisibility["blueBunny"] = !objectVisibility["blueBunny"];
      clickedMessage = objectVisibility["blueBunny"] ? "I am looking hard, but no luck! Maybe red can see better, or maybe they are just floating off into space like I am! Go ask them!" : "";
    }
    // Verificar clique no bunny vermelho
    else if (isSphereClicked(glm::vec3(1.5f, 0.0f, 0.0f), 0.3f, mouseX, mouseY)) {
      objectVisibility["redBunny"] = !objectVisibility["redBunny"];
      clickedMessage = objectVisibility["redBunny"] ? "I cannot see them either! It’s like looking for a tiny spaceship in all this space! Maybe blue can see something, or maybe they’re lost in space! Who knows?" : "";
    }

     else if (isSphereClicked(glm::vec3(-0.7f, 1.5f, -0.5f), 0.4f, mouseX, mouseY)) {

    m_bunnyColor = glm::vec4(0.882f, 0.584f, 0.671f, 1.0f);

      objectVisibility["hiddenBunny"] = !objectVisibility["hiddenBunny"];
      clickedMessage = objectVisibility["hiddenBunny"] ? "You found me! I was starting to think I would have to set up camp on this asteroid. Thanks for rescuing me from the space void!" : "";
    }
  }

  if (!clickedMessage.empty()) {
    // Criar um painel na parte inferior ocupando 20% da altura da janela
    ImGui::SetNextWindowSize(ImVec2(m_viewportSize.x, m_viewportSize.y * 0.2f));
    ImGui::SetNextWindowPos(ImVec2(0, m_viewportSize.y * 0.8f));

    ImGui::Begin("Bottom Panel", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("%s", clickedMessage.c_str());
    ImGui::End();
  }

  // Add a section to show the current speeds and accelerations at the bottom of the screen
  ImGui::SetNextWindowPos(ImVec2(180, 5));
  ImGui::SetNextWindowSize(ImVec2(300, 135));

  ImGui::Begin("Speed & Acceleration", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

  // Display current speeds and accelerations for dolly, pan, and truck
  ImGui::Text("Dolly Speed: %.3f", m_dollySpeed);
  ImGui::Text("Dolly Acceleration: %.3f", m_dollyAcceleration);

  ImGui::Text("Pan Speed: %.3f", m_panSpeed);
  ImGui::Text("Pan Acceleration: %.3f", m_panAcceleration);

  ImGui::Text("Truck Speed: %.3f", m_truckSpeed);
  ImGui::Text("Truck Acceleration: %.3f", m_truckAcceleration);

  ImGui::End();
}

bool Window::isSphereClicked(glm::vec3 sphereCenter, float radius, float mouseX, float mouseY) {
  // Normalizar coordenadas do mouse (entre -1 e 1)
  float normalizedX = (2.0f * mouseX) / m_viewportSize.x - 1.0f;
  float normalizedY = 1.0f - (2.0f * mouseY) / m_viewportSize.y;

  // Construir a direção do raio no espaço da câmera
  glm::vec4 rayClip = glm::vec4(normalizedX, normalizedY, -1.0f, 1.0f);
  glm::vec4 rayEye = glm::inverse(m_camera.getProjMatrix()) * rayClip;
  rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Definir direção

  // Direção do raio no espaço do mundo
  glm::vec3 rayWorld = glm::vec3(glm::inverse(m_camera.getViewMatrix()) * rayEye);
  rayWorld = glm::normalize(rayWorld);

  // Origem do raio no espaço do mundo
  glm::vec3 rayOrigin = glm::vec3(glm::inverse(m_camera.getViewMatrix())[3]);

  // Verificar se o raio intersecta a esfera
  glm::vec3 oc = rayOrigin - sphereCenter;
  float a = glm::dot(rayWorld, rayWorld);
  float b = 2.0f * glm::dot(oc, rayWorld);
  float c = glm::dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;

  return (discriminant >= 0); // Clique está dentro da esfera se houver interseção
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Apply acceleration logic for each type of movement based on key states
  if (m_keyState[SDLK_w] || m_keyState[SDLK_UP]) {
    m_dollySpeed = std::min(m_dollySpeed + m_dollyAcceleration, m_dollyMaxSpeed); // Increase speed
  }
  if (m_keyState[SDLK_s] || m_keyState[SDLK_DOWN]) {
    m_dollySpeed = std::max(m_dollySpeed - m_dollyAcceleration, -m_dollyMaxSpeed); // Decrease speed
  }
  if (m_keyState[SDLK_d] || m_keyState[SDLK_RIGHT]) {
    m_panSpeed = std::min(m_panSpeed + m_panAcceleration, m_panMaxSpeed); // Increase speed
  }
  if (m_keyState[SDLK_a] || m_keyState[SDLK_LEFT]) {
    m_panSpeed = std::max(m_panSpeed - m_panAcceleration, -m_panMaxSpeed); // Decrease speed
  }
  if (m_keyState[SDLK_q]) {
    m_truckSpeed = std::min(m_truckSpeed + m_truckAcceleration, m_truckMaxSpeed); // Increase speed
  }
  if (m_keyState[SDLK_e]) {
    m_truckSpeed = std::max(m_truckSpeed - m_truckAcceleration, -m_truckMaxSpeed); // Decrease speed
  }

  // Apply movement with acceleration/deceleration
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);

  // Deceleration logic when no keys are pressed
  if (m_dollySpeed != 0.0f) {
    if (m_dollySpeed > 0.0f) {
      m_dollySpeed = glm::max(m_dollySpeed - 0.005f, 0.0f);  // Deceleration for dolly
    } else {
      m_dollySpeed = glm::min(m_dollySpeed + 0.005f, 0.0f);  // Deceleration for dolly (negative direction)
    }
  }

  if (m_panSpeed != 0.0f) {
    if (m_panSpeed > 0.0f) {
      m_panSpeed = glm::max(m_panSpeed - 0.005f, 0.0f);  // Deceleration for pan
    } else {
      m_panSpeed = glm::min(m_panSpeed + 0.005f, 0.0f);  // Deceleration for pan (negative direction)
    }
  }

  if (m_truckSpeed != 0.0f) {
    if (m_truckSpeed > 0.0f) {
      m_truckSpeed = glm::max(m_truckSpeed - 0.005f, 0.0f);  // Deceleration for truck
    } else {
      m_truckSpeed = glm::min(m_truckSpeed + 0.005f, 0.0f);  // Deceleration for truck (negative direction)
    }
  }
}

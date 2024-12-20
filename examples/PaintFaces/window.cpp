#include "window.hpp"
#include <iostream>
#include "imfilebrowser.h"

// Adicionando controle de dificuldade
enum class DifficultyLevel { Easy = 0, Medium, Hard };

DifficultyLevel currentDifficulty = DifficultyLevel::Easy;

std::string objetoSelecionado = "cube.obj";

std::string atualizaTexturaNormal = "maps/brick_normal.jpg";
std::string atualizaTexturaDifuse = "maps/brick_base.jpg";

std::array<std::string, 6> faceTextures{
    "maps/roman_lamp_diffuse.jpg", "maps/pattern.png", "maps/roman_lamp_diffuse.jpg",
    "maps/roman_lamp_diffuse.jpg", "maps/pattern.png", "maps/roman_lamp_diffuse.jpg"};
int selectedFace = 0;


void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallModel.mouseMove(mousePosition);
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mousePress(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mouseRelease(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }

   if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {

      auto closestFace = getClosestFace();
      if (closestFace.has_value()) {
        int faceIndex = closestFace.value();

        faceTextures[faceIndex] = atualizaTexturaDifuse;

        m_model.loadFaceTexture(faceIndex, abcg::Application::getAssetsPath() + faceTextures[faceIndex]);

        std::cout << "Atualizando textura da face " << faceIndex << " com " << faceTextures[faceIndex] << "\n";
      } else {
        std::cerr << "Nao encontrou.\n";
      }
    }
}





void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  for (auto const &name : m_shaderNames) {
    auto const path{assetsPath + "shaders/" + name};
    auto const program{abcg::createOpenGLProgram(
        {{.source = path + ".vert", .stage = abcg::ShaderStage::Vertex},
         {.source = path + ".frag", .stage = abcg::ShaderStage::Fragment}})};
    m_programs.push_back(program);
  }


  loadModel(assetsPath + "cube.obj"); // modelo carregado inicalmente
  m_mappingMode = 3; // Mesh


  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  m_trackBallModel.setVelocity(0.1f);

    // Todas as faces iniciam com a textura padrao da abcg
  for (size_t i = 0; i < faceTextures.size(); ++i) {
    faceTextures[i] = "maps/pattern.png";
    m_model.loadFaceTexture(i, assetsPath + faceTextures[i]);
  }

}



void Window::loadModel(std::string_view path) {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  m_model.destroy();

  m_model.loadDiffuseTexture(assetsPath + atualizaTexturaDifuse);
  m_model.loadNormalTexture(assetsPath + atualizaTexturaNormal);
  m_model.loadObj(path);
  m_model.setupVAO(m_programs.at(m_currentProgramIndex));
  m_trianglesToDraw = m_model.getNumTriangles();


  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  auto const program{m_programs.at(m_currentProgramIndex)};
  abcg::glUseProgram(program);

  auto const viewMatrixLoc{abcg::glGetUniformLocation(program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(program, "projMatrix")};
  auto const modelMatrixLoc{abcg::glGetUniformLocation(program, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(program, "normalMatrix")};
  auto const lightDirLoc{
      abcg::glGetUniformLocation(program, "lightDirWorldSpace")};
  auto const shininessLoc{abcg::glGetUniformLocation(program, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(program, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(program, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(program, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(program, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(program, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(program, "Ks")};
  auto const diffuseTexLoc{abcg::glGetUniformLocation(program, "diffuseTex")};
  auto const normalTexLoc{abcg::glGetUniformLocation(program, "normalTex")};
  auto const mappingModeLoc{abcg::glGetUniformLocation(program, "mappingMode")};


  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(normalTexLoc, 1);
  abcg::glUniform1i(mappingModeLoc, m_mappingMode);

  auto const lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);


  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);

  auto const modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  abcg::glUniform1f(shininessLoc, m_shininess);

  m_model.render(m_trianglesToDraw);
  m_modelMatrix= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
                  m_trackBallModel.getRotation();


  abcg::glUseProgram(0);
}

void Window::onUpdate() {
   

  float distanceFromOrigin = 4.0f;

  m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, distanceFromOrigin+m_zoom),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f));
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  auto const scaledWidth{gsl::narrow_cast<int>(m_viewportSize.x * 0.8f)};
  auto const scaledHeight{gsl::narrow_cast<int>(m_viewportSize.y * 0.8f)};

  // Procura Model
  static ImGui::FileBrowser fileDialogModel;
  fileDialogModel.SetTitle("Load 3D Model");
  fileDialogModel.SetTypeFilters({".obj"});
  fileDialogModel.SetWindowSize(scaledWidth, scaledHeight);

  // Procura  Textura
  static ImGui::FileBrowser fileDialogDiffuseMap;
  fileDialogDiffuseMap.SetTitle("Load Diffuse Map");
  fileDialogDiffuseMap.SetTypeFilters({".jpg", ".png"});
  fileDialogDiffuseMap.SetWindowSize(scaledWidth, scaledHeight);

  // Normal maps
  static ImGui::FileBrowser fileDialogNormalMap;
  fileDialogNormalMap.SetTitle("Load Normal Map");
  fileDialogNormalMap.SetTypeFilters({".jpg", ".png"});
  fileDialogNormalMap.SetWindowSize(scaledWidth, scaledHeight);

#if defined(__EMSCRIPTEN__)
  auto const assetsPath{abcg::Application::getAssetsPath()};
  fileDialogModel.SetPwd(assetsPath);
  fileDialogDiffuseMap.SetPwd(assetsPath + "/maps");
  fileDialogNormalMap.SetPwd(assetsPath + "/maps");
#endif


  {
    auto widgetSize{ImVec2(222, 280)};

    if (!m_model.isUVMapped()) {

      widgetSize.y += 26;
    }

    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr,
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration);

    // Menu
    {
      bool loadModel{};
      bool loadDiffuseMap{};
      bool loadNormalMap{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          ImGui::MenuItem("Load 3D Model...", nullptr, &loadModel);
          ImGui::MenuItem("Load Diffuse Map...", nullptr, &loadDiffuseMap);
          ImGui::MenuItem("Load Normal Map...", nullptr, &loadNormalMap);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (loadModel)
        fileDialogModel.Open();
      if (loadDiffuseMap)
        fileDialogDiffuseMap.Open();
      if (loadNormalMap)
        fileDialogNormalMap.Open();
    }

    // Slider triangulos
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderInt(" ", &m_trianglesToDraw, 0, m_model.getNumTriangles(),
                     "%d triangles");
    ImGui::PopItemWidth();

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
        
        for (std::size_t index = 0; index < comboItems.size(); ++index){
        
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

        for (std::size_t index = 0; index < comboItems.size(); ++index){
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                        gsl::narrow<float>(m_viewportSize.y)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
      } else {
        m_projMatrix =
            glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }

    // Shader combo box
    {
      static std::size_t currentIndex{};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Shader", m_shaderNames.at(currentIndex))) {

        for (std::size_t index = 0; index < m_shaderNames.size(); ++index){
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(m_shaderNames.at(index), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      // Set up VAO 
      if (gsl::narrow<int>(currentIndex) != m_currentProgramIndex) {
        m_currentProgramIndex = gsl::narrow<int>(currentIndex);
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
      }
    }

    if (!m_model.isUVMapped()) {
      ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh has no UV coords.");
    }

     // UV mapping box
    {
      std::vector<std::string> comboItems{"Triplanar", "Cylindrical",
                                          "Spherical"};

      if (m_model.isUVMapped())
        comboItems.emplace_back("From mesh");

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("UV mapping",
                            comboItems.at(m_mappingMode).c_str())) {

        for (std::size_t index = 0; index < comboItems.size(); ++index){
          auto const isSelected{m_mappingMode == static_cast<int>(index)};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            m_mappingMode = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();
    }


    // // Textura
    // {
    //   static std::size_t currentIndex{};
    //   std::vector<std::string> comboItems{"brick", "rock", "paper",
    //                                       "roman"};

    //   ImGui::PushItemWidth(120);
    //   if (ImGui::BeginCombo("Textura",
    //                         comboItems.at(currentIndex).c_str())) {

    //   for (std::size_t index = 0; index < comboItems.size(); ++index){
    //     auto const isSelected{currentIndex == index};
    //     if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected)) {
    //         currentIndex = index;
    //         if (index == 0 ) {
    //            atualizaTexturaNormal = "maps/brick_normal.jpg";
    //            atualizaTexturaDifuse = "maps/brick_base.jpg"   ;
    //         } else if (index == 1){
    //             atualizaTexturaNormal = "maps/rock.png";
    //            atualizaTexturaDifuse = "maps/rock.png"   ;
    //         } else if (index == 2){
    //             atualizaTexturaNormal = "maps/paper.png";
    //            atualizaTexturaDifuse = "maps/paper.png"   ;
    //         }
    //         else  {
    //             atualizaTexturaNormal = "maps/roman_lamp_normal.jpg";
    //            atualizaTexturaDifuse =  "maps/roman_lamp_diffuse.jpg" ;
    //         }
    //     }
    // }

    //   ImGui::EndCombo();
    // }
    //   ImGui::PopItemWidth();
    // }

// Controle de dificuldade e texturas

// Controle de dificuldade e texturas

// Controle de dificuldade e texturas

// Variável global ou membro da classe
static std::size_t textureIndex = 0;

// Controle de dificuldade
{
  static DifficultyLevel currentDifficulty = DifficultyLevel::Easy;
  const std::vector<std::string> difficultyOptions{"Easy", "Medium", "Hard"};
  static int selectedDifficulty = 0;

  ImGui::Text("Select Difficulty:");
  ImGui::PushItemWidth(120);
  if (ImGui::BeginCombo("Difficulty", difficultyOptions[selectedDifficulty].c_str())) {
    for (int i = 0; i < difficultyOptions.size(); ++i) {
      bool isSelected = (i == selectedDifficulty);
      if (ImGui::Selectable(difficultyOptions[i].c_str(), isSelected)) {
        selectedDifficulty = i;
        currentDifficulty = static_cast<DifficultyLevel>(i);

        // Resetar índice de textura ao mudar a dificuldade
        textureIndex = 0;
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();
}

// Controle de seleção de textura
{
  std::vector<std::string> textureOptions;

  // Define as opções de textura com base na dificuldade
  if (currentDifficulty == DifficultyLevel::Easy) {
    textureOptions = {"brick", "rock", "paper", "roman"};
  } else if (currentDifficulty == DifficultyLevel::Medium) {
    textureOptions = {"brick", "rock", "roman"};
  } else if (currentDifficulty == DifficultyLevel::Hard) {
    textureOptions = {"brick", "roman"};
  }

  ImGui::Text("Select Texture:");
  ImGui::PushItemWidth(120);
  if (ImGui::BeginCombo("Texture", textureOptions[textureIndex].c_str())) {
    for (std::size_t i = 0; i < textureOptions.size(); ++i) {
      bool isSelected = (i == textureIndex);
      if (ImGui::Selectable(textureOptions[i].c_str(), isSelected)) {
        textureIndex = i;

        // Atualiza as texturas com base na seleção
        if (textureOptions[i] == "brick") {
          atualizaTexturaNormal = "maps/brick_normal.jpg";
          atualizaTexturaDifuse = "maps/brick_base.jpg";
        } else if (textureOptions[i] == "rock") {
          atualizaTexturaNormal = "maps/rock.png";
          atualizaTexturaDifuse = "maps/rock.png";
        } else if (textureOptions[i] == "paper") {
          atualizaTexturaNormal = "maps/paper.png";
          atualizaTexturaDifuse = "maps/paper.png";
        } else if (textureOptions[i] == "roman") {
          atualizaTexturaNormal = "maps/roman_lamp_normal.jpg";
          atualizaTexturaDifuse = "maps/roman_lamp_diffuse.jpg";
        }
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();
}




    

//     if (ImGui::Button("Apply Texture to Closest Face")) {
//       auto closestFace = getClosestFace();
//       if (closestFace.has_value()) {
//         int faceIndex = closestFace.value();

//         // Atualize a textura da face mais próxima
//         faceTextures[faceIndex] = atualizaTexturaDifuse;

//         // Carregue a textura para a face
//         m_model.loadFaceTexture(faceIndex, abcg::Application::getAssetsPath() + faceTextures[faceIndex]);

//         std::cout << "Atualizando textura para face " << faceIndex << " usando " << faceTextures[faceIndex] << "\n";
//   }

//       else {
//         std::cerr << "N encontrou.\n";
//     }
// }



    // Object Box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Cube", "Cube",
                                          "Cube"};
      ImGui::PushItemWidth(120);
     if (ImGui::BeginCombo("Object", 
                          comboItems.at(currentIndex).c_str())) {

      for (std::size_t index = 0; index < comboItems.size(); ++index){
        auto const isSelected{currentIndex == index};

        if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected)) {
            currentIndex = index;
            if (index == 0 ) {
                objetoSelecionado = "cube.obj";
            } else if (index == 1){
                objetoSelecionado = "cube.obj";
            }
            else  {
                objetoSelecionado = "cube.obj";
            }
        }
      }

      ImGui::EndCombo();
    }
      ImGui::PopItemWidth();
    }

  
    

    ImGui::End();
  }

  // Iluminacao
  if (m_currentProgramIndex <= 4) {
    auto const widgetSize{ImVec2(222, 244)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5,
                                   m_viewportSize.y - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("Light properties");

    // Slider iluminacao
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ia", &m_Ia.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Id", &m_Id.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Is", &m_Is.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Material properties");

    // Slider propriedades do material
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ka", &m_Ka.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Kd", &m_Kd.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Ks", &m_Ks.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    // Slider shineness
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderFloat(" ", &m_shininess, 0.0f, 500.0f, "shininess: %.1f");
    ImGui::PopItemWidth();

    ImGui::End();
  }

  fileDialogModel.Display();
  if (fileDialogModel.HasSelected()) {
    loadModel(fileDialogModel.GetSelected().string());
    fileDialogModel.ClearSelected();

    if (m_model.isUVMapped()) {

      m_mappingMode = 3;
    } else {

      m_mappingMode = 0;
    }
  }

  fileDialogDiffuseMap.Display();
  if (fileDialogDiffuseMap.HasSelected()) {
    m_model.loadDiffuseTexture(fileDialogDiffuseMap.GetSelected().string());
    fileDialogDiffuseMap.ClearSelected();
  }

  fileDialogNormalMap.Display();
  if (fileDialogNormalMap.HasSelected()) {
    m_model.loadNormalTexture(fileDialogNormalMap.GetSelected().string());
    fileDialogNormalMap.ClearSelected();
  }
}


void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_trackBallModel.resizeViewport(size);
  m_trackBallLight.resizeViewport(size);
}

void Window::onDestroy() {
  m_model.destroy();
  for (auto const &program : m_programs) {
    abcg::glDeleteProgram(program);
  }
}



std::optional<int> Window::getClosestFace() {
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.0f + m_zoom);

    std::optional<int> closestFaceIndex = std::nullopt;
    float closestDistance = std::numeric_limits<float>::max();

    // dois triangulos
    int indicesPerFace = 6;

    // Itera pelas faces
    for (std::size_t i = 0; i < m_model.m_indices.size(); i += indicesPerFace) {
        glm::vec3 center(0.0f);

        // Calcula o centro da face usando
        for (int j = 0; j < indicesPerFace; ++j) {
            auto vertexIndex = m_model.m_indices[i + j];
            center += m_model.m_vertices[vertexIndex].position;
        }
        center /= static_cast<float>(indicesPerFace);

        // Transformar o centro usando a matriz do modelo
        glm::vec4 transformedCenter = m_modelMatrix * glm::vec4(center, 1.0f);

        // Calcular a distancia do centro
        float distance = glm::distance(glm::vec3(transformedCenter), cameraPosition);

        if (distance < closestDistance) {
            closestDistance = distance;
            closestFaceIndex = i / indicesPerFace; // indice da face
        }
    }

    if (closestFaceIndex.has_value()) {
        std::cout << "Indice da face mais proxima encontrada: " << closestFaceIndex.value() << "\n";
    } else {
        std::cerr << "Nao encontrou.\n";
    }

    return closestFaceIndex;
}

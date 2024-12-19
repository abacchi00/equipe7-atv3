// #include "window.hpp"

// int main(int argc, char **argv) {
//   try {
//     abcg::Application app(argc, argv);

//     Window window;
//     window.setOpenGLSettings({.samples = 4});
//     window.setWindowSettings({
//         .width = 600,
//         .height = 600,
//         .title = "Model Viewer (version 5)",
//     });

//     app.run(window);
//   } catch (std::exception const &exception) {
//     fmt::print(stderr, "{}\n", exception.what());
//     return -1;
//   }
//   return 0;
// }

#include "window.hpp"

#ifdef _WIN32
#include <windows.h>
#include <iostream>

// Função para anexar um console no Windows
void attachConsole() {
  AllocConsole(); // Cria um console
  freopen("CONOUT$", "w", stdout); // Redireciona stdout para o console
  freopen("CONOUT$", "w", stderr); // Redireciona stderr para o console
  std::cout.clear();
  std::cerr.clear();
}
#endif

int main(int argc, char **argv) {
  try {
    #ifdef _WIN32
    attachConsole(); // Anexa o console no Windows
    #endif

    abcg::Application app(argc, argv);

    Window window;
    window.setOpenGLSettings({.samples = 4});
    window.setWindowSettings({
        .width = 600,
        .height = 600,
        .title = "Model Viewer (version 5)",
    });

    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }

  #ifdef _WIN32
  std::cout << "Pressione qualquer tecla para sair..." << std::endl;
  system("pause"); // Pausa o console até o usuário pressionar uma tecla
  #endif

  return 0;
}

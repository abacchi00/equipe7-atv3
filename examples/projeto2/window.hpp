#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>
#include <vector> // Adicionado para armazenar os pontos do gráfico

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
public:
  // Funções de controle do ciclo de vida da janela
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onDestroy() override;

private:
// TODO
};
#endif>

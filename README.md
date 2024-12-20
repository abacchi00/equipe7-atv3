
# Projeto 3 - Computação Gráfica

  

## Tema

  

Exploração de Texturas e Iluminação em Ambientes 3D: Ferramenta interativa para renderização realista e manipulação de materiais e fontes de luz

  

## Introdução

  

Este projeto tem como tema "Texturas e Iluminação 3D" e busca explorar técnicas de computação gráfica aplicadas à renderização de cenas tridimensionais com foco na criação de ambientes visualmente realistas e interativos.

  

Cenário de aplicação: O projeto visa criar uma cena 3D onde os usuários possam selecionar diferentes modelos e "pintar" suas faces utilizando o mouse com texturas variadas, além de manipular a iluminação da cena. Esse tipo de interação é útil em cenários educacionais e industriais, como design de produtos, arquitetura e jogos digitais.

  

Objetivo final: Desenvolver uma aplicação interativa que permita aos usuários manipular texturas e iluminação de forma intuitiva e prática, incentivando o aprendizado e experimentação com conceitos gráficos.

### Justificativa: 
O projeto foi desenvolvido para auxiliar no raciocínio lógico e no entendimento de conceitos matemáticos, inspirado pelo Teorema das Quatro Cores. Esse teorema, que afirma que qualquer mapa pode ser colorido com no máximo quatro cores de maneira que regiões adjacentes não compartilhem a mesma cor, serve como base para a dinâmica do jogo. O objetivo é pintar as faces de um cubo tridimensional, onde cada face deve ser colorida de maneira que as faces adjacentes, que compartilham uma aresta, não tenham a mesma textura. Assim, o projeto permite a exploração intuitiva de conceitos de topologia e grafos, estimulando o usuário a aplicar raciocínio lógico para encontrar uma solução viável e colorir o cubo de acordo com as restrições do problema.

  

## Guia de utilização da aplicação final

  

1. Clone o repositório localmente. Entre na pasta `equipe7-atv3` e execute o script `build.bat` se estiver usando Windows, ou então o `build.sh` para Linux;

  

2. Abra um terminal e execute o comando:

  

- Para Windows: `.\build\bin\paintFaces\paintFaces | cat`

- Para Linux: `./build/bin/paintFaces/paintFaces | cat`

  

A janela interativa deve ser aberta em seguida;

  

3. Caso queira compilar a aplicação em Assembly, execute:

  

- Para Windows: `build-wasm.bat && runweb.bat`

- Para Linux: `./build-wasm.sh && ./runweb.sh`

  

Você pode também visualizar a aplicação compilada através do link: [Link para o app](https://abacchi00.github.io/equipe7-atv3/public/projeto3.html)


### **Instruções:**

-   **Movimentar o cubo**: Use o **botão esquerdo do mouse** para rotacionar o cubo 3D.
-   **Trocar textura**: Clique com o **botão direito do mouse** na face que deseja alterar.
-   **Escolher textura**: Selecione a textura desejada no **dropdown** no canto superior direito.

O objetivo é pintar as faces do cubo sem que faces adjacentes compartilhem a mesma textura.
  

## Projeto e desenvolvimento

  

Aspectos Originais do Código e Atendimento aos Requisitos

1. Gráficos 3D com Primitivas do OpenGL:
Uso de OpenGL para renderizar modelos 3D com primitivos (triângulos).
Função relacionada: Model::render (Arquivo: window.cpp).

2. Interação com o Usuário:
Interação - Seleção de faces para pintura com texturas usando o mouse.
Funções relacionadas:
Window::getClosestFace (Arquivo: window.cpp) — detecta a face mais próxima para interação.
Window::onEvent (Arquivo: window.cpp) — lida com eventos do mouse para aplicar texturas.

3. Animação e Manipulação do Objeto 3D:
Manipulação do modelo 3D usando a técnica de "trackball" para rotação e movimento.
Função relacionada: m_trackBallModel (Arquivo: trackball.cpp).

4. Iluminação e Texturização:
Iluminação configurada no arquivo .mtl (ambiental, difusa, especular).
Texturização do modelo com mapeamento UV a partir do arquivo .obj.
Funções relacionadas:
window::loadModel (Arquivo: window.cpp) — carrega e aplica texturas.
box.mtl (Arquivo de material) — define as propriedades de iluminação e material.

  

## Problemas e soluções:

  

Problema: Foi necessário fazer com que cada face de um modelo 3D fosse tratada de forma independente para aplicação das texturas. Isso exigiu duplicar vértices compartilhados, mesmo que tenham as mesmas coordenadas.

  

Solução: A duplicação foi feita manualmente durante o processo de mapeamento de texturas.

  

Problema: A seleção da face mais próxima da tela não garante que a face clicada seja sempre a correta.

  

Solução futura: Melhorar a lógica de detecção para pintar a face onde o clique de fato ocorreu, considerando coordenadas de tela e profundidade.

  

## Resultados e análise

  

Aspectos computacionais: A aplicação atingiu um desempenho satisfatório, com renderização fluida e interatividade responsiva.

  

Resultados visuais: A aplicação de texturas e iluminação proporcionou uma experiência gráfica rica e interativa.

  

## Conclusões

  

O projeto alcançou seus objetivos de implementar texturas e iluminação realistas em um ambiente 3D interativo. Destacamos:

  

Pontos de destaque: Uso efetivo de shaders, interatividade bem implementada.

  

Limitações: Necessidade de melhorar a precisão da seleção de faces.

  

## Trabalhos futuros

  

- Refinar o sistema de seleção de faces para garantir precisão ao clicar.

- Expandir o projeto com mapeamento de sombras e texturas procedurais.
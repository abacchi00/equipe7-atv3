# Projeto 2 - Computação Gráfica

## Tema

Jogo interativo 3D para ensinar inglês: Ferramenta interativa para movimentação, manipulação de visualizações tridimensionais e ensino de inglês

## Como executar a aplicação

1. Clone o repositório localmente. Entre na pasta `equipe7-atv2` e execute o script `build.bat` se estiver usando Windows, ou então o `build.sh` para Linux;

2. Abra um terminal e execute o comando:

- Para Windows: `.\build\bin\projeto2\projeto2.exe | cat`
- Para Linux: `./build/bin/projeto2/projeto2 | cat`

A janela interativa deve ser aberta em seguida;

3. Caso queira compilar a aplicação em Assembly, execute:

- Para Windows: `build-wasm.bat && runweb.bat`
- Para Linux: `./build-wasm.sh && ./runweb.sh`

Você pode também visualizar a aplicação compilada através do link: [Link para o app](https://abacchi00.github.io/equipe7-atv2/public/projeto2.html)

## Detalhes da implementação

A implementação do projeto envolveu várias técnicas de computação gráfica e interatividade, utilizando principalmente **C++**, **OpenGL** e bibliotecas auxiliares como **SDL2** e **ImGui**. A seguir, são descritas as principais técnicas utilizadas na construção da aplicação:

### 1. **Movimentação da Câmera em 3D**

A movimentação da câmera é uma das partes fundamentais do projeto. A câmera 3D foi configurada para permitir os seguintes tipos de movimento, baseados na interação do usuário com o teclado:

- **Dolly**: Movimento de aproximação e afastamento da câmera, ajustado pela velocidade controlada pelas teclas `W` (aproximar) e `S` (afastar).
- **Pan**: Movimento de rotação ao redor do eixo horizontal da câmera, controlado pelas teclas `A` (rotacionar à esquerda) e `D` (rotacionar à direita).
- **Truck**: Movimento lateral, controlado pelas teclas `Q` (mover para a esquerda) e `E` (mover para a direita).

Cada um desses movimentos utiliza um conceito de aceleração e desaceleração. A aceleração aumenta a velocidade do movimento até atingir um valor máximo, enquanto a desaceleração suaviza a parada quando as teclas não são pressionadas.

### 2. **Desaceleração de Movimento**

A desaceleração de movimento é aplicada automaticamente quando o usuário solta as teclas de movimento. Para isso, verificamos se a velocidade de cada tipo de movimento (dolly, pan, truck) é diferente de zero e, se for o caso, a velocidade é gradualmente reduzida até que o movimento pare completamente. Esse comportamento cria uma sensação de movimento suave e realista.

- **Cálculo da desaceleração**: A desaceleração é feita subtraindo um valor constante da velocidade atual, até que a velocidade atinja zero. O valor da desaceleração é configurável e foi ajustado para garantir uma transição suave.

### 3. **Renderização Gráfica com OpenGL**

A renderização do ambiente 3D foi realizada utilizando a API **OpenGL**, que oferece as ferramentas necessárias para desenhar e manipular objetos 3D. Os objetos são representados por malhas (meshes), que são carregadas e renderizadas na tela.

- **Shaders**: A aplicação faz uso de shaders em OpenGL para aplicar efeitos visuais aos objetos 3D, como iluminação básica, efeitos de texturização e transparência.
- **Buffers de vértices**: Para otimizar a renderização, os objetos 3D são armazenados em buffers de vértices, que são então passados para o pipeline gráfico do OpenGL.

### 4. **Interação com Modelos 3D (Click-to-Action)**

A aplicação permite ao usuário interagir com objetos 3D no ambiente, utilizando cliques do mouse para selecionar e ativar modelos. Para isso, a interação é tratada da seguinte forma:

- **Detecção de cliques**: Usamos a biblioteca **SDL2** para capturar os eventos de clique do mouse. Quando o usuário clica na tela, a posição do clique é convertida de coordenadas de tela para coordenadas no espaço 3D.
- **Raycasting**: A técnica de **raycasting** é utilizada para determinar qual objeto na cena foi clicado. Um "raio" é projetado a partir da posição da câmera em direção à posição do mouse, e verifica-se qual objeto está mais próximo ao raio.
- **Ação no objeto**: Ao detectar o objeto clicado, o sistema pode realizar ações associadas a ele, como exibir um texto, ativar animações ou mudar o estado de alguma propriedade visual do objeto (como cor ou forma).

### 5. **Interface Gráfica com ImGui**

Para facilitar a criação da interface de controle e fornecer feedback visual ao usuário, foi utilizada a biblioteca **ImGui**. Através dela, foi possível criar controles interativos, como botões, sliders e caixas de texto, para ajustar os parâmetros da cena (como a velocidade dos movimentos da câmera e a intensidade das interações).

### 6. **Ensino de Inglês Interativo**

A aplicação não se limita apenas à exploração gráfica, mas também incorpora um módulo de ensino de inglês. Por meio da interação com objetos 3D, o usuário pode acessar informações sobre vocabulário e frases, com foco em termos relacionados ao contexto da cena.

- **Exibição de texto**: Ao clicar em certos objetos ou áreas da cena, o usuário é apresentado a traduções, palavras ou frases em inglês, associadas ao objeto ou à ação realizada.
- **Contextualização**: O aprendizado do inglês ocorre de maneira contextual, ou seja, as palavras e expressões são apresentadas de forma a serem facilmente compreendidas pelo usuário, utilizando a visualização 3D como suporte.

### 7. **Compilação Web (WASM)**

Além da versão desktop, a aplicação foi compilada para a web utilizando **WebAssembly (WASM)**, permitindo que a ferramenta seja acessada diretamente em navegadores. A versão WASM utiliza os mesmos conceitos gráficos e interativos, mas é otimizada para rodar no navegador de forma eficiente.

- **Compilação para WASM**: A aplicação foi configurada para compilar os arquivos C++ diretamente para WASM utilizando ferramentas como Emscripten, permitindo que o código C++ seja executado no navegador sem a necessidade de plugins adicionais.

## Justificativa

A escolha deste tema foi motivada pela necessidade de criar uma ferramenta de visualização interativa e manipulável em 3D, que permitisse explorar diferentes tipos de movimentos de câmera em uma cena. Além disso, como parte da inovação pedagógica, o projeto também integra um componente de ensino de inglês, utilizando a visualização 3D como meio de ensino de vocabulário e expressões, promovendo o aprendizado contextual.

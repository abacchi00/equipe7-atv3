# Projeto 1 - Computação Gráfica

## Tema
**Ensinando matemática financeira: uma ferramenta interativa e visual para aprender sobre juros**

## Como executar a aplicação
1. Clone o repositório localmente. Entre na pasta `equipe7-atv1` e execute o script `build.bat` se estiver usando Windows, ou então o `build.sh` para Linux;
2. Abra um terminal e execute o comando: 
   - Para Windows: `.\build\bin\projeto1\projeto1.exe | cat`
   - Para Linux: `./build/bin/projeto1/projeto1 | cat`
   
   A janela interativa deve ser aberta em seguida;
3. Caso queira compilar a aplicação em Assembly, execute:
   - Para Windows: `build-wasm.bat && runweb.bat`
   - Para Linux: `./build-wasm.sh && ./runweb.sh`
   
   Você pode também visualizar a aplicação compilada através do link: [Ensinando matemática financeira](https://abacchi00.github.io/equipe7-atv1/public/projeto1.html)

## Detalhes da implementação
O projeto foi desenvolvido nos três arquivos: `window.cpp`, `main.cpp` e `window.hpp`, na linguagem C++ e com o auxílio da biblioteca **ImGui**, para interface gráfica, e da biblioteca **ABCg**, para simplificar comandos com OpenGL. Um resumo do conteúdo de cada um é dado a seguir.

- **Window.hpp**: Declara as variáveis, valores iniciais, métodos e funções usados em `window.cpp`. Abaixo, detalhamento dos parâmetros (variáveis financeiras) que são mutáveis pelo usuário:  
  - `m_P`: Investimento inicial
  - `m_PMT`: Contribuição mensal
  - `m_i`: Taxa de juros mensal
  - `m_nMax`: Número de meses
  
  Também foram definidos neste arquivo os Getters e Setters para as variáveis financeiras.

- **Window.cpp**: Contém as principais funções do projeto, como a `calculateGraphPoints`, responsável por plotar os valores a partir dos parâmetros de entrada do usuário. Além disso:
  - Controle de limpeza de tela a partir da biblioteca `abcg`
  - Funções `onPaint` e `onCreate` para definir o comportamento das janelas de acordo com a interação do usuário
  - Criação de widgets e dos textos exibidos na interface gráfica.

Para a construção do código, foram aplicados os conceitos e técnicas de:
  - Renderização gráfica (`onPaint` e `calculateGraphPoints`)
  - Manipulação de shaders (`onCreate`)
  - Comandos ImGui para interface gráfica (`onPaintUI`)
  - Configuração de buffers (`onCreate` e `setupModel`)
  - Normalização de funções e conceitos gerais de computação gráfica (`calculateGraphPoints`)

- **main.cpp**: Criação da aplicação, configurações gerais das janelas abertas e tratamento de exceções.

## Justificativa
O tema foi escolhido por ser um tópico recente na grade curricular dos estudantes do ensino fundamental e médio. A disciplina **Educação Financeira** passou a ser obrigatória nas escolas estaduais do estado de São Paulo a partir de 2024. Com essa mudança, há necessidade de desenvolver novas ferramentas que facilitem o ensino-aprendizagem.

Para entender a percepção dos professores, foi realizada uma **entrevista empática** com o objetivo de colher insights sobre as dificuldades e sugestões de uso da tecnologia e recursos gráficos para apoiar a educação.

## Entrevista Empática

- **Pergunta**: “Os alunos demonstram interesse pela disciplina de Educação Financeira?”
  - **Resposta**: “Sim, por ser uma disciplina que envolve diretamente ‘o mundo real’ deles, como as compras de casa, a fatura do cartão de crédito e como economizar, os alunos ficam bem interessados com o tema.”

- **Pergunta**: “Quais recursos digitais educativos você usa em sala de aula para ensinar os alunos nessa disciplina?”
  - **Resposta**: “Uso principalmente o CMSP – Centro de Mídias, que possui algumas aulas e slides já prontos. Além disso, uso a lousa para passar o conteúdo.”

- **Pergunta**: “Você acredita que uma ferramenta digital, acessada por um computador, que permita ver de forma visual as fórmulas de juros e montante, ajude os alunos a fixar os conceitos aprendidos em sala?”
  - **Resposta**: “Sem dúvidas, os alunos ficam mais engajados quando podem usar o computador nas atividades. Não uso porque não conheço nenhuma ferramenta para essa finalidade, mas seria interessante e usaria com certeza.”

Nome da professora entrevistada: Vanda R. de Camargo, professora na Escola Estadual Giuseppe Pisoni, em Rio Grande da Serra. 
Entrevista realizada no dia 30 de outubro de 2024.

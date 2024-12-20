# Projeto 3 - Computação Gráfica

## Tema

Exploração de Texturas e Iluminação em Ambientes 3D: Ferramenta interativa para renderização realista e manipulação de materiais e fontes de luz

## Introdução

Este projeto tem como tema "Texturas e Iluminação 3D" e busca explorar técnicas de computação gráfica aplicadas à renderização de cenas tridimensionais com foco na criação de ambientes visualmente realistas e interativos.

Cenário de aplicação: O projeto visa criar uma cena 3D onde os usuários possam selecionar diferentes modelos e "pintar" suas faces utilizando o mouse com texturas variadas, além de manipular a iluminação da cena. Esse tipo de interação é útil em cenários educacionais e industriais, como design de produtos, arquitetura e jogos digitais.

Objetivo final: Desenvolver uma aplicação interativa que permita aos usuários manipular texturas e iluminação de forma intuitiva e prática, incentivando o aprendizado e experimentação com conceitos gráficos.

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

## Projeto e desenvolvimento

TODO

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

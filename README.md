## Objetivo

Implementar um nível de jogo contendo áreas
organizadas em uma malha com:

● inimigos (NPCs)

● caixas de primeiros socorros

● caixas de munição

Cada célula da malha deverá ser ativada e desativada a depender a localização do jogador, assim como os inimigos que estão dentro de cada célula.

## Como Compilar

Para compilar o projeto, siga os passos abaixo.

### Pré-requisitos

- **Git**: Para clonar o repositório e seus submódulos.
- **CMake**: Para gerar os arquivos de build.
- **Compilador C++**: Um compilador C++ moderno (como GCC, Clang ou MSVC).
- **Dependências do Raylib**: O Raylib possui algumas dependências que variam de acordo com o sistema operacional. 

Consulte a [página de wiki do raylib](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux) para obter as dependências necessárias.

### Passos para Compilação

1.  **Clone o repositório**

    Use o seguinte comando para clonar o repositório junto com o submódulo do raylib:

    ```bash
    git clone --recurse-submodules https://github.com/ivysonnn/dim0126-ia-jogos-atv1
    cd dim0126-ia-jogos-atv1
    ```

    Caso já tenha clonado o repositório sem a flag `--recurse-submodules`, navegue até o diretório do projeto e execute:

    ```bash
    git submodule init
    git submodule update
    ```

2.  **Crie e acesse o diretório de build**

    ```bash
    mkdir build
    cd build
    ```

3.  **Gere os arquivos de projeto com o CMake**

    ```bash
    cmake ..
    ```

4.  **Compile o código fonte**

    ```bash
    cmake --build .
    ```

5.  **Execute o jogo**

    Após a compilação, o executável estará dentro do diretório `build`. Para executá-lo:

    ```bash
    ./dim0126-ia-jogos-atv1
    ```

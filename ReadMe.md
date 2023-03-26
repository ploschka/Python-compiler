# Компилятор Python

## Сборка

1. Клонируйте репозиторий и перейдите в соданную директорию

    ```sh
    git clone --recurse-submodules https://github.com/ploschka/Python-compiler.git Python-compiler
    cd Python-compiler
    ```

2. Создайте директорию для сборки и перейдите в неё

    ```sh
    mkdir build
    cd build
    ```

3. Соберите проект с помощью `cmake`

    ```sh
    cmake .. && cmake --build .
    ```

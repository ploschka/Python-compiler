# Компилятор Python

## Сборка

1. Клонируйте репозиторий и перейдите в созданную директорию

    ```sh
    git clone --recurse-submodules https://github.com/ploschka/Python-compiler.git Python-compiler
    cd Python-compiler
    ```

2. Сконфигурируйте проект с помощью `cmake`

    ```sh
    cmake -S . -B build
    ```

    Сборка настраивается следующими дополнительными флагами

    |Флаг|Значения|Значение по умолчанию|Описание|
    |----|--------|---------------------|--------|
    |-DPYCOM_BUILD_TESTS=$VALUE|ON; OFF|OFF|Включает и выключает сборку тестов|
    |-DPYCOM_BUILD_EXAMPLES=$VALUE|ON; OFF|OFF|Включает и выключает сборку примеров|
    |-DPYCOM_BUILD_EXECUTABLE=$VALUE|ON; OFF|ON|Включает и выключает сборку исполняемого файла
    |-DCMAKE_BUILD_TYPE=$VALUE|Debug; Release; RelWithDebInfo; MinSizeRel|Debug|Определяет тип сборки. Для детальной информации смотреть [CMAKE_BUILD_TYPE](https://llvm.org/docs/CMake.html#cmake-build-type)|

3. Соберите проект

    ```sh
    cmake --build build
    ```

## Зависимости

- LLVM 17.0.6

## Системные требования

- Архитектура x86 / x86-64

# Ejercicios OpenMP cap4/cap5

This is the development branch.

## Descripción

Este es un repositorio que contiene ejercicios hechos en OpenMP.

### Formas de implementar listas enlazadas en multithreading

1. **Listas enlazadas con un solo mutex**: Utiliza un único mutex para proteger toda la lista, lo que simplifica la implementación pero puede reducir la concurrencia.
2. **Listas enlazadas con un mutex por nodo**: Utiliza un mutex para cada nodo de la lista, permitiendo que múltiples hilos accedan a diferentes partes de la lista simultáneamente.
3. **Listas enlazadas con read-write lock**: Usa un read-write lock para permitir múltiples lectores simultáneos mientras asegura que solo un escritor pueda modificar la lista a la vez.

## Guía de compilación

Para compilar los ejercicios con C++, sigue estos pasos:

1. Abre una terminal.
2. Navega al directorio del ejercicio que deseas compilar.
3. Usa el siguiente comando para compilar el archivo:

    ```sh
    g++ -fopenmp -o nombre_ejecutable nombre_archivo.cpp
    ```

4. Ejecuta el archivo compilado:

    ```sh
    ./nombre_ejecutable
    ```
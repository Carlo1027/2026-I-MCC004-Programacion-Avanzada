# 4 Castro Galindo Carlo André — Tarea: MatrixCarlo, Regex y Threads

## Clase MatrixCarlo

* Implementacion de Copy Constructor y Move Constructor.
* Implementacion de `operator+`, `operator-`, `operator*` matricial y por escalar.
* Implementacion de `operator[]` para indexado `mat[i][j]` y asignacion `mat[i][j] = valor`.
* Demo `DemoMatrixCarlo3`: `m1 = 5*m2 + m3*m4`.

## Expresiones Regulares

* Funcion template `MatrixFromString<T>` que parsea `NxM: n1 n2 ... / fila2 ...` con `std::regex`.
* Demo `DemoMatrixCarloRegex`: lee input del usuario, parsea con regex y aplica `Square` a todos los elementos.

## Threads

* Demo `DemoMatrixCarloThreads`: un `std::thread` por fila aplicando `Square` en paralelo.
* Demo `DemoMatrixCarloThreadsMul`: calcula `C = A * B` en paralelo, cada hilo computa una fila de C.
* Usa `std::mutex` con `lock_guard` para sincronizar el log de progreso entre hilos.

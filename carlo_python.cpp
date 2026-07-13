// carlo_python.cpp
// Ejecuta Python desde C++: lee input del usuario en C++,
// lo pasa a Python que aplica regex para parsear la matriz.
//
// Compilar:
//   c++ -std=c++2b carlo_python.cpp $(python3-config --cflags --ldflags --embed) -o carlo_python
//
// Ejecutar:
//   ./carlo_python

#include <Python.h>
#include <iostream>
#include <string>

int main() {
    // ── 1. Leer input en C++ ───────────────────────────────────────────────
    std::cout << "Ingrese la matriz (formato: NxM: n1 n2 ... / fila2 ...):\n";
    std::string input;
    std::getline(std::cin, input);

    // ── 2. Iniciar el interprete Python ───────────────────────────────────
    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.insert(0, '.')");

    // ── 3. Pasar el string de C++ a Python como variable global ──────────
    // Obtenemos el diccionario global de Python y guardamos el input ahi
    PyObject *globals = PyModule_GetDict(PyImport_AddModule("__main__"));
    PyDict_SetItemString(globals, "texto_matriz", PyUnicode_FromString(input.c_str()));

    // ── 4. Python aplica regex, construye la matriz y opera sobre ella ────
    PyRun_SimpleString(R"(
from carlo_matrix import MatrixCarlo

print("\n=== Procesando en Python (llamado desde C++) ===")
print(f"String recibido desde C++: '{texto_matriz}'")

# from_string usa re.search y re.findall internamente
mat = MatrixCarlo.from_string(texto_matriz)

print("\nMatriz original:")
print(mat)

mat.apply_function_to_all(lambda x: x + 1)
print("Despues de +1 a cada elemento:")
print(mat)

mat.apply_function_to_all(lambda x: x * x)
print("Despues de elevar al cuadrado:")
print(mat)

mat.apply_function_to_all(lambda x, v1, v2: x + v1 + v2, 5, 10, threaded=True)
print("Despues de sumar 5+10 con threads:")
print(mat)
)");

    // ── 5. Cerrar el interprete Python ────────────────────────────────────
    Py_Finalize();

    return 0;
}

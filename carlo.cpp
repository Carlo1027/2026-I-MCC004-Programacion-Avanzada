#include <iostream>
#include <fstream>
#include "carlo.h"
#include "carlo_matrix.h"
#include "types.h"

using namespace std;

// Funciones reutilizables
template <typename T>
void AddOne(T &n) { n += 1; }

template <typename T>
void Square(T &n) { n *= n; }

template <typename T>
void AddX(T &n, T val1, T val2) { n += val1 + val2; }

// Demo #1: leer de teclado, aplicar funciones, guardar en archivo
void DemoMatrixCarlo1() {
    cout << "=== DemoMatrixCarlo1 ===\n";
    cout << "Ingrese filas, columnas y luego los elementos:\n";

    MatrixCarlo<TI> mat;
    cin >> mat;  // lee filas, cols y elementos

    cout << "Matriz original:\n";
    cout << mat;

    // Sumarle 1 a todos los elementos
    cout << "Aplicando AddOne (+1 a cada elemento):\n";
    mat.ApplyFunctionToAll(AddOne<TI>);
    cout << mat;

    // Elevar al cuadrado cada elemento
    cout << "Aplicando Square (cuadrado de cada elemento):\n";
    mat.ApplyFunctionToAll(Square<TI>);
    cout << mat;

    // Guardar resultado en archivo para el Demo2
    ofstream ofs("output_carlo1.txt");
    ofs << mat;
    ofs.close();
    cout << "Resultado guardado en output_carlo1.txt\n\n";
}

// Demo #2: leer desde archivo, aplicar función con args extra
void DemoMatrixCarlo2() {
    cout << "=== DemoMatrixCarlo2 ===\n";

    MatrixCarlo<TI> mat;
    ifstream ifs("output_carlo1.txt");
    ifs >> mat;
    ifs.close();

    cout << "Matriz leida desde output_carlo1.txt:\n";
    cout << mat;

    // Sumar dos valores extra a cada elemento
    cout << "Aplicando AddX(5, 10) a todos los elementos:\n";
    mat.ApplyFunctionToAll(AddX<TI>, 5, 10);
    cout << mat;

    // Guardar resultado final
    ofstream ofs("output_carlo2.txt");
    ofs << mat;
    ofs.close();
    cout << "Resultado guardado en output_carlo2.txt\n";
}

// Demo #3: m1 = 5*m2 + m3*m4
// Usa copy constructor, move assignment, operator*, operator+
void DemoMatrixCarlo3() {
    cout << "=== DemoMatrixCarlo3: m1 = 5*m2 + m3*m4 ===\n";
    cout << "Ingrese m2 (filas cols elementos):\n";
    MatrixCarlo<TI> m2;
    cin >> m2;

    cout << "Ingrese m3 (filas cols elementos):\n";
    MatrixCarlo<TI> m3;
    cin >> m3;

    cout << "Ingrese m4 (filas cols elementos — filas debe coincidir con cols de m3):\n";
    MatrixCarlo<TI> m4;
    cin >> m4;

    MatrixCarlo<TI> m1;
    m1 = 5 * m2 + m3 * m4;

    cout << "m1 = 5*m2 + m3*m4:\n";
    cout << m1;
}

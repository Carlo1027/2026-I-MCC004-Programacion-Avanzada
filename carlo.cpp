#include <iostream>
#include <fstream>
#include <regex>
#include <thread>
#include <mutex>
#include <vector>
#include <sstream>
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

// ── MatrixFromString: parsea "NxM: n1 n2 ... / n3 n4 ..." con std::regex ──
// Recibe un string con la matriz en formato texto y retorna un MatrixCarlo<T>.
template <typename T>
MatrixCarlo<T> MatrixFromString(const string &text) {
    // Define el patron para capturar NxM: dos numeros separados por 'x'/'X'
    // (\d+) captura digitos, \s* permite espacios entre ellos
    regex dim_re(R"((\d+)\s*[xX]\s*(\d+))");
    smatch dim_match;

    // Busca el patron en el string y llena dim_match con el resultado
    regex_search(text, dim_match, dim_re);
    assert(dim_match.ready() && "Formato invalido: se espera NxM");

    // Extrae filas y columnas de los grupos de captura 1 y 2
    size_t rows = stoul(dim_match[1].str());
    size_t cols = stoul(dim_match[2].str());

    // Toma solo el texto despues del "NxM" para buscar los numeros de la matriz
    string rest = text.substr(dim_match.position(0) + dim_match.length(0));

    // Patron para encontrar enteros incluyendo negativos (-?\d+)
    regex num_re(R"(-?\d+)");

    // sregex_iterator recorre todas las coincidencias del patron en el string
    // equivalente al re.findall() de Python
    auto it  = sregex_iterator(rest.begin(), rest.end(), num_re);
    auto end = sregex_iterator();

    // Arma "rows cols n1 n2 ..." que es el formato que espera operator>> de MatrixCarlo
    ostringstream oss;
    oss << rows << " " << cols;
    for (; it != end; ++it)
        oss << " " << it->str();

    // Crea la matriz y la llena leyendo desde el string construido
    MatrixCarlo<T> mat;
    istringstream iss(oss.str());
    iss >> mat;
    return mat;
}

// ── DemoMatrixCarloRegex: lee input del usuario y parsea con regex ─────────
void DemoMatrixCarloRegex() {
    cout << "=== DemoMatrixCarloRegex: parsear string con std::regex ===\n";
    cout << "Ingrese la matriz (formato: NxM: n1 n2 ... / fila2 ...):\n";

    // getline lee la linea completa incluyendo espacios y el separador '/'
    string linea;
    getline(cin, linea);

    // MatrixFromString aplica regex internamente y retorna la matriz lista
    MatrixCarlo<TI> mat = MatrixFromString<TI>(linea);

    cout << "Matriz parseada con regex:\n";
    cout << mat;

    // Aplica Square usando la funcion variadic template del profesor
    cout << "Aplicando Square a todos los elementos:\n";
    mat.ApplyFunctionToAll(Square<TI>);
    cout << mat;
}

// ── DemoMatrixCarloThreads: un std::thread por fila en paralelo ────────────
void DemoMatrixCarloThreads() {
    cout << "=== DemoMatrixCarloThreads: un std::thread por fila ===\n";
    cout << "Ingrese la matriz (formato: NxM: n1 n2 ... / fila2 ...):\n";

    string linea;
    getline(cin, linea);

    // Parsea con regex igual que DemoMatrixCarloRegex
    MatrixCarlo<TI> mat = MatrixFromString<TI>(linea);

    cout << "Matriz original:\n";
    cout << mat;

    // Vector que almacena un hilo por cada fila de la matriz
    vector<thread> threads;

    for (size_t i = 0; i < mat.rows(); ++i)
        // emplace_back construye el hilo directamente en el vector
        // La lambda captura &mat por referencia e i por valor
        // Cada hilo trabaja solo en su fila i — sin conflicto entre hilos
        threads.emplace_back([&mat, i]() {
            for (size_t j = 0; j < mat.cols(); ++j)
                Square(mat[i][j]);  // modifica el elemento directamente en memoria
        });

    // join() espera a que cada hilo termine antes de continuar
    for (auto &t : threads)
        t.join();

    cout << "Despues de Square con " << mat.rows() << " threads (uno por fila):\n";
    cout << mat;
}

// ── DemoMatrixCarloThreadsMul: multiplicacion matricial en paralelo ─────────
// Calcula C = A * B donde cada hilo computa una fila completa de C.
// Usa std::mutex para proteger el log de progreso — demuestra sincronizacion
// entre hilos ademas del paralelismo.
void DemoMatrixCarloThreadsMul() {
    cout << "=== DemoMatrixCarloThreadsMul: C = A*B con un hilo por fila ===\n";

    cout << "Ingrese A (formato: NxM: n1 n2 ... / fila2 ...):\n";
    string lineaA; getline(cin, lineaA);
    MatrixCarlo<TI> A = MatrixFromString<TI>(lineaA);

    cout << "Ingrese B (filas de B debe coincidir con columnas de A):\n";
    string lineaB; getline(cin, lineaB);
    MatrixCarlo<TI> B = MatrixFromString<TI>(lineaB);

    assert(A.cols() == B.rows() && "Dimensiones incompatibles para multiplicar");

    // Matriz resultado C inicializada en ceros (filas de A x cols de B)
    // Init() llama Destroy + asigna dimensiones + Create internamente
    MatrixCarlo<TI> C;
    C.Init(A.rows(), B.cols());

    // mutex protege cout — sin el, los mensajes de distintos hilos se mezclan
    mutex mtx_log;

    vector<thread> threads;

    for (size_t i = 0; i < A.rows(); ++i) {
        // Cada hilo recibe su indice de fila i por valor (captura segura)
        // A y B se capturan por referencia — son de solo lectura, no hay race condition
        // C se captura por referencia — cada hilo escribe solo en su fila i
        threads.emplace_back([&A, &B, &C, &mtx_log, i]() {

            // Log de inicio: lock_guard adquiere el mutex y lo libera al salir del scope
            { lock_guard<mutex> lock(mtx_log);
              cout << "  hilo " << i << " iniciando fila " << i << "\n"; }

            // Calculo del producto punto para cada elemento C[i][j]
            for (size_t j = 0; j < B.cols(); ++j) {
                C[i][j] = TI{};                          // inicializa en cero
                for (size_t k = 0; k < A.cols(); ++k)
                    C[i][j] += A[i][k] * B[k][j];       // acumulacion del producto punto
            }

            // Log de fin: otro lock_guard para proteger la escritura en cout
            { lock_guard<mutex> lock(mtx_log);
              cout << "  hilo " << i << " termino fila " << i << "\n"; }
        });
    }

    // Esperar a que todos los hilos terminen antes de imprimir C
    for (auto &t : threads)
        t.join();

    cout << "A:\n" << A;
    cout << "B:\n" << B;
    cout << "C = A * B (calculado en paralelo):\n" << C;

    // Verificacion: calcula C con el operator* normal y compara
    MatrixCarlo<TI> C_check = A * B;
    cout << "Verificacion con operator* secuencial:\n" << C_check;
}

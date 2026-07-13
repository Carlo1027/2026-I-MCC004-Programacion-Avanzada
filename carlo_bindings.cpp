// carlo_bindings.cpp
// Expone MatrixCarlo<T> de C++ como modulo Python usando pybind11.
// Soporta multiples tipos (int, float, double) igual que el template de C++.
//
// Instalar pybind11:
//   pip install pybind11 --break-system-packages
//
// Compilar:
//   c++ -O3 -shared -fPIC -std=c++2b \
//       $(python3 -m pybind11 --includes) \
//       carlo_bindings.cpp \
//       -o carlo_module$(python3-config --extension-suffix)
//
// Usar en Python:
//   import carlo_module
//   m = carlo_module.MatrixCarloInt()      # MatrixCarlo<int>
//   m = carlo_module.MatrixCarloFloat()    # MatrixCarlo<float>
//   m = carlo_module.MatrixCarloDouble()   # MatrixCarlo<double>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include "carlo_matrix.h"
#include "types.h"

namespace py = pybind11;

// ── RowProxy templado: permite m[i][j] y m[i][j] = valor ─────────────────
template <typename T>
struct RowProxy {
    MatrixCarlo<T> &mat;
    size_t           row;
    RowProxy(MatrixCarlo<T> &mat, size_t row) : mat(mat), row(row) {}

    T    get(size_t j)          const { return mat[row][j]; }
    void set(size_t j, T value)       { mat[row][j] = value; }
};

// ── Funcion template: registra MatrixCarlo<T> con el nombre dado ──────────
template <typename T>
void bind_matrix(py::module &m, const TC *class_name, const TC *proxy_name) {
    using Mat   = MatrixCarlo<T>;
    using Proxy = RowProxy<T>;

    // Registrar RowProxy para este tipo
    py::class_<Proxy>(m, proxy_name)
        .def("__getitem__", &Proxy::get,  py::arg("j"))
        .def("__setitem__", &Proxy::set,  py::arg("j"), py::arg("value"));

    // Registrar MatrixCarlo<T>
    py::class_<Mat>(m, class_name)

        // ── Constructores ────────────────────────────────────────────────
        .def(py::init<>(),
             "Constructor default.")

        .def(py::init<const Mat &>(), py::arg("other"),
             "Copy constructor — copia profunda fila por fila.")

        // ── I/O ──────────────────────────────────────────────────────────
        .def("read", [](Mat &self) { std::cin >> self; },
             "Lee desde stdin (cin >> mat).")

        .def("read_from_string", [](Mat &self, const std::string &s) {
                std::istringstream iss(s); iss >> self;
             }, py::arg("text"),
             "Lee desde string 'rows cols n1 n2 ...'.")

        .def("print", [](Mat &self) { self.Print(std::cout); },
             "Imprime en stdout.")

        .def("__str__", [](Mat &self) {
                std::ostringstream oss; self.Print(oss); return oss.str();
             })

        .def("__repr__", [](Mat &self) {
                std::ostringstream oss; self.Print(oss);
                return std::string("<") + typeid(T).name() + " MatrixCarlo:\n" + oss.str() + ">";
             })

        // ── Destroy ──────────────────────────────────────────────────────
        .def("destroy", &Mat::Destroy,
             "Libera la memoria (delete[] en C++).")

        // ── Propiedades ──────────────────────────────────────────────────
        .def_property_readonly("rows", &Mat::rows, "Numero de filas.")
        .def_property_readonly("cols", &Mat::cols, "Numero de columnas.")

        // ── Indexado m[i][j] y m[i][j] = valor ──────────────────────────
        .def("__getitem__",
             [](Mat &self, size_t i) { return Proxy(self, i); }, py::arg("i"),
             "Retorna RowProxy. Habilita m[i][j] y m[i][j] = valor.")

        .def("get", [](Mat &self, size_t i, size_t j) { return self[i][j]; },
             py::arg("i"), py::arg("j"))

        .def("set", [](Mat &self, size_t i, size_t j, T value) { self[i][j] = value; },
             py::arg("i"), py::arg("j"), py::arg("value"))

        // ── apply_function_to_all ────────────────────────────────────────
        .def("apply_function_to_all",
             [](Mat &self, py::function func) {
                 self.ApplyFunctionToAll([&func](T &n) {
                     n = func(n).template cast<T>();
                 });
             }, py::arg("func"),
             "Aplica func(elemento) a cada celda. Ej: lambda x: x*x")

        .def("apply_function_to_all_with_args",
             [](Mat &self, py::function func, T val1, T val2) {
                 self.ApplyFunctionToAll([&func, val1, val2](T &n) {
                     n = func(n, val1, val2).template cast<T>();
                 });
             }, py::arg("func"), py::arg("val1"), py::arg("val2"),
             "Aplica func(elemento, val1, val2) a cada celda.")

        // ── Operadores aritmeticos ────────────────────────────────────────
        .def("__add__", &Mat::operator+)
        .def("__sub__", &Mat::operator-)
        .def("__mul__",
             [](const Mat &a, const Mat &b) { return a * b; }, py::arg("other"))
        .def("__mul__",
             [](const Mat &a, T scalar)     { return a * scalar; }, py::arg("scalar"))
        .def("__rmul__",
             [](const Mat &a, T scalar)     { return a * scalar; }, py::arg("scalar"));
}

// ── Helper template: registra from_string<T> con el nombre dado ──────────
template <typename T>
void bind_from_string(py::module &m, const TC *name) {
    m.def(name, [](const std::string &s) {
        MatrixCarlo<T> mat;
        std::istringstream iss(s); iss >> mat; return mat;
    }, py::arg("text"));
}

// ── Modulo principal ──────────────────────────────────────────────────────
PYBIND11_MODULE(carlo_module, m) {
    m.doc() = "MatrixCarlo<T> de C++ expuesto a Python via pybind11. "
              "Tipos definidos en types.h: TI, T3F, T5.";

    // Instanciar usando los alias de types.h — sin tipos nativos hardcodeados
    bind_matrix<TI> (m, "MatrixCarloTI",  "RowProxyTI");   // int
    bind_matrix<T3F>(m, "MatrixCarloT3F", "RowProxyT3F");  // float
    bind_matrix<T5> (m, "MatrixCarloT5",  "RowProxyT5");   // double

    // from_string por tipo — tambien via alias de types.h
    bind_from_string<TI> (m, "from_string_TI");
    bind_from_string<T3F>(m, "from_string_T3F");
    bind_from_string<T5> (m, "from_string_T5");
}

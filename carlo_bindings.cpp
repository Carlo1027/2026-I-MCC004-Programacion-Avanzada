// carlo_bindings.cpp
// Expone MatrixCarlo<int> de C++ como modulo Python usando pybind11.
//
// Instalar pybind11:
//   pip install pybind11   (o: python3 -m pip install pybind11 --break-system-packages)
//
// Compilar (desde la carpeta del proyecto):
//   c++ -O3 -shared -fPIC -std=c++2b \
//       $(python3 -m pybind11 --includes) \
//       carlo_bindings.cpp \
//       -o carlo_module$(python3-config --extension-suffix)
//
// Usar en Python:
//   import carlo_module
//   m = carlo_module.MatrixCarlo()
//   m.read_from_string("2 2  1 2  3 4")
//   print(m)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include <vector>
#include "carlo_matrix.h"
#include "types.h"

namespace py = pybind11;
using Mat = MatrixCarlo<TI>;

PYBIND11_MODULE(carlo_module, m) {
    m.doc() = "Modulo Python generado desde MatrixCarlo<int> de C++ via pybind11";

    py::class_<Mat>(m, "MatrixCarlo")

        // ── Constructores ──────────────────────────────────────────────────
        .def(py::init<>(),
             "Constructor default — equivale a MatrixCarlo() en C++.")

        .def(py::init<const Mat &>(),
             py::arg("other"),
             "Copy constructor — copia profunda fila por fila.")

        // ── I/O ────────────────────────────────────────────────────────────
        .def("read", [](Mat &self) {
                std::cin >> self;
             },
             "Lee filas, columnas y elementos desde stdin (cin >> mat).")

        .def("read_from_string", [](Mat &self, const std::string &s) {
                std::istringstream iss(s);
                iss >> self;
             },
             py::arg("text"),
             "Lee la matriz desde un string con formato 'rows cols n1 n2 ...'.")

        .def("print", [](Mat &self) {
                self.Print(std::cout);
             },
             "Imprime la matriz en stdout (cout << mat).")

        .def("__str__", [](Mat &self) {
                std::ostringstream oss;
                self.Print(oss);
                return oss.str();
             },
             "Representacion en string — equivale a operator<< en C++.")

        .def("__repr__", [](Mat &self) {
                std::ostringstream oss;
                self.Print(oss);
                return "<MatrixCarlo C++:\n" + oss.str() + ">";
             })

        // ── Destroy ────────────────────────────────────────────────────────
        .def("destroy", &Mat::Destroy,
             "Libera la memoria (equivale a Destroy() / delete[] en C++).")

        // ── Propiedades ────────────────────────────────────────────────────
        .def_property_readonly("rows", &Mat::rows, "Numero de filas.")
        .def_property_readonly("cols", &Mat::cols, "Numero de columnas.")

        // ── Indexado m[i][j] ───────────────────────────────────────────────
        // Retorna la fila i como lista Python (copia, no referencia)
        .def("__getitem__",
             [](Mat &self, int i) -> py::list {
                 py::list row;
                 for (size_t j = 0; j < self.cols(); ++j)
                     row.append(self[i][j]);
                 return row;
             },
             py::arg("i"),
             "Retorna la fila i como lista. Permite mat[i][j].")

        .def("set",
             [](Mat &self, int i, int j, TI value) {
                 self[i][j] = value;
             },
             py::arg("i"), py::arg("j"), py::arg("value"),
             "Asigna value a [i][j]. Uso: mat.set(0, 0, 8)  — equivale a mat[0][0]=8.")

        .def("get",
             [](Mat &self, int i, int j) -> TI {
                 return self[i][j];
             },
             py::arg("i"), py::arg("j"),
             "Retorna el elemento [i][j]. Uso: mat.get(0, 0).")

        // ── apply_function_to_all ──────────────────────────────────────────
        .def("apply_function_to_all",
             [](Mat &self, py::function func) {
                 self.ApplyFunctionToAll([&func](TI &n) {
                     n = func(n).cast<TI>();
                 });
             },
             py::arg("func"),
             "Aplica func(elemento) a cada celda.\n"
             "Equivale a ApplyFunctionToAll(Func func, Args...) en C++.\n"
             "Ejemplo: mat.apply_function_to_all(lambda x: x * x)")

        .def("apply_function_to_all_with_args",
             [](Mat &self, py::function func, TI val1, TI val2) {
                 self.ApplyFunctionToAll([&func, val1, val2](TI &n) {
                     n = func(n, val1, val2).cast<TI>();
                 });
             },
             py::arg("func"), py::arg("val1"), py::arg("val2"),
             "Aplica func(elemento, val1, val2) a cada celda (version con args extra).")

        // ── Operadores aritmeticos ─────────────────────────────────────────
        .def("__add__", &Mat::operator+,
             "Suma elemento a elemento — equivale a operator+ en C++.")

        .def("__sub__", &Mat::operator-,
             "Resta elemento a elemento — equivale a operator- en C++.")

        .def("__mul__",
             [](const Mat &a, const Mat &b) { return a * b; },
             py::arg("other"),
             "Multiplicacion matricial — equivale a operator*(Matrix) en C++.")

        .def("__mul__",
             [](const Mat &a, TI scalar) { return a * scalar; },
             py::arg("scalar"),
             "Multiplicacion por escalar — equivale a operator*(T value) en C++.")

        .def("__rmul__",
             [](const Mat &a, TI scalar) { return a * scalar; },
             py::arg("scalar"),
             "Multiplicacion escalar a la izquierda: 5 * mat.\n"
             "Equivale a la funcion libre operator*(T, MatrixCarlo) en C++.");

    // ── Funcion de conveniencia: crear matriz desde string ─────────────────
    m.def("from_string", [](const std::string &s) {
        Mat mat;
        std::istringstream iss(s);
        iss >> mat;
        return mat;
    }, py::arg("text"),
    "Crea MatrixCarlo desde string 'rows cols n1 n2 ...'. Uso: carlo_module.from_string('2 2 1 2 3 4')");
}

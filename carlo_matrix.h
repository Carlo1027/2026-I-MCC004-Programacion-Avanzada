#ifndef __CARLO_MATRIX_H__
#define __CARLO_MATRIX_H__

#include <cassert>
#include <functional>
#include <iostream>
#include <fstream>
#include <utility>

using namespace std;

/// @brief Imprime un elemento en un ostream. Funcion auxiliar para debugging.
/// @tparam T Tipo del elemento a imprimir.
/// @param n Elemento a imprimir.
/// @param os Stream de salida destino.
template <typename T>
void PrintMatEl(T &n, ostream &os) { os << n << " "; }

/**
 * @brief Matriz dinamica 2D generica con gestion manual de memoria.
 *
 * Implementa el Rule of Five: constructor default, copy constructor,
 * move constructor, copy assignment y move assignment.
 * Internamente usa un doble puntero T** con new/delete manual.
 *
 * @tparam T Tipo de los elementos de la matriz (ej: TI, T5, T3F de types.h).
 *
 * @example
 * @code
 * MatrixCarlo<TI> m;
 * cin >> m;                         // lee filas, cols y elementos
 * m.ApplyFunctionToAll(Square<TI>); // aplica funcion a cada elemento
 * cout << m;                        // imprime la matriz
 * @endcode
 */
template <typename T>
class MatrixCarlo {
    private:
        T      **m_pMat = nullptr; ///< Doble puntero: arreglo de punteros a filas.
        size_t   m_rows = 0;       ///< Numero de filas.
        size_t   m_cols = 0;       ///< Numero de columnas.

        /// @brief Reserva memoria para la matriz. Requiere m_rows y m_cols > 0.
        void Create() {
            assert(m_rows > 0 && m_cols > 0);
            m_pMat = new T *[m_rows];
            for (size_t i = 0; i < m_rows; ++i)
                m_pMat[i] = new T[m_cols];
        }

    public:
        /// @brief Constructor default. Crea una matriz vacia sin memoria reservada.
        MatrixCarlo() { }

        /**
         * @brief Copy constructor. Copia profunda elemento a elemento.
         * @param other Matriz fuente a copiar.
         */
        MatrixCarlo(const MatrixCarlo &other) {
            m_rows = other.m_rows;
            m_cols = other.m_cols;
            Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    m_pMat[i][j] = other.m_pMat[i][j];
        }

        /**
         * @brief Move constructor. Transfiere la memoria sin copiar usando std::exchange.
         * @param other Matriz fuente que queda vacia tras el movimiento.
         */
        MatrixCarlo(MatrixCarlo &&other) {
            m_pMat = std::exchange(other.m_pMat, nullptr);
            m_rows = std::exchange(other.m_rows, 0);
            m_cols = std::exchange(other.m_cols, 0);
        }

        /// @brief Destructor. Llama a Destroy() para liberar memoria.
        ~MatrixCarlo() { Destroy(); }

        /**
         * @brief Copy assignment. Libera memoria propia y copia desde other.
         * @param other Matriz fuente.
         * @return Referencia a this.
         */
        MatrixCarlo &operator=(const MatrixCarlo &other) {
            if (this == &other) return *this;
            Destroy();
            m_rows = other.m_rows;
            m_cols = other.m_cols;
            Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    m_pMat[i][j] = other.m_pMat[i][j];
            return *this;
        }

        /**
         * @brief Move assignment. Libera memoria propia y transfiere desde other.
         * @param other Matriz fuente que queda vacia.
         * @return Referencia a this.
         */
        MatrixCarlo &operator=(MatrixCarlo &&other) {
            if (this == &other) return *this;
            Destroy();
            m_pMat = std::exchange(other.m_pMat, nullptr);
            m_rows = std::exchange(other.m_rows, 0);
            m_cols = std::exchange(other.m_cols, 0);
            return *this;
        }

        /**
         * @brief Lee filas, columnas y elementos desde un istream.
         * @param is Stream de entrada (cin o ifstream).
         * @return Referencia al stream para encadenamiento.
         */
        istream &Read(istream &is) {
            is >> m_rows >> m_cols;
            Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    is >> m_pMat[i][j];
            return is;
        }

        /**
         * @brief Aplica una funcion a cada elemento de la matriz.
         *
         * Usa templates variadicos para pasar argumentos extra a func,
         * igual que std::forward en la implementacion del profesor.
         *
         * @tparam Func Tipo del callable (funcion, lambda, functor).
         * @tparam Args Tipos de los argumentos extra opcionales.
         * @param func Funcion que recibe el elemento por referencia y lo modifica.
         * @param args Argumentos extra que se reenvian a func.
         */
        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args) {
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    func(m_pMat[i][j], std::forward<Args>(args)...);
        }

        /**
         * @brief Imprime la matriz en un ostream.
         *
         * Formato: primera linea "rows cols", luego una fila por linea.
         *
         * @param os Stream de salida (cout o ofstream).
         * @return Referencia al stream para encadenamiento.
         */
        ostream &Print(ostream &os) {
            os << m_rows << " " << m_cols << "\n";
            for (size_t i = 0; i < m_rows; ++i) {
                for (size_t j = 0; j < m_cols; ++j)
                    os << m_pMat[i][j] << " ";
                os << "\n";
            }
            return os;
        }

        /**
         * @brief Suma elemento a elemento. Requiere mismas dimensiones.
         * @param other Matriz operando derecho.
         * @return Nueva matriz resultado.
         */
        MatrixCarlo operator+(const MatrixCarlo &other) const {
            assert(m_rows == other.m_rows && m_cols == other.m_cols);
            MatrixCarlo result;
            result.m_rows = m_rows;
            result.m_cols = m_cols;
            result.Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    result.m_pMat[i][j] = m_pMat[i][j] + other.m_pMat[i][j];
            return result;
        }

        /**
         * @brief Resta elemento a elemento. Requiere mismas dimensiones.
         * @param other Matriz operando derecho.
         * @return Nueva matriz resultado.
         */
        MatrixCarlo operator-(const MatrixCarlo &other) const {
            assert(m_rows == other.m_rows && m_cols == other.m_cols);
            MatrixCarlo result;
            result.m_rows = m_rows;
            result.m_cols = m_cols;
            result.Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    result.m_pMat[i][j] = m_pMat[i][j] - other.m_pMat[i][j];
            return result;
        }

        /**
         * @brief Multiplicacion matricial clasica con tres loops.
         *
         * Requiere que this->cols() == other.rows().
         *
         * @param other Matriz operando derecho.
         * @return Nueva matriz de dimensiones this->rows() x other.cols().
         */
        MatrixCarlo operator*(const MatrixCarlo &other) const {
            assert(m_cols == other.m_rows);
            MatrixCarlo result;
            result.m_rows = m_rows;
            result.m_cols = other.m_cols;
            result.Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < other.m_cols; ++j) {
                    result.m_pMat[i][j] = T{};
                    for (size_t k = 0; k < m_cols; ++k)
                        result.m_pMat[i][j] += m_pMat[i][k] * other.m_pMat[k][j];
                }
            return result;
        }

        /**
         * @brief Multiplicacion por escalar: mat * valor.
         * @param value Escalar multiplicador.
         * @return Nueva matriz con cada elemento multiplicado por value.
         */
        MatrixCarlo operator*(T value) const {
            MatrixCarlo result;
            result.m_rows = m_rows;
            result.m_cols = m_cols;
            result.Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    result.m_pMat[i][j] = m_pMat[i][j] * value;
            return result;
        }

        /**
         * @brief Inicializa la matriz con dimensiones dadas y memoria en ceros.
         *
         * Libera memoria previa si existe. Util para construir la matriz
         * resultado antes de llenarla en paralelo con threads.
         *
         * @param rows Numero de filas.
         * @param cols Numero de columnas.
         */
        void Init(size_t rows, size_t cols) {
            Destroy();
            m_rows = rows; m_cols = cols;
            Create();
        }

        /**
         * @brief Acceso por fila. Permite mat[i][j] y mat[i][j] = valor.
         * @param i Indice de fila.
         * @return Puntero a la fila i (acceso directo a memoria).
         */
        T       *operator[](size_t i)       { return m_pMat[i]; }
        /// @copydoc operator[](size_t)
        const T *operator[](size_t i) const { return m_pMat[i]; }

        /// @brief Retorna el numero de filas.
        size_t rows() const { return m_rows; }

        /// @brief Retorna el numero de columnas.
        size_t cols() const { return m_cols; }

        /// @brief Libera toda la memoria reservada y resetea el estado.
        void Destroy() {
            if (m_pMat != nullptr) {
                for (size_t i = 0; i < m_rows; ++i)
                    delete[] m_pMat[i];
                delete[] m_pMat;
                m_pMat = nullptr;
            }
        }
};

/**
 * @brief Multiplicacion escalar a la izquierda: valor * mat.
 * @tparam T Tipo de los elementos.
 * @param value Escalar multiplicador.
 * @param mat Matriz operando.
 * @return Nueva matriz resultado.
 */
template <typename T>
MatrixCarlo<T> operator*(T value, const MatrixCarlo<T> &mat) {
    return mat * value;
}

/// @brief Lee una matriz desde un istream usando operator>>.
template <typename T>
istream &operator>>(istream &is, MatrixCarlo<T> &mat) {
    return mat.Read(is);
}

/// @brief Imprime una matriz en un ostream usando operator<<.
template <typename T>
ostream &operator<<(ostream &os, MatrixCarlo<T> &mat) {
    return mat.Print(os);
}


#endif // __CARLO_MATRIX_H__

#ifndef __CARLO_MATRIX_H__
#define __CARLO_MATRIX_H__

#include <cassert>
#include <functional>
#include <iostream>
#include <fstream>

using namespace std;

// Función auxiliar: imprime un elemento en un ostream
template <typename T>
void PrintMatEl(T &n, ostream &os) { os << n << " "; }

template <typename T>
class MatrixCarlo {
    private:
        T      **m_pMat = nullptr;
        size_t   m_rows = 0, m_cols = 0;

        void Create() {
            assert(m_rows > 0 && m_cols > 0);
            m_pMat = new T *[m_rows];
            for (size_t i = 0; i < m_rows; ++i)
                m_pMat[i] = new T[m_cols];
        }

    public:
        MatrixCarlo()  { }
        ~MatrixCarlo() { Destroy(); }

        // Lee: primero filas y cols, luego los elementos fila por fila
        istream &Read(istream &is) {
            is >> m_rows >> m_cols;
            Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    is >> m_pMat[i][j];
            return is;
        }

        // Aplica func a todos los elementos con argumentos extra opcionales
        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args) {
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    func(m_pMat[i][j], std::forward<Args>(args)...);
        }

        // Imprime: "filas cols\n" seguido de cada fila en una línea
        ostream &Print(ostream &os) {
            os << m_rows << " " << m_cols << "\n";
            for (size_t i = 0; i < m_rows; ++i) {
                for (size_t j = 0; j < m_cols; ++j)
                    os << m_pMat[i][j] << " ";
                os << "\n";
            }
            return os;
        }

        void Destroy() {
            if (m_pMat != nullptr) {
                for (size_t i = 0; i < m_rows; ++i)
                    delete[] m_pMat[i];
                delete[] m_pMat;
                m_pMat = nullptr;
            }
        }
};

// Operadores >> y <<
template <typename T>
istream &operator>>(istream &is, MatrixCarlo<T> &mat) {
    return mat.Read(is);
}

template <typename T>
ostream &operator<<(ostream &os, MatrixCarlo<T> &mat) {
    return mat.Print(os);
}

#endif // __CARLO_MATRIX_H__

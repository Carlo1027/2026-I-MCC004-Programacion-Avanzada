#ifndef __CARLO_MATRIX_H__
#define __CARLO_MATRIX_H__

#include <cassert>
#include <functional>
#include <iostream>
#include <fstream>
#include <utility>

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
        MatrixCarlo() { }

        // Copy constructor
        MatrixCarlo(const MatrixCarlo &other) {
            m_rows = other.m_rows;
            m_cols = other.m_cols;
            Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    m_pMat[i][j] = other.m_pMat[i][j];
        }

        // Move constructor
        MatrixCarlo(MatrixCarlo &&other) {
            m_pMat = std::exchange(other.m_pMat, nullptr);
            m_rows = std::exchange(other.m_rows, 0);
            m_cols = std::exchange(other.m_cols, 0);
        }

        ~MatrixCarlo() { Destroy(); }

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

        MatrixCarlo &operator=(MatrixCarlo &&other) {
            if (this == &other) return *this;
            Destroy();
            m_pMat = std::exchange(other.m_pMat, nullptr);
            m_rows = std::exchange(other.m_rows, 0);
            m_cols = std::exchange(other.m_cols, 0);
            return *this;
        }

        istream &Read(istream &is) {
            is >> m_rows >> m_cols;
            Create();
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    is >> m_pMat[i][j];
            return is;
        }

        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args) {
            for (size_t i = 0; i < m_rows; ++i)
                for (size_t j = 0; j < m_cols; ++j)
                    func(m_pMat[i][j], std::forward<Args>(args)...);
        }

        ostream &Print(ostream &os) {
            os << m_rows << " " << m_cols << "\n";
            for (size_t i = 0; i < m_rows; ++i) {
                for (size_t j = 0; j < m_cols; ++j)
                    os << m_pMat[i][j] << " ";
                os << "\n";
            }
            return os;
        }

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

        // Multiplicación por escalar: mat * valor
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

        void Destroy() {
            if (m_pMat != nullptr) {
                for (size_t i = 0; i < m_rows; ++i)
                    delete[] m_pMat[i];
                delete[] m_pMat;
                m_pMat = nullptr;
            }
        }
};

// Multiplicación por escalar: valor * mat
template <typename T>
MatrixCarlo<T> operator*(T value, const MatrixCarlo<T> &mat) {
    return mat * value;
}

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

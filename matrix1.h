#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <functional>
#include <iostream>

using namespace std;

template <typename T>
void Print4(T &n, ostream &os) { os << n << " "; }

template <typename T>
class Matrix1 {
    private:
        T      **m_pMat = nullptr;
        size_t   m_rows = 0, m_cols = 0;
    public:
        Matrix1()      { }
        Matrix1(Matrix1 &other) = delete; // No copy constructor
        Matrix1(Matrix1 &&other); 
        ~Matrix1()     { Destroy(); }
        void     Create();
        istream &Read(istream &is);
        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args);
        ostream &Print(ostream &os);
        void Destroy();
};

template <typename T>
void Matrix1<T>::Create()
{   assert(m_rows > 0 && m_cols > 0);
    m_pMat = new T *[m_rows];
    for(size_t i = 0 ; i < m_rows ; ++i)
        m_pMat[i] = new T[m_cols];
}

// Move constructor
template <typename T>
Matrix1<T>::Matrix1(Matrix1 &&other) {
    m_pMat = exchange(other.m_pMat, nullptr);
    m_rows = exchange(other.m_rows, 0);
    m_cols = exchange(other.m_cols, 0);
}

#endif // __MATRIX_H__
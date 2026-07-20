// carlo_metal.metal
// Kernel de multiplicacion matricial C = A * B en GPU.
// Cada hilo calcula un elemento C[row][col].

#include <metal_stdlib>
using namespace metal;

/// @brief Multiplicacion matricial C = A * B en paralelo.
/// Cada hilo tiene su propio (row, col) y calcula C[row][col] completo.
/// @param A      Matriz A aplanada en fila mayor (row-major).
/// @param B      Matriz B aplanada en fila mayor.
/// @param C      Matriz resultado C (escritura).
/// @param N      Dimension de las matrices (NxN).
/// @param pos    Posicion 2D del hilo: x=columna, y=fila.
kernel void matmul_kernel(
    device const float *A   [[buffer(0)]],
    device const float *B   [[buffer(1)]],
    device       float *C   [[buffer(2)]],
    constant     uint  &N   [[buffer(3)]],
    uint2               pos [[thread_position_in_grid]])
{
    uint row = pos.y;
    uint col = pos.x;

    if (row >= N || col >= N) return;  // hilo fuera del rango

    // Producto punto de la fila row de A con la columna col de B
    float sum = 0.0f;
    for (uint k = 0; k < N; ++k)
        sum += A[row * N + k] * B[k * N + col];

    C[row * N + col] = sum;
}

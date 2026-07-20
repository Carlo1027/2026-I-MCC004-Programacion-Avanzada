// carlo_metal.mm  (Objective-C++ — requiere extension .mm)
// Compara multiplicacion matricial NxN en CPU (secuencial) vs GPU (Metal).
//
// Compilar:
//   xcrun metal -c carlo_metal.metal -o carlo_metal.air
//   xcrun metallib carlo_metal.air -o carlo_metal.metallib
//   clang++ -std=c++17 carlo_metal.mm \
//       -framework Metal -framework Foundation -framework CoreFoundation \
//       -o carlo_metal
// Ejecutar:
//   ./carlo_metal

#include <Foundation/Foundation.h>
#include <Metal/Metal.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include "types.h"

using TGpu   = T3F;                    // float — tipo de datos de la GPU
using TClock = std::chrono::high_resolution_clock;

static const U1 N = 512;               // dimension de las matrices (512x512)
static const size_t TOTAL = N * N;

// ── Multiplicacion matricial CPU (secuencial) ─────────────────────────────
/**
 * @brief Multiplicacion matricial C = A * B en CPU con tres loops.
 * @param A Matriz A aplanada (N*N).
 * @param B Matriz B aplanada (N*N).
 * @param C Matriz resultado (N*N).
 */
void matmul_cpu(const std::vector<TGpu> &A,
                const std::vector<TGpu> &B,
                std::vector<TGpu>       &C) {
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j) {
            TGpu sum = 0;
            for (size_t k = 0; k < N; ++k)
                sum += A[i * N + k] * B[k * N + j];
            C[i * N + j] = sum;
        }
}

// ── Main ──────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== Carlo Metal: C = A*B  (" << N << "x" << N << ")  CPU vs GPU ===\n\n";

    // ── 1. Generar matrices aleatorias ────────────────────────────────────
    std::mt19937 rng(42);
    std::uniform_real_distribution<TGpu> dist(0.0f, 1.0f);

    std::vector<TGpu> A(TOTAL), B(TOTAL), C_cpu(TOTAL, 0), C_gpu(TOTAL, 0);
    for (auto &x : A) x = dist(rng);
    for (auto &x : B) x = dist(rng);

    // ── 2. CPU secuencial ─────────────────────────────────────────────────
    std::cout << "Corriendo en CPU (secuencial)...\n";
    auto t0 = TClock::now();
    matmul_cpu(A, B, C_cpu);
    auto t1 = TClock::now();
    T5 ms_cpu = std::chrono::duration<T5, std::milli>(t1 - t0).count();
    std::cout << "  Tiempo CPU: " << ms_cpu << " ms\n\n";

    // ── 3. GPU con Metal ──────────────────────────────────────────────────
    std::cout << "Corriendo en GPU (Metal)...\n";

    // Obtener dispositivo GPU
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    std::cout << "  GPU: " << [device.name UTF8String] << "\n";

    // Cargar shader compilado
    NSError *err = nil;
    id<MTLLibrary> library =
        [device newLibraryWithURL:[NSURL fileURLWithPath:@"carlo_metal.metallib"]
                            error:&err];
    if (!library) {
        std::cerr << "Error cargando metallib: "
                  << [[err localizedDescription] UTF8String] << "\n";
        return 1;
    }

    // Obtener kernel y crear pipeline
    id<MTLFunction>             fn       = [library newFunctionWithName:@"matmul_kernel"];
    id<MTLComputePipelineState> pipeline =
        [device newComputePipelineStateWithFunction:fn error:&err];

    // Crear buffers GPU con los datos de entrada
    id<MTLBuffer> bufA = [device newBufferWithBytes:A.data()
                                             length:TOTAL * sizeof(TGpu)
                                            options:MTLStorageModeShared];
    id<MTLBuffer> bufB = [device newBufferWithBytes:B.data()
                                             length:TOTAL * sizeof(TGpu)
                                            options:MTLStorageModeShared];
    id<MTLBuffer> bufC = [device newBufferWithLength:TOTAL * sizeof(TGpu)
                                             options:MTLStorageModeShared];
    U1 dim = N;
    id<MTLBuffer> bufN = [device newBufferWithBytes:&dim
                                             length:sizeof(U1)
                                            options:MTLStorageModeShared];

    // Medir tiempo GPU incluyendo encode + dispatch + sync
    auto t2 = TClock::now();

    id<MTLCommandQueue>          queue   = [device newCommandQueue];
    id<MTLCommandBuffer>         cmdBuf  = [queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder = [cmdBuf computeCommandEncoder];

    [encoder setComputePipelineState:pipeline];
    [encoder setBuffer:bufA offset:0 atIndex:0];
    [encoder setBuffer:bufB offset:0 atIndex:1];
    [encoder setBuffer:bufC offset:0 atIndex:2];
    [encoder setBuffer:bufN offset:0 atIndex:3];

    // Grid 2D: un hilo por elemento de C
    // Cada threadgroup procesa un bloque de 16x16 elementos
    MTLSize threadsPerGroup = MTLSizeMake(16, 16, 1);
    MTLSize threadGroups    = MTLSizeMake((N + 15) / 16, (N + 15) / 16, 1);

    [encoder dispatchThreadgroups:threadGroups threadsPerThreadgroup:threadsPerGroup];
    [encoder endEncoding];
    [cmdBuf commit];
    [cmdBuf waitUntilCompleted];  // espera GPU — equivale a cudaDeviceSynchronize

    auto t3 = TClock::now();
    T5 ms_gpu = std::chrono::duration<T5, std::milli>(t3 - t2).count();
    std::cout << "  Tiempo GPU: " << ms_gpu << " ms\n\n";

    // ── 4. Verificar que ambos resultados coinciden ────────────────────────
    TGpu *gpuResult = static_cast<TGpu *>([bufC contents]);
    TGpu maxErr = 0;
    for (size_t i = 0; i < TOTAL; ++i)
        maxErr = std::max(maxErr, std::abs(C_cpu[i] - gpuResult[i]));

    // ── 5. Resumen ────────────────────────────────────────────────────────
    std::cout << "=== Resultados ===\n";
    std::cout << "  Tiempo CPU:  " << ms_cpu           << " ms\n";
    std::cout << "  Tiempo GPU:  " << ms_gpu           << " ms\n";
    std::cout << "  Speedup:     " << ms_cpu / ms_gpu  << "x\n";
    std::cout << "  Error maximo CPU vs GPU: " << maxErr << "\n";

    return 0;
}

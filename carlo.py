import re
import sys
from carlo_matrix import MatrixCarlo


# ── Funciones reutilizables ────────────────────────────────────────────────
def add_one(n):
    return n + 1

def square(n):
    return n * n

def add_x(n, val1, val2):
    return n + val1 + val2


def leer_matriz(nombre):
    """
    Pide al usuario una matriz como texto en formato 'NxM: n1 n2 ... / n3 n4 ...'
    y la parsea con regex via MatrixCarlo.from_string().
    """
    print(f"Ingrese {nombre} (formato: NxM: n1 n2 ... / fila2 ...):")
    texto = input()
    return MatrixCarlo.from_string(texto)


# ── Demo 1: leer de teclado con regex, aplicar funciones, guardar en archivo
def demo_matrix_carlo1():
    print("=== DemoMatrixCarlo1 ===")
    mat = leer_matriz("la matriz")

    print("Matriz original:")
    print(mat)

    print("Aplicando add_one (+1 a cada elemento):")
    mat.apply_function_to_all(add_one)
    print(mat)

    print("Aplicando square (cuadrado de cada elemento):")
    mat.apply_function_to_all(square)
    print(mat)

    with open("output_carlo1.txt", "w") as f:
        mat.print(f)
    print("Resultado guardado en output_carlo1.txt\n")


# ── Demo 2: leer desde archivo, aplicar funcion con args extra ────────────
def demo_matrix_carlo2():
    print("=== DemoMatrixCarlo2 ===")

    mat = MatrixCarlo()
    with open("output_carlo1.txt", "r") as f:
        mat.read(f)

    print("Matriz leida desde output_carlo1.txt:")
    print(mat)

    print("Aplicando add_x(5, 10) a todos los elementos:")
    mat.apply_function_to_all(add_x, 5, 10)
    print(mat)

    with open("output_carlo2.txt", "w") as f:
        mat.print(f)
    print("Resultado guardado en output_carlo2.txt")


# ── Demo 3: m1 = 5*m2 + m3*m4 ────────────────────────────────────────────
def demo_matrix_carlo3():
    print("=== DemoMatrixCarlo3: m1 = 5*m2 + m3*m4 ===")
    m2 = leer_matriz("m2")
    m3 = leer_matriz("m3")
    print("Ingrese m4 (filas debe coincidir con cols de m3):")
    m4 = leer_matriz("m4")

    m1 = 5 * m2 + m3 * m4
    print("m1 = 5*m2 + m3*m4:")
    print(m1)


# ── Demo 4: m1 = m2 * m3, indexado y print ───────────────────────────────
def demo_matrix_carlo4():
    print("=== DemoMatrixCarlo4: m1 = m2 * m3 ===")
    m2 = leer_matriz("m2")
    m3 = leer_matriz("m3 (filas debe coincidir con cols de m2)")

    m1 = m2 * m3
    print("m1 = m2 * m3:")
    print(m1)

    m1[0][0] = 8
    print("Despues de m1[0][0] = 8:")
    print(m1)


# ── Demo Threads: apply_function_to_all con hilos en matriz del usuario ───
def demo_threads():
    print("=== Demo Threads: apply_function_to_all(threaded=True) ===")
    mat = leer_matriz("la matriz")
    print("Matriz original:")
    print(mat)

    mat.apply_function_to_all(square, threaded=True)
    print("Despues de square con threaded=True (un hilo por fila):")
    print(mat)


# ── Main ──────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    demo_matrix_carlo1()
    demo_matrix_carlo2()
    demo_matrix_carlo3()
    demo_matrix_carlo4()
    demo_threads()

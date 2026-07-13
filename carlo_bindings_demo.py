"""
carlo_bindings_demo.py
Demo que usa MatrixCarlo<T> de C++ desde Python via pybind11.
Muestra que el template funciona para int, float y double.

Compilar primero:
    c++ -O3 -shared -fPIC -std=c++2b \\
        $(python3 -m pybind11 --includes) \\
        carlo_bindings.cpp \\
        -o carlo_module$(python3-config --extension-suffix)
"""

import re
import carlo_module


def leer_matriz(nombre, tipo="int"):
    """
    Lee una matriz del usuario en formato 'NxM: n1 n2 ... / fila2'
    y retorna el tipo C++ correspondiente (int, float o double).
    """
    print(f"Ingrese {nombre} (formato: NxM: n1 n2 ... / fila2 ...):")
    texto = input()

    header  = re.search(r'(\d+)\s*[xX]\s*(\d+)', texto)
    assert header, "Formato invalido: se espera NxM"
    rows, cols = header.group(1), header.group(2)

    numbers = re.findall(r'-?\d+\.?\d*', texto[header.end():])
    assert len(numbers) == int(rows) * int(cols)

    cpp_string = f"{rows} {cols} " + " ".join(numbers)

    if tipo == "T3F":
        return carlo_module.from_string_T3F(cpp_string)
    elif tipo == "T5":
        return carlo_module.from_string_T5(cpp_string)
    else:
        return carlo_module.from_string_TI(cpp_string)


# ── Demo int: m1 = 5*m2 + m3*m4 ─────────────────────────────────────────
def demo_int():
    print("=== MatrixCarlo<TI>: m1 = 5*m2 + m3*m4 ===")
    m2 = leer_matriz("m2", "TI")
    m3 = leer_matriz("m3", "TI")
    m4 = leer_matriz("m4 (filas = cols de m3)", "TI")

    m1 = 5 * m2 + m3 * m4
    print("m1 = 5*m2 + m3*m4:")
    print(m1)


# ── Demo float: apply y operadores ───────────────────────────────────────
def demo_float():
    print("=== MatrixCarlo<T3F>: apply_function_to_all ===")
    mat = leer_matriz("la matriz", "T3F")
    print("Original:")
    print(mat)

    mat.apply_function_to_all(lambda x: x * 1.5)
    print("Despues de *1.5:")
    print(mat)


# ── Demo double: indexado m[i][j] = valor ────────────────────────────────
def demo_double():
    print("=== MatrixCarlo<T5>: m1=m2*m3, m1[0][0]=8.0 ===")
    m2 = leer_matriz("m2", "T5")
    m3 = leer_matriz("m3 (filas = cols de m2)", "T5")

    m1 = m2 * m3
    print("m1 = m2 * m3:")
    print(m1)

    print(f"m1[0][0] antes: {m1[0][0]}")
    m1[0][0] = 8.0        # doble corchete directo gracias al RowProxy
    print(f"m1[0][0] despues: {m1[0][0]}")
    print(m1)


# ── Main ──────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    demo_int()
    demo_float()
    demo_double()

"""
carlo_bindings_demo.py
Demo que usa la clase MatrixCarlo de C++ directamente desde Python
via el modulo carlo_module compilado con pybind11.

Pasos para compilar y ejecutar:
    1. Instalar pybind11:
           pip install pybind11  (o con --break-system-packages si da error)

    2. Compilar el modulo C++:
           c++ -O3 -shared -fPIC -std=c++2b \\
               $(python3 -m pybind11 --includes) \\
               carlo_bindings.cpp \\
               -o carlo_module$(python3-config --extension-suffix)

    3. Ejecutar este script:
           python3 carlo_bindings_demo.py
"""

import carlo_module


def demo1():
    """Leer desde string, aplicar funciones — equivale a DemoMatrixCarlo1."""
    print("=== Demo 1: leer, apply_function_to_all ===")

    mat = carlo_module.from_string("2 3  1 2 3  4 5 6")
    print("Matriz original:")
    print(mat)

    mat.apply_function_to_all(lambda x: x + 1)
    print("Despues de +1 a cada elemento:")
    print(mat)

    mat.apply_function_to_all(lambda x: x * x)
    print("Despues de elevar al cuadrado:")
    print(mat)


def demo2():
    """apply_function_to_all con args extra — equivale a DemoMatrixCarlo2."""
    print("=== Demo 2: apply_function_to_all con args ===")

    mat = carlo_module.from_string("2 2  1 2  3 4")
    print("Matriz original:")
    print(mat)

    mat.apply_function_to_all_with_args(lambda x, v1, v2: x + v1 + v2, 5, 10)
    print("Despues de sumar 5+10 a cada elemento:")
    print(mat)


def demo3():
    """m1 = 5*m2 + m3*m4 — equivale a DemoMatrixCarlo3."""
    print("=== Demo 3: m1 = 5*m2 + m3*m4 ===")

    m2 = carlo_module.from_string("2 2  1 0  0 1")   # identidad 2x2
    m3 = carlo_module.from_string("2 3  1 2 3  4 5 6")
    m4 = carlo_module.from_string("3 2  1 0  0 1  1 0")

    m1 = 5 * m2 + m3 * m4
    print("m2 (identidad 2x2):")
    print(m2)
    print("m3 * m4:")
    print(m3 * m4)
    print("m1 = 5*m2 + m3*m4:")
    print(m1)


def demo4():
    """m1 = m2 * m3, luego m1[0][0] = 8 — equivale a DemoMatrixCarlo4."""
    print("=== Demo 4: m1 = m2*m3, indexado ===")

    m2 = carlo_module.from_string("2 3  1 2 3  4 5 6")
    m3 = carlo_module.from_string("3 2  7 8  9 10  11 12")

    m1 = m2 * m3
    print("m1 = m2 * m3:")
    print(m1)

    print(f"m1[0][0] = {m1.get(0, 0)}")
    m1.set(0, 0, 8)
    print("Despues de m1.set(0, 0, 8):")
    print(m1)


def demo_copy_constructor():
    """Copy constructor y move semantics."""
    print("=== Demo Copy Constructor ===")

    original = carlo_module.from_string("2 2  1 2  3 4")
    copia    = carlo_module.MatrixCarlo(original)   # copy constructor de C++

    copia.apply_function_to_all(lambda x: x * 10)

    print("Original (no debe cambiar):")
    print(original)
    print("Copia * 10:")
    print(copia)


if __name__ == "__main__":
    demo1()
    demo2()
    demo3()
    demo4()
    demo_copy_constructor()

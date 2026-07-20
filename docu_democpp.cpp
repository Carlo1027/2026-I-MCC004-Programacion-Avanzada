#include <iostream>
#include <type_traits>   // std::is_void, std::alignment_of
using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
// FUNCIÓN 1: DemoIsVoid
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Demuestra el uso del type trait std::is_void<T>.
 *
 * `std::is_void<T>::value` es una constante booleana en tiempo de compilación
 * que evalúa a `true` solo cuando `T` es exactamente el tipo `void`.
 * Para cualquier otro tipo (incluyendo `void*`) el valor es `false`.
 *
 * ### Ejemplos de evaluación
 * | Tipo       | `is_void<T>::value` |
 * |------------|---------------------|
 * | `int`      | `false`             |
 * | `void`     | `true`              |
 * | `void*`    | `false`             |
 *
 * @note Este trait es útil en programación genérica para distinguir si una
 *       función retorna `void`, por ejemplo al usar `std::invoke_result_t`.
 *
 * @see https://en.cppreference.com/w/cpp/types/is_void
 */
void DemoIsVoid()
{
    std::cout << std::boolalpha;            // imprime true/false en lugar de 1/0
    std::cout << "is_void:" << std::endl;
    std::cout << "int:  " << std::is_void<int >::value << std::endl;  // false
    std::cout << "void: " << std::is_void<void>::value << std::endl;  // true
}

// ─────────────────────────────────────────────────────────────────────────────
// FUNCIÓN 2: DemoAlignmentOf
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Demuestra el uso del type trait std::alignment_of<T>.
 *
 * `std::alignment_of<T>::value` retorna el **requisito de alineación en bytes**
 * del tipo `T`, es decir, la dirección de memoria debe ser múltiplo de ese valor.
 * Es equivalente a `alignof(T)` introducido en C++11.
 *
 * La alineación determina en qué dirección de memoria puede almacenarse una
 * variable: el procesador accede más eficientemente a datos alineados en
 * múltiplos del tamaño de su tipo.
 *
 * ### Valores típicos en arquitecturas de 64 bits
 * | Tipo          | Alineación (bytes) | Razón                            |
 * |---------------|--------------------|----------------------------------|
 * | `char`        | 1                  | 1 byte, puede ir en cualquier dir |
 * | `int`         | 4                  | 4 bytes, alineado a múltiplos de 4 |
 * | `int[20]`     | 4                  | El array hereda la alineación del elemento |
 * | `long long`   | 8                  | 8 bytes, alineado a múltiplos de 8 |
 *
 * @note El valor puede variar según la plataforma (32 vs 64 bits) y el compilador.
 *
 * @see https://en.cppreference.com/w/cpp/types/alignment_of
 * @see alignof (operador equivalente en C++11)
 */
void DemoAlignmentOf()
{
    std::cout << "alignment_of:" << std::endl;
    std::cout << "char         : " << std::alignment_of<char         >::value << std::endl;  // 1
    std::cout << "int          : " << std::alignment_of<int          >::value << std::endl;  // 4
    std::cout << "int[20]      : " << std::alignment_of<int[20]      >::value << std::endl;  // 4
    std::cout << "long long int: " << std::alignment_of<long long int>::value << std::endl;  // 8
}

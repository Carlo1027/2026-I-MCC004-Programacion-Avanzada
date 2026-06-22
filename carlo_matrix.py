import re
import sys
import threading


class MatrixCarlo:
    """
    Clase para matriz dinámica 2D, equivalente Python de MatrixCarlo<T> en C++.

    En C++ se usa un doble puntero T** con new/delete manual.
    En Python se usa una lista de listas y el GC maneja la memoria.

    Attributes:
        _rows (int): Número de filas.
        _cols (int): Número de columnas.
        _mat (list[list]): Datos de la matriz como lista de listas.

    Example:
        >>> m = MatrixCarlo.from_string("2x2: 1 2 / 3 4")
        >>> print(m)
        2 2
        1 2
        3 4
    """

    # ── Constructor / Copy constructor ────────────────────────────────────
    def __init__(self, other=None):
        """
        Constructor default o copy constructor.

        Args:
            other (MatrixCarlo, optional): Si se pasa, hace copia profunda.
                Equivale al copy constructor de C++.
        """
        if other is None:
            self._mat  = None
            self._rows = 0
            self._cols = 0
        elif isinstance(other, MatrixCarlo):
            # Copy constructor — copia profunda fila por fila
            self._rows = other._rows
            self._cols = other._cols
            self._mat  = [row[:] for row in other._mat]

    # ── Memoria ────────────────────────────────────────────────────────────
    def _create(self):
        """
        Reserva la matriz interna de ceros.
        Equivale a Create() en C++: new T*[rows] + new T[cols] por fila.

        Raises:
            AssertionError: Si rows o cols son 0.
        """
        assert self._rows > 0 and self._cols > 0
        self._mat = [[0] * self._cols for _ in range(self._rows)]

    def destroy(self):
        """
        Libera la memoria y resetea el estado.
        Equivale a Destroy() en C++: delete[] por fila + delete[] del arreglo.
        En Python solo se eliminan las referencias; el GC hace el resto.
        """
        self._mat  = None
        self._rows = 0
        self._cols = 0

    # ── I/O ────────────────────────────────────────────────────────────────
    def read(self, stream=None):
        """
        Lee filas, columnas y elementos desde un stream, igual que Read(istream&) en C++.

        El stream puede ser stdin o un archivo abierto. Lee tokens ignorando
        saltos de línea, igual que el operador >> de C++.

        Args:
            stream (IO, optional): Stream de entrada. Por defecto sys.stdin.

        Returns:
            MatrixCarlo: self, para encadenamiento.
        """
        if stream is None:
            stream = sys.stdin
        tokens = _tokenizer(stream)
        self._rows = int(next(tokens))
        self._cols = int(next(tokens))
        self._create()
        for i in range(self._rows):
            for j in range(self._cols):
                self._mat[i][j] = int(next(tokens))
        return self

    def print(self, stream=None):
        """
        Imprime la matriz en un stream, igual que Print(ostream&) en C++.

        Formato de salida: primera línea 'rows cols', luego una fila por línea.

        Args:
            stream (IO, optional): Stream de salida. Por defecto sys.stdout.

        Returns:
            MatrixCarlo: self, para encadenamiento.
        """
        if stream is None:
            stream = sys.stdout
        stream.write(f"{self._rows} {self._cols}\n")
        for i in range(self._rows):
            stream.write(" ".join(str(self._mat[i][j]) for j in range(self._cols)) + "\n")
        return self

    def __str__(self):
        """
        Representación en string, equivalente a operator<< en C++.

        Returns:
            str: Matriz formateada como 'rows cols\\nfila0\\nfila1\\n...'.
        """
        lines = [f"{self._rows} {self._cols}"]
        for row in self._mat:
            lines.append(" ".join(str(x) for x in row))
        return "\n".join(lines)

    # ── ApplyFunctionToAll ─────────────────────────────────────────────────
    def apply_function_to_all(self, func, *args, threaded=False):
        """
        Aplica func a cada elemento [i][j] de la matriz.

        Equivale a ApplyFunctionToAll(Func, Args...) en C++ con variadic templates.
        Con threaded=True, cada fila se procesa en un hilo separado (como en
        la implementación con std::thread del profesor).

        Args:
            func (callable): Función que recibe el elemento (y args extra) y
                retorna el nuevo valor. Ej: lambda x: x + 1.
            *args: Argumentos extra opcionales que se pasan a func.
            threaded (bool): Si True, usa un Thread por fila. Default False.

        Example:
            >>> mat.apply_function_to_all(lambda x: x * x)
            >>> mat.apply_function_to_all(lambda x, v: x + v, 5, threaded=True)
        """
        if not threaded:
            for i in range(self._rows):
                for j in range(self._cols):
                    self._mat[i][j] = func(self._mat[i][j], *args)
        else:
            def process_row(i):
                for j in range(self._cols):
                    self._mat[i][j] = func(self._mat[i][j], *args)

            threads = [threading.Thread(target=process_row, args=(i,))
                       for i in range(self._rows)]
            for t in threads: t.start()
            for t in threads: t.join()

    # ── Parsear desde string con regex ────────────────────────────────────
    @classmethod
    def from_string(cls, text):
        """
        Crea una MatrixCarlo desde un string con formato 'NxM: n1 n2 ... / n3 n4 ...'.

        Usa expresiones regulares para extraer las dimensiones y los elementos,
        sin depender del formato exacto de separadores.

        Args:
            text (str): String de entrada. Ej: '2x3: 1 2 3 / 4 5 6'.

        Returns:
            MatrixCarlo: Nueva instancia con los datos parseados.

        Raises:
            AssertionError: Si el formato no contiene 'NxM' o el número de
                elementos no coincide con rows * cols.

        Example:
            >>> m = MatrixCarlo.from_string("3x3: 1 2 3 / 4 5 6 / 7 8 9")
        """
        header = re.search(r'(\d+)\s*[xX]\s*(\d+)', text)
        assert header, "Formato invalido: se espera NxM al inicio"
        rows, cols = int(header.group(1)), int(header.group(2))

        numbers = re.findall(r'-?\d+', text[header.end():])
        assert len(numbers) == rows * cols, \
            f"Se esperaban {rows * cols} elementos, se encontraron {len(numbers)}"

        mat = cls()
        mat._rows = rows
        mat._cols = cols
        mat._create()
        idx = 0
        for i in range(rows):
            for j in range(cols):
                mat[i][j] = int(numbers[idx]); idx += 1
        return mat

    # ── Operadores aritmeticos ─────────────────────────────────────────────
    def __add__(self, other):
        """
        Suma elemento a elemento, equivale a operator+ en C++.

        Args:
            other (MatrixCarlo): Matriz con las mismas dimensiones.

        Returns:
            MatrixCarlo: Nueva matriz resultado.

        Raises:
            AssertionError: Si las dimensiones no coinciden.
        """
        assert self._rows == other._rows and self._cols == other._cols
        result = MatrixCarlo()
        result._rows = self._rows
        result._cols = self._cols
        result._create()
        for i in range(self._rows):
            for j in range(self._cols):
                result._mat[i][j] = self._mat[i][j] + other._mat[i][j]
        return result

    def __sub__(self, other):
        """
        Resta elemento a elemento, equivale a operator- en C++.

        Args:
            other (MatrixCarlo): Matriz con las mismas dimensiones.

        Returns:
            MatrixCarlo: Nueva matriz resultado.

        Raises:
            AssertionError: Si las dimensiones no coinciden.
        """
        assert self._rows == other._rows and self._cols == other._cols
        result = MatrixCarlo()
        result._rows = self._rows
        result._cols = self._cols
        result._create()
        for i in range(self._rows):
            for j in range(self._cols):
                result._mat[i][j] = self._mat[i][j] - other._mat[i][j]
        return result

    def __mul__(self, other):
        """
        Multiplicación matricial o por escalar, equivale a operator* en C++.

        Dos casos:
        - MatrixCarlo * MatrixCarlo: multiplicación matricial clásica (tres loops).
          Requiere self._cols == other._rows.
        - MatrixCarlo * escalar: multiplica cada elemento por el valor.

        Args:
            other (MatrixCarlo | int | float): Otra matriz o escalar.

        Returns:
            MatrixCarlo: Nueva matriz resultado.

        Raises:
            AssertionError: Si es multiplicación matricial y las dimensiones
                no son compatibles.
        """
        if isinstance(other, MatrixCarlo):
            assert self._cols == other._rows
            result = MatrixCarlo()
            result._rows = self._rows
            result._cols = other._cols
            result._create()
            for i in range(self._rows):
                for j in range(other._cols):
                    result._mat[i][j] = sum(
                        self._mat[i][k] * other._mat[k][j]
                        for k in range(self._cols)
                    )
            return result
        else:
            result = MatrixCarlo()
            result._rows = self._rows
            result._cols = self._cols
            result._create()
            for i in range(self._rows):
                for j in range(self._cols):
                    result._mat[i][j] = self._mat[i][j] * other
            return result

    def __rmul__(self, scalar):
        """
        Multiplicación con escalar a la izquierda: escalar * mat.

        Equivale a la función libre operator*(T value, const MatrixCarlo&) en C++.
        Permite escribir 5 * mat en vez de solo mat * 5.

        Args:
            scalar (int | float): Escalar multiplicador.

        Returns:
            MatrixCarlo: Nueva matriz resultado.
        """
        return self.__mul__(scalar)

    # ── Indexado: m[i][j] = valor ──────────────────────────────────────────
    def __getitem__(self, i):
        """
        Acceso por índice de fila: m[i] retorna la fila i como lista.

        Permite la sintaxis m[i][j] y m[i][j] = valor, igual que
        m_pMat[i][j] en C++.

        Args:
            i (int): Índice de fila.

        Returns:
            list: La fila i (referencia directa, no copia).
        """
        return self._mat[i]


# ── Funcion auxiliar: tokenizador de stream ────────────────────────────────
def _tokenizer(stream):
    """
    Genera tokens desde un stream ignorando saltos de línea.

    Equivale al comportamiento de operator>> en C++, que lee el siguiente
    token sin importar cuántos espacios o newlines haya entre ellos.

    Args:
        stream (IO): Stream de texto (stdin o archivo).

    Yields:
        str: Siguiente token del stream.
    """
    for line in stream:
        for token in line.split():
            yield token

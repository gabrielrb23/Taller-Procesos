# Suma de Arreglos con Procesos en C

Este programa utiliza múltiples procesos creados con `fork()` y comunicación mediante `pipe()` para calcular la suma de elementos almacenados en dos archivos de texto. Cada proceso tiene una función específica, y el resultado final es mostrado por el proceso padre.

---

## 🧩 Estructura de procesos

- **Nieto**: Calcula la suma del arreglo A desde el primer archivo.
- **Segundo hijo**: Calcula la suma del arreglo B desde el segundo archivo.
- **Primer hijo**: Recibe ambas sumas y calcula la suma total.
- **Padre**: Recibe las tres sumas y las imprime.

---

## ⚙️ Compilación

make

./taller_procesos N1 archivo00 N2 archivo01

# TareaIA

Es una aplicación que le escribes reglas en español y arma el árbol semántico para ver si todas se pueden cumplir al mismo tiempo.

## Funciones

- Encuentra las proposiciones simples y les pone una letra.
- Si la misma frase sale en dos reglas, usa la misma letra.
- Entiende negación, "y", "o", "si… entonces" y "si y solo si".
- Muestra los átomos, las fórmulas y el árbol.
- Marca las fórmulas que ya usó y dice qué ramas quedaron abiertas o cerradas.
- Da los valores de las ramas abiertas, o avisa que la base es insatisfacible.

## Uso

1. Escribir una regla por línea.
2. analizar.
3. Revisar: átomos, fórmulas, árbol gráfico, detalle de ramas y resultados.

### Ejemplo
Si es de día, entonces voy a trabajar
Si llueve, entonces no voy a trabajar
Es de día


p = es de día 
q = voy a trabajar
r = llueve

El resultado es satisfacible: dos ramas cerradas y una abierta con p = V, q = V, r = F.

## Formato de las reglas

| Conectivo | Ejemplo |
| --- | --- |
| Negación | `no estudio` |
| Conjunción | `estudio y trabajo` |
| Disyunción inclusiva | `estudio o trabajo` |
| Implicación | `si estudio, entonces apruebo` |
| Bicondicional | `estudio si y solo si trabajo` |

Puedo usar paréntesis, por ejemplo `no (estudio o trabajo)`. La "y" pesa más que la "o", y si anido implicaciones o quiero aclarar, uso paréntesis.

Si un átomo sale como libre (V o F) en una rama abierta, con cualquiera de los dos valores esa rama se cumple. Varias ramas abiertas pueden repetir los mismos valores.

## Estructura

- `assets/`: imágenes u otros recursos, si hacen falta.
- `bin/`: el ejecutable y lo que se genera al compilar.
- `include/`: los archivos de cabecera (`.hpp`).
- `src/`: el código del parser, la lógica del árbol y la interfaz.
- `TareaIA.pro`: la configuración del proyecto Qt, que genera `Makefile.qt`.
- `makefile`: los atajos (`build`, `run`, `start`, `clean`, `rebuild`).
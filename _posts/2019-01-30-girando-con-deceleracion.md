---
layout: post
title:  "Girando con deceleracion"
commit: 0bcc0009b5fadbd287b0561b3e05b3e34820bfb9
---

Haremos lo mismo que en el caso anterior, pero esta vez teniendo encuenta
que la velocidad es en radianes/s y la aceleración en radianes/ss

# Bugs por todos los lados

Nos ha llevado más de lo previsto porque teníamos varios bugs ocultos por
ahí. Vamos a tener que dedicar un día a revisar, limpiar y ordenar el 
código porque el ansia viva nos hace avanzar alocadamente. Entre las cosas
que hemos echo están:

- Igualamos el código de los encoders derecho e izquierdo

- Controlamos la dirección de los motores con una variable, en vez de con
la lectura del segundo pin del encoder (unos días atrás detectamos que a veces
el encoder contaba en sentido contrario: creemos que era
debido a que se estaba ejecutando el timer, se activaba la interrupción de
encoder activando el flag correspondiente del procesador, pero cuando acababa
el timer y se ejecutaba finalmente la ISR (el arduino sólo puede hacer una)
la lectura del segundo pin ya había cambiado.

- Hemos puesto constantes de procesador para varias variables (máximas velocidades
lineales, angulares, aceleraciones...) aunque las funciones correspondiente utilizan
una variable en memoria, que se puede setear con la línea de comandos

- La distancia entre las ruedas, que ya nos había dado algún quebradero de cabeza,
ha vuelto a dar problemas. Finalmente encontramos buenos resultados con 0.085... pero
el giro no es exacto cada vez.

- Hemos calibrado la máxima velocidad angular independientemente del pwm máximo que
podemos obtener. Si nos acercabamos (como en el movimiento rectilíneo) el robot
deceleraba muy mal. Quizá tengamos mucho momento inercial, al estar las ruedas bastante
atrás. Tampoco nos importa la velocidad para este movimiento, que sólo se realiza en
la fase de exploración. Nos preocupa más que sea lo más preciso posible.

En el commit de hoy hay varias trazas en el `main()`, sólo para comprobar que los cálculos
eran correctos. Se puede comprobar que los encoders terminan con distintas distancias, y
tenemos que introducir unas constantes de corrección para el movimiento angular (que 
también se utilizaran en el movimiento rectilíneo).

Para mañana, a ver si somos capaces de hacer el movimiento más difícil: un giro suave de
90 grados.

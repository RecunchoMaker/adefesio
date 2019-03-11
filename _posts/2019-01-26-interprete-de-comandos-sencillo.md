---
layout: post
title:  "Interprete de comandos sencillo"
commit: 54d4e70df3bce10ebc46c71175f4262771ca66bc
---

Queremos hacer un intérprete de comandos sencillo y así poder cambiar parámetros
(como los valores del pid) de forma rápida y ver rápidamente el resultado, sin
tener que recompilar y subir el sketch. Tambien lo usaremos en el futuro para
lanzar rutinas concretas o pruebas determinadas.

# Lectura en bucle del puerto serie

Creamos una función `comando_lee_serial` que acumula los caracteres introducidos
por el puerto serie en un pequeño buffer (inicialmente de 16 caracteres, 
creemos que será suficiente). Esta función se puede llamar en el bucle de `main()`
y no es bloqueante, aunque en las pruebas que estamos haciendo actualmente nos
conviente esperar la introdución de comandos hasta que lancemos la orden especial
'_go_'

```cpp
void comando_lee_serial() {
    while (Serial.available()) {
        caracter = Serial.read();

        buffer[buffer_idx] = caracter;
        if (caracter == COMANDO_FIN) {
            if (buffer_idx > 0) {
                buffer[buffer_idx] = 0;
                _procesa_buffer();
                comando_init();
            }
        } else {
            buffer_idx += (buffer_idx > COMANDO_MAX_TAMANO-1?0:1);
        }
    }
}
```

# Comandos con parámetro

Hemos decidido que los comandos tendrán la forma `\<comando\> \<parametro\>`, 
separados por un espacio. Para simplificar, sólo permitiremos 0 o 1 parámetro.
El comando especial `_go_` simplemente pone una variable a _true_ que se puede consultar en
el bucle principal (por ejemplo, para lanzar una prueba).

```cpp
comando_prompt();
while (!comando_go()) {
    comando_lee_serial();
}
Serial.println("GO!");
```

# Definición de instrucciones

No nos complicamos la vida y lo que tenemos es una lista de `strcmp` con las instrucciones
que vayamos definiendo.

```cpp
void _procesa_comando(char *token, char *parametro) {
    if (!strcmp(token, "kp")) {
        motores_set_kp_lineal(atof(parametro));
        Serial.print("kp=");
        Serial.println(motores_get_kp_lineal());
    } else if (!strcmp(token, "kd")) {
        motores_set_kd_lineal(atof(parametro));
        Serial.print("kd=");
        Serial.println(motores_get_kd_lineal());
    }
    else if (!strcmp(token, "ki")) {
        motores_set_ki_lineal(atof(parametro));
        Serial.print("ki=");
        Serial.println(motores_get_ki_lineal());
    }
}
```

Todo es muy sencillo y rápido. Como todo lo que estamos haciendo últimamente, al no tener
exactamente claras las necesidades, lo programamos en _prototype mode_ y veremos cómo resulta.
No hay (ni creemos que habrá) control de errores, eco local, ni nada.  Si queremos ver lo
que tecleamos, usaremos el modo estándar de las comunicaciones serie y lanzaremos el monitor
de esta forma:

```
pio device monitor --echo
```

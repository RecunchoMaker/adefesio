---
layout: post
title:  "Resoldando"
commit: 
---

Hemos desmontado Adefesio y comprobado un montón de conexiones. Aún no tenemos claro
cual es la conexión que fallaba (ni siquiera si es realmente eso), pero hemos comprobado
que la alimentación hacia la placa de sensores no estaba muy segura: por una parte,
el conector Dupont parecía algo flojo (como con holgura), y por otra, la soldadura
que unía esa alimentación con el cable general de la placa de sensores (que va hacia
las resistencias) no parecía muy buena.

Hemos estañado un poco el macho del Dupont, y hemos soldado bien la alimentación.

# Aún no

Seguimos teniendo el mismo problema. Intentamos diagnosticar de otras maneras y vemos
algo realmente curioso: cuando el robot está en este "estado rompehuevos", acercarse
a cualquier sensor hace subir todos los demás, en menos proporción, pero apreciable. Parece
como que la corriente se está repartiendo entre todos los sensores. Pensamos en un problema
con el ULN2003 (que haya que poner a tierra las patillas que no se usan), pero no vemos en
el datasheet nada que haga referencia a eso.

Dejamos el código al mínimo, quitando la interrupción temporal y haciendo el bucle
de leds en el main. Sigue el mismo problema. Descartamos casi al 100% un fallo en el
software.

Logueando a través del USB ocurre lo mismo que con la batería. Algo es algo, y nos permite
al menos depurar a través del puerto serie normal.

"Apretando" el chasis, el robot pasa aleatoriamente del estado normal al estado rompehuevos.

Desconectamos todos los dupont y los volvemos a conectar. No habíamos notado anteriormente
la holgura en el pin de tierra. Estañamos un poquito el pin macho y... voilá! Parece que ahora
funciona. Apretando el chasis al tiempo que logueamos no notamos esos saltos de lectura que
notábamos antes. Confiamos en que por fin esté solucionado esto.



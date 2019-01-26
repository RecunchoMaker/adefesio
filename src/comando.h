#ifndef __COMANDO_H
#define __COMANDO_H

void comando_init();
void comando_prompt();
void comando_lee_serial();
void _procesa_buffer();
void _procesa_comando(char *token, char *parametro);
void _procesa_comando(char *token);
bool comando_go();

#endif /* ifndef _COMANDO_H

 */

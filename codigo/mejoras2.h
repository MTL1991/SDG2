//------------------------------------------------------------------------------
// mejoras2.h
//
//   Definición de tipos y declaración de funciones para el archivo
//   mejoras2.h
//
//   Creado por : Manuel Toro Legaz y Jorge Ulloa Núñez
//------------------------------------------------------------------------------

#ifndef __MEJORAS2_H__
#define __MEJORAS2_H__

//------------------------------------------------------------------
//       DECLARACIÓN DE FUNCIONES IMPLEMENTADAS EN mejoras2.c
//------------------------------------------------------------------
void dia_o_noche(void); //NUEVO
int suma_binario(int dato1, int dato2); //NUEVO
int paridad_incorrecta(int dato1); //NUEVO
void pintar_LCD(void);
void configurar_alarmas(void);
char teclado(void);
void pinta_tecla(void);
void comprueba_paridad (void);
void alarmas_acusticas (void);
int logica(BOOL agua_activa,BOOL humo1_activa,BOOL humo2_activa,BOOL gas_activa);

#endif
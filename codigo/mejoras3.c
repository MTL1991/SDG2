//------------------------------------------------------------------------------
// mejoras3.c
//
//   Funciones realizadas para el HITO2 de SDG2
//
//   Creado por: Manuel Toro Legaz y Jorge Ulloa Núñez
//------------------------------------------------------------------------------

#ifndef __MEJORAS3_C__
#define __MEJORAS3_C__

#include "mejoras3.h"
#define NUM_FILAS 4
#define NUM_COLS 4
#define EXCIT 1




//------------------------------------------------------
// void alarmas_acusticas(void)
//
// Descripción:
//   
//   NUEVO
//------------------------------------------------------

void alarmas_acusticas(void)
{
  WORD valor, incremento = 0;

  // Imprime mensaje de generando señal
  output("...GENERANDO SEÑAL...\r\n");

  // Bucle de generación de señal
  while(agua_activa || humo1_activa || humo2_activa || gas_activo)
  {
	int alarmas=logica(agua_activa, humo1_activa, humo2_activa, gas_activo);
	if(agua_activa){ incremento = 100; }
	if(humo1_activa){ incremento = 150; }
	if(humo2_activa){ incremento = 200; }
	if(gas_activo){ incremento = 250; }
	if(alarmas==2){ incremento = 350; }
	if(alarmas==3){ incremento = 450; }
    // Rampa de subida
    for(valor = 0; valor <= 0xFFF; valor += incremento)
      DAC_dato(valor);

    // Rampa de bajada
    for(valor = 0xFFF; valor >= 0; valor -= incremento)
      DAC_dato(valor);

	
  

}
}

//------------------------------------------------------
// int logica(BOOL agua_activa,BOOL humo1_activa,BOOL humo2_activa,BOOL gas_activo)
//
// Descripción:
//   
//   NUEVO
//------------------------------------------------------
int logica(BOOL agua_activa,BOOL humo1_activa,BOOL humo2_activa,BOOL gas_activa){
	int temp = 0;
	if(agua_activa){
		temp++;
	}
	if(humo1_activa){
		temp++;
	}
	if(humo2_activa){
		temp++;
	}
	if(gas_activo){
		temp++;
	}
	return temp;
}








		

#endif
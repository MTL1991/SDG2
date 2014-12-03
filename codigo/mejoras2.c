//------------------------------------------------------------------------------
// mejoras2.c
//
//   Funciones realizadas para el HITO2 de SDG2
//
//   Creado por: Manuel Toro Legaz y Jorge Ulloa Núñez
//------------------------------------------------------------------------------

#ifndef __MEJORAS2_C__
#define __MEJORAS2_C__

#include "mejoras2.h"
#define NUM_FILAS 4
#define NUM_COLS 4
#define EXCIT 1


//--------------------------------------------------------
// void dia_o_noche(void)
//
// Descripción:
//   Función que se encarga de determinar si es de dia o
//   de noche  mediante el sensor lumínico. Una vez 
//   determinado se aplicaran los valores correspondientes
//   en funcion de esto.
//--------------------------------------------------------

void dia_o_noche(void){
	
	if (valorbit[5] == 0)
	{
	
		ausencia_de_luz++;
		
	}else{
		if(index_desechar>=LIMITE_AJUSTE){
		
			output(" DIA ");
		
		}            
		
		limite_agua = LIMITE_AGUA_INICIAL + (pulsaciones_agua*10);
		limite_gas = LIMITE_GAS_INICIAL + (pulsaciones_gas*10);
		limite_humo1= LIMITE_HUMO1_INICIAL + (pulsaciones_humo1*10);
		limite_humo2= LIMITE_HUMO2_INICIAL+ (pulsaciones_humo2*10);
		ausencia_de_luz = 0;

	
	}
	
	if(ausencia_de_luz>limite_ausencia_de_luz)
	{
	
		output(" NOCHE ");
		limite_agua = 50;
		limite_gas = 10;
		limite_humo1 = 20;
		limite_humo2 =  50;
		
		
	}

}

//------------------------------------------------------
// void comprueba_paridad (void)
//
// Descripción:
//   Función que se encarga de comprobar si el bit de 
//   paridad recibido es correcto o incorrecto.
//   
//------------------------------------------------------

void comprueba_paridad (void){

	comprobacion = suma_binario (valorbit[1], valorbit[2]);		
	comprobacion = suma_binario (comprobacion, valorbit[3]);
	comprobacion = suma_binario (comprobacion, valorbit[4]);
	comprobacion = suma_binario (comprobacion, valorbit[5]);
		
	if(index_desechar>LIMITE_AJUSTE){
		outNum(10,comprobacion,0);
		output(" <- ");
		
	}

		

		
	if (comprobacion != valorbit [6]){
			
		output(" Paridad Incorrecta ");
		valor_agua = paridad_incorrecta(valor_agua);
		valor_gas = paridad_incorrecta(valor_gas);
		valor_humo1 = paridad_incorrecta(valor_humo1);
		valor_humo2 = paridad_incorrecta(valor_humo2);
	}
		
	comprobacion = 0;
}

//------------------------------------------------------
// int suma_binario(int dato1, int dato2)
//
// Descripción:
//   Función auxiliar para realizar sumas binarias
//   
//------------------------------------------------------

int suma_binario(int dato1, int dato2){

	if (dato1 == 1 && dato2 == 1){
	
		return 0;
	
	}
	
	return dato1+dato2;

}

//------------------------------------------------------
// void paridad_incorrecta(int dato1)
//
// Descripción:
//   Función que se encarga de tratar los tiempos de la 
//   alarma de agua
//   NUEVO
//------------------------------------------------------

int paridad_incorrecta(int dato1)
{

	if (dato1 > AJUSTE_PARIDAD){
		return dato1-AJUSTE_PARIDAD;
	}
	
 	
	return 0;
}

//------------------------------------------------------
// void pintar_LCD(void)
//
// Descripción:
//   Función que se encarga de utilizar la pantalla LCD
//   NUEVO
//------------------------------------------------------

void pintar_LCD(void){
  
  char* mensaje = "EMERGENCIA";

  if(agua_activa || humo1_activa || humo2_activa || gas_activo){
	
  	while(*mensaje){		// Imprime el mensaje en el display
    		LCD_dato(*mensaje++); 	// carácter a carácter
  	}
	alarmas_acusticas();
  			// Movemos el cursor a la 1ª línea
  	retardo(RET_1S);		// Esperamos otro segundo
  }
  LCD_inst(CLR_DISP);		// Limpiamos display
  LCD_inst(LIN_1LCD);

}

//------------------------------------------------------
// char teclado(void)
//
// Descripción:
//   Explora el teclado matricial y devuelve la tecla 
//   pulsada
//------------------------------------------------------
char teclado(void){

  //char tecla;
  BYTE fila, columna, fila_mask;
  static char teclas[4][4] = {{"123C"},
                              {"456D"},
                              {"789E"},
                              {"A0BF"}};
  // Bucle de exploración del teclado
  while(TRUE){

	if(agua_activa || humo1_activa || humo2_activa || gas_activo){
		pintar_LCD();
		
	}


    // Excitamos una columna
    for(columna = NUM_COLS - 1; columna >= 0; columna--){
      set16_puertoS(EXCIT << columna);		// Se envía la excitación de columna
      retardo(1150);				// Esperamos respuesta de optoacopladores

      // Exploramos las filas en busca de respuesta
      for(fila = NUM_FILAS - 1; fila >= 0; fila--){
        fila_mask = EXCIT << fila;		// Máscara para leer el bit de la fila actual
        if(lee16_puertoE() & fila_mask){		// Si encuentra tecla pulsada,
          while(lee16_puertoE() & fila_mask);	//   Esperamos a que se suelte
          retardo(1150);			//   Retardo antirrebotes
          return teclas[fila][columna];		//   Devolvemos la tecla pulsada
        }
      }
      // Siguiente columna
    }
    // Exploración finalizada sin encontrar una tecla pulsada
  }
  // Reiniciamos exploración
}


//------------------------------------------------------
// void configurar_alarmas(void)
//
// Descripción:
//   Función que se encarga de aumentar o disminuir
//   los limites de las alarmas mediante la pulsación
//   del teclado.
//   NUEVO
//------------------------------------------------------

void configurar_alarmas(void){

	char tecla1;
  	tecla1 = teclado();
	LCD_dato(tecla1);
	if (tecla1 == 'C'){
		output(" INICIAR CONFIGURACION ");
	}
 	while(tecla1=='C'){
		
		char tecla2;
		tecla2=teclado();
		LCD_dato(tecla2);
		
		if(tecla2 == '1'){
			
			pulsaciones_agua++;
			output("\r\nAUMENTADO LIMITE AGUA 1s ");
		}
		if(tecla2 == '2'){
			if(LIMITE_AGUA_INICIAL + (pulsaciones_agua*10)>0){
				pulsaciones_agua--;
				output("\r\nDISMINUIDO LIMITE AGUA 1s ");
			}
		}
		if(tecla2 == '3'){
			pulsaciones_humo1++;
			output("\r\nAUMENTADO LIMITE HUMO ALTO 1s ");
		}
		if(tecla2 == '4'){
			if(LIMITE_HUMO1_INICIAL + (pulsaciones_humo1*10)>0){

			pulsaciones_humo1--;
			output("\r\nDISMINUIDO LIMITE HUMO ALTO 1s ");
			}
		}
		if(tecla2 == '5'){
			pulsaciones_humo2++;
			output("\r\nAUMENTADO LIMITE HUMO MEDIO 1s ");
		}
		if(tecla2 == '6'){
			if(LIMITE_HUMO2_INICIAL + (pulsaciones_humo2*10)>0){

			pulsaciones_humo2--;
			output("\r\nDISMINUIDO LIMITE HUMO MEDIO 1s ");
			}
		}
		if(tecla2 == '7'){
			pulsaciones_gas++;
			output("\r\nAUMENTADO LIMITE GAS 1s ");
		}
		if(tecla2 == '8'){
			if(LIMITE_GAS_INICIAL + (pulsaciones_gas*10)>0){

			pulsaciones_gas--;
			output("\r\nDISMINUIDO LIMITE GAS 1s ");
			}
		}
		if(tecla2 == '0'){
			pulsaciones_agua=0;
			pulsaciones_gas=0;
			pulsaciones_humo1=0;
			pulsaciones_humo2=0;
			output("\r\nTIEMPOS LÍMITE AJUSTADOS POR DEFECTO ");

		}

		if(tecla2 == 'F'){
			tecla1='F';
			output("\r\n FINALIZAR CONFIGURACION ");
		}
		
		
	}
	
}

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
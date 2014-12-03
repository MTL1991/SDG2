
//------------------------------------------------------------------------------
// MT-7.c
//
//   El programa se encarga de detectar el bit de arranque y realizar
//   10 muestreos de cada bit para determinar su valor por mayoria
// Autores: Manuel Toro Legaz y Jorge Ulloa Nuñez
//------------------------------------------------------------------------------
#include "m5272.h"
#include "m5272lib.c"
#include "m5272gpio.c"
#include "m5272lcd.c"
#include "m5272adc_dac.c" //NUEVO

#define V_BASE 0x40				  // Dirección de inicio de los vectores de interrupción
#define DIR_VTMR0 4*(V_BASE+5)			  // Dirección del vector de TMR0

#define FREC_INT 100				  // Frec. de interr. TMR0 = 100 Hz (cada 10ms)
#define PRESCALADO 2
#define DIR_VINT1 4 * (V_BASE + 1)
#define CNT_INT1 MCF_CLK/(FREC_INT*PRESCALADO*16)	  // Valor de precarga del temporizador de interrupciones TRR0
#if CNT_INT1>0xFFFF
#error PRESCALADO demasiado pequeño para esa frecuencia (CNT_INT1>0xFFFF)
#endif
#define BORRA_REF 0x0002			  // Valor de borrado de interr. pendientes de tout1 para TER0

volatile ULONG cont_retardo;
volatile UWORD agua;

#define LIMITE_AJUSTE 11 
#define AJUSTE_PARIDAD 10
#define LIMITE_AGUA_INICIAL 200
#define LIMITE_GAS_INICIAL 100
#define LIMITE_HUMO1_INICIAL 100
#define LIMITE_HUMO2_INICIAL 300

int index = 0;
int index_general = 0;
int index_desechar = 0;

int periodica [10];
int valorbit [10];

int valor_agua = 0;
int limite_agua = LIMITE_AGUA_INICIAL;
BOOL agua_activa = FALSE; //NUEVO
int pulsaciones_agua=0;

int valor_gas = 0;
int limite_gas = LIMITE_GAS_INICIAL;
BOOL gas_activo = FALSE; //NUEVO
int pulsaciones_gas=0;

int valor_humo1 = 0;
int limite_humo1 = LIMITE_HUMO1_INICIAL;
BOOL humo1_activa = FALSE; //NUEVO
int pulsaciones_humo1=0;

int valor_humo2 = 0;
int limite_humo2 = LIMITE_HUMO2_INICIAL;
BOOL humo2_activa = FALSE; //NUEVO
int pulsaciones_humo2=0;

int transicion_humo = 0;
int inicio = 0;
int ausencia_de_luz=0;			//NUEVO
int limite_ausencia_de_luz=200; // NUEVO
int comprobacion = 0; //NUEVO

void trata_alarma_agua(void);
void trata_alarma_humo(void);
void trata_alarma_gas(void);
void print (void);

#include "mejoras2.c"

	
	

//------------------------------------------------------
// void rutina_tout0(void)
//
// Descripción:
//   Función de atención a la interrupción para TIMER0
//------------------------------------------------------
void rutina_tout0(void)
{

  periodica [index++] = (lee16_puertoE() & 0x0010)/16; //obtenemos el bit de entrada de datos

  mbar_writeShort(MCFSIM_TER0,BORRA_REF); 	// Reset del bit de fin de cuenta
		  
     

}


//------------------------------------------------------
// void rutina_int1(void)
//
// Descripción:
//   Función de atención a la interrupción para la
//   interrupción externa.
//------------------------------------------------------
void rutina_int1(void)
{
	int numeroUnos = 0;
	int i;
	for(i = 0; i <index; i++)
	{
		if(index_desechar>10)
		{
		
			outNum(10, periodica[i],0);
		
		}            	   
            	
		numeroUnos += periodica[i];
         
	}
        
//Se procede a determinar el número por mayoría    
 
	if (2*numeroUnos > index)
	{
		valorbit[index_general++]=1;
		
		if (index_desechar <= LIMITE_AJUSTE)
		{
			
			index_desechar++;
		
		}
		if(index_desechar>LIMITE_AJUSTE){
			
			output(" --> UNO\r\n");
		
		}
	}
	else
	{
		valorbit[index_general++]=0;
		if (index_desechar <= LIMITE_AJUSTE)
		{
			
			index_desechar++;
		
		} 
		
		if(index_desechar>LIMITE_AJUSTE){
		
			output(" -->CERO\r\n");
		
		}
	}
	
	if (index_general == 9){
		
		comprueba_paridad();
	}
		
	
//Ajusta el índice del array tras el primer ciclo, lo limpia y empieza a mostrar por pantalla	
		
	if (index_general > 9)
	{
		index_general = 0;
	}
	
	if (index_desechar == LIMITE_AJUSTE)
	{
		index_general = 0;
		
		for(i = 0; i <10; i++)
		{
           	   
            		valorbit[i]=0;
         	}
		
	}
	
		
	//llama al metodo que utiliza las alarmas de agua
		
	trata_alarma_agua();
	
	//llama al metodo que utiliza las alarmas de gas
	
	trata_alarma_gas();
	
	//llama al metodo que utiliza las alarmas de humo
	
	trata_alarma_humo();
	
	//lama al metodo que determina si es de dia o de noche   NUEVO
	
	dia_o_noche();
	
	// inicializa el indice del array de interrupciones periodicas
	
	index = 0;
	
	// Imprime por pantalla los tiempos acumulados de las alarmas
	
	print();
	
	
  	// Al ser interrupción externa debemos volverla a activar

  	mbar_writeLong(MCFSIM_ICR1, 0xA888F888);
}
//------------------------------------------------------
// void trata_alarma_agua(void)
//
// Descripción:
//   Función que se encarga de tratar los tiempos de la 
//   alarma de agua
//------------------------------------------------------

void trata_alarma_agua(void){
	
	if (valorbit[1] == 1)
	{
	
		valor_agua++;
		
	}else
	{
		agua_activa = FALSE;
		valor_agua=0;
		
	}
	
	if(valor_agua >= limite_agua)
	{
		agua_activa=TRUE;
		output(" AGUA ");
		
	}

}
//------------------------------------------------------
// void trata_alarma_gas(void)
//
// Descripción:
//   Función que se encarga de tratar los tiempos de la 
//   alarma de gas
//------------------------------------------------------
void trata_alarma_gas(void){
	
	if (valorbit[4] == 1)
	{
		
		valor_gas++;
			
	}else
	{
		gas_activo=FALSE;
		valor_gas=0;
			
	}
		
	if(valor_gas >= limite_gas)
	{
		gas_activo=TRUE;
		output(" GAS ");
			
	}
}
//------------------------------------------------------
// void trata_alarma_humo(void)
//
// Descripción:
//   Función que se encarga de tratar los tiempos de la 
//   alarma de humo
//------------------------------------------------------

void trata_alarma_humo(void){

	if (valorbit[2] == 1)
	{
	
		if(valorbit[3]==1)
		{
		
			valor_humo1++;
			
			valor_humo2=0;
			
		}else
		{
		
			if (valor_humo1>= limite_humo1)
			{
			
				valor_humo2 = limite_humo2;
				
			}
			
			valor_humo2++;	
					
			valor_humo1=0;
			
		}
	}else
	{
		humo1_activa=FALSE;
		valor_humo1=0;
		humo2_activa=FALSE;
		valor_humo2=0;

	}
	if(valor_humo1 >= limite_humo1)
	{
		humo1_activa=TRUE;
		humo2_activa=FALSE;
		output(" HUMO ELEVADO ");
		
	}
	if(valor_humo2 >= limite_humo2)
	{
		humo2_activa=TRUE;
		humo1_activa=FALSE;
		output("HUMO MEDIO");
		
	}
}

//------------------------------------------------------
// void print(void)
//
// Descripción:
//   Función que se encarga de imprimir por pantalla los
//   tiempos acumulados de las alarmas y el índice del
//   array de datos.
//------------------------------------------------------

void print (void)
{

	if(index_desechar>=LIMITE_AJUSTE)
	{
		
		output(" ");
		
		outNum(10,index_general,0);
	
		output(" ");
	
		outNum(10,valor_agua,0);
	
		output(" ");
	
		outNum(10,valor_gas,0);
	
		output(" ");
		
		outNum(10,valor_humo1,0);
		
		output(" ");
	
		outNum(10,valor_humo2,0);
	
		output(" ");
		
		outNum(10,ausencia_de_luz,0);
	
		output(" ");
	
	}
}


//------------------------------------------------------
// void __init(void)
//
// Descripción:
//   Función por defecto de inicialización del sistema
//------------------------------------------------------
void __init(void)
{

	DAC_ADC_init();  //Inicializamos el DAC
	LCD_reset();	// Reseteamos el LCD
  	LCD_init();	// e inicializamos el display	

	// Fijamos el comienzo de vectores de interrupción en V_BASE
	mbar_writeByte(MCFSIM_PIVR,V_BASE);
	
	// Escribimos la dirección de la función para TMR0
 	ACCESO_A_MEMORIA_LONG(DIR_VTMR0)= (ULONG)_prep_TOUT0;

 	// Escribimos la dirección de la función de atención a la interrupción	
  	ACCESO_A_MEMORIA_LONG(DIR_VINT1) = (ULONG) _prep_INT1; 	
 	
	// Imprimimos el mensaje de comienzo
  	output("COMIENZA EL PROGRAMA\r\n");

	// Configuramos INT1 para que se active con flanco de subida
  	mbar_writeLong(MCFSIM_PITR, mbar_readLong(MCFSIM_PITR) | 0x80000000);

  	// TMR0: PS=1-1=0 CE=00 OM=1 ORI=1 FRR=1 CLK=10 RST=1
  	mbar_writeShort(MCFSIM_TMR0, (PRESCALADO-1)<<8|0x3D);

	// Ponemos a 0 el contador del TIMER0
  	mbar_writeShort(MCFSIM_TCN0, 0x0000);

	// Fijamos la cuenta final del contador
  	mbar_writeShort(MCFSIM_TRR0, CNT_INT1);

	// definimos la interrupcion del TIMER_0 y de la INT_1 como no pendiente y de nivel
  	mbar_writeLong(MCFSIM_ICR1, 0xA888F888);

  	cont_retardo = 0;

	// Habilitamos interrupciones
  	sti();

			
}

//------------------------------------------------------
// void bucleMain(void)
//
// Descripción:
//   Función del programa principal
//------------------------------------------------------
void bucleMain(void)
{
	configurar_alarmas();
	pintar_LCD();
	
  
}



//---------------------------------------------------------
// Definición de rutinas de atención a la interrupción
// Es necesario definirlas aunque estén vacías
//---------------------------------------------------------


void rutina_int2(void){}
void rutina_int3(void){}
void rutina_int4(void){}
void rutina_tout1(void){}
void rutina_tout2(void){}
void rutina_tout3(void){}
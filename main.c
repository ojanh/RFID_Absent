#include "DrvGPIO.h"
#include "DrvSYS.h"
#include "LCD_Driver.h"
#include "DrvSPI.h"
#include <DrvUART.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//inisialisasi fungsi
void Init();
void delayCheck();
void connectWifu();
void sendDatas();
void bacarfid();

//inisialisasi data
char* dataRead;
int get;


int main(void){

	int get;
	char inSensor[40];

	Init();
    Initial_pannel();
    DrvGPIO_ClrBit(E_GPD,14);
    uint8_t buff[7];
    bacarfid();


    while(1){
    	int i;
    	char printdata[3];
    	char TextData[8];
    	clr_all_pannal();
    	buff[0]='0';

    	//tulis ke lcs baris 0
    	print_lcd(0,"Kelompok IOTA");

    	//baca input dari nuvoton
    	DrvUART_Read(UART_PORT0,buff,3);

    	//cek input awal dari buffer (array 0) adalah char y
    	//untuk memastikan kiriman data dari arduino
    	if (buff[0]=='y') {
    		print_lcd(1,"Detect RFID");
			DrvSYS_Delay(900000);
			delayCheck();
    	}



    }

}

//konfigurasi UART0 untuk komunikasi dengan Arduino
void bacarfid(){
	 STR_UART_T UartParam;
	    E_DRVGPIO_FUNC FuncNum = E_FUNC_UART0;
	    E_UART_PORT UartNum = UART_PORT0;

	    /* Uart Pin Config */
	    DrvGPIO_InitFunction(E_FUNC_UART0);

	    /* Uart Config */
	    UartParam.u32BaudRate = 9600;
	    UartParam.u8cDataBits = DRVUART_DATABITS_8;
	    UartParam.u8cStopBits = DRVUART_STOPBITS_2;
	    UartParam.u8cParity = DRVUART_PARITY_NONE;
	    UartParam.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;
	    UartParam.u8TimeOut = 0;

	    /* Uart Open */
	    DrvUART_Open(UartNum, &UartParam);
}


//delay +-4 detik sambil menunggu ada orang yang masuk melalui sensor
void delayCheck(){
	char inSensor[16];
	long var;
	clr_all_pannal();
	print_lcd(0,"Kelompok IOTA");


	get =  DrvGPIO_GetBit(E_GPB,13); //mendapatkan input awal dari sensor
	sprintf(inSensor, "senInfra: %d", get); //string builder
	print_lcd(1,inSensor);

	//looping sambil mendapatkan input dari sensor
	for (var = 0; var < 4000000; var++) {

		//jika mendapat input snesor maka nuvoton langsung ngirim sinyal ke arduino untuk kirim data ke server lewat ESP
		if (!DrvGPIO_GetBit(E_GPB,13)){
			sprintf(inSensor, "senInfra: %d", DrvGPIO_GetBit(E_GPB,13));
			print_lcd(1,inSensor);
			DrvUART_Write(UART_PORT0,"send$",5);
			break;
		}
	}
}


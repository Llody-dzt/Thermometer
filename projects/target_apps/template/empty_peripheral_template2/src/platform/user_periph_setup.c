#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "syscntl.h"
#include "arch_console.h"
#include "app_nst1001.h"


#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{

#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif
	
	//RESERVE_GPIO(PWM1, PWM1_PORT, PWM1_PIN, PID_PWM1);
		RESERVE_GPIO(GPIO,NST1001_PORT, NST1001_PIN, PID_GPIO);
}

#endif

void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif
		//GPIO_ConfigurePin(PWM1_PORT, PWM1_PIN, INPUT_PULLUP, PID_PWM1, true);
		//GPIO_ConfigurePin(NST1001_PORT, NST1001_PIN, INPUT, PID_GPIO, false);
	
	
}

#if defined (CFG_PRINTF_UART2)
// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
    .baud_rate = UART2_BAUDRATE,
    .data_bits = UART2_DATABITS,
    .parity = UART2_PARITY,
    .stop_bits = UART2_STOPBITS,
    .auto_flow_control = UART2_AFCE,
    .use_fifo = UART2_FIFO,
    .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
    .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
    .intr_priority = 2,
};
#endif

#if 1
uint16_t cnt=0;
int16_t TempInteger=0;
uint16_t TempDecimal=0;
unsigned char tempNST1001[6];

void GetNST1001Temp(uint8_t *Temp)
{
	unsigned int num,temp;	  //num脉冲计数值，tmp 整数温度值转换
	float tmp = 0;			  //浮点数温度值
	unsigned char Number[10] = "0123456789"; //0-9用于将温度值转字符串
	num = 0;		//脉冲计数清零	
	
	{
	 GPIO_SetActive(PWM1_PORT, PWM1_PIN);
	
		while(!GPIO_GetPinStatus(PWM1_PORT, PWM1_PIN))
		{
			num++;
		}				
	}	
	tmp =  num*0.0625 - 50.0625;
	
	if(num<1281)  //小于30℃
	{
	   tmp = tmp + (tmp - 30)*0.005;
	}
	if((num>1281) && (num<2401))  //30℃<温度<100℃
	{
	   tmp = tmp;
	}
	if(num > 2401)//大于100℃
	{
	   tmp = tmp + (100 - tmp)*0.012;
	}
		if(num < 2401)//  <100℃
	{
	   temp = tmp*100;	//乘以100方便转换
	   tempNST1001[0] = Number[temp/1000%10];
	   tempNST1001[1] = Number[temp/100%10]; 
	   tempNST1001[2] = '.';				
	   tempNST1001[3] = Number[temp/10%10];  
     tempNST1001[4] = Number[temp%10]; 
	   tempNST1001[5] = '\0';
	}
	if(num >= 2401)//  >=100℃
	{
	   temp = tmp*100;	//乘以100方便转换
	   tempNST1001[0] = Number[temp/1000%10];
	   tempNST1001[1] = Number[temp/100%10]; 				
	   tempNST1001[2] = Number[temp/10%10];
	   tempNST1001[3] = '.';  
     tempNST1001[4] = Number[temp%10]; 
	   tempNST1001[5] = '\0';
	}
//	TempInteger=tmp*1000/1000;
//	TempInteger=(uint16_t)tmp*1000%1000;
	
	memcpy(tempNST1001, Temp, 6);
	arch_printf("%s\r\n",Temp);
}

#endif




void periph_init(void)
{
#if defined (__DA14531__)
    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

    // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

    // Set pad functionality
    set_pad_functions();

	
    // Enable the pads
    GPIO_set_pad_latch_en(true);
}



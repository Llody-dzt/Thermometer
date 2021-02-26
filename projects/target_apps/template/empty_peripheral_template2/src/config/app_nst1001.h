
#include "gpio.h"


#define APP_NST1001

#ifdef APP_NST1001
		#define NST1001_PORT	GPIO_PORT_0
		#define NST1001_PIN		GPIO_PIN_8
#endif

#define APP_COLLECT_TIMER			5	
#define APP_TEM_CONST					500625
#define APP_TEM_SCALE					625


typedef enum app_nst_stat{
	APP_NST_STAT_IDLE,
	APP_NST_STAT_READY,
	APP_NST_STAT_COLLECT,
	APP_NST_STAT_RELEASE,
} app_nst_stat_t;

//the end

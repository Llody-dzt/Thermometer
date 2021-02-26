
#include "rwip_config.h"             // SW configuration

#include "app_api.h"
#include "user_empty_peripheral_template.h"

#if 1
#include "app_easy_timer.h"
#include "gap.h"
#include "co_bt.h"
#include "gpio.h"
#include "app.h"
#include "prf_utils.h"
#include "custs1.h"
#include "user_custs1_def.h"
#include "user_periph_setup.h"
#include "custs1_task.h"
#include <math.h>
#include "arch_console.h"
#include "app_nst1001.h"
#include "adc.h"
#include "adc_531.h"
#include "wkupct_quadec.h"
#ifdef APP_NST1001
uint32_t tp_cunt=0;
uint32_t tem_vlaue;
app_nst_stat_t nst_stat = APP_NST_STAT_IDLE;
timer_hnd nst_timer;
#endif

#define NOTIFICATION_DELAY 50 

struct mnf_specific_data_ad_structure
{
    uint8_t ad_structure_size;
    uint8_t ad_structure_type;
    uint8_t company_id[APP_AD_MSD_COMPANY_ID_LEN];
    uint8_t proprietary_data[APP_AD_MSD_DATA_LEN];
};

uint8_t app_connection_idx                      __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_adv_data_update_timer_used        __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_param_update_request_timer_used   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
int16_t X_data									__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
int16_t Y_data									__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
int16_t Z_data									__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t X_string[8] 							__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Y_string[8] 							__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Z_string[8]								__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t X_timer     							__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Y_timer     							__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Z_timer    								__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t g_timer    								__attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
struct mnf_specific_data_ad_structure  mnf_data  __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data_len                     __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

static void mnf_data_init()
{
    mnf_data.ad_structure_size = sizeof(struct mnf_specific_data_ad_structure ) - sizeof(uint8_t); // minus the size of the ad_structure_size field
    mnf_data.ad_structure_type = GAP_AD_TYPE_MANU_SPECIFIC_DATA;
    mnf_data.company_id[0] = APP_AD_MSD_COMPANY_ID & 0xFF; // LSB
    mnf_data.company_id[1] = (APP_AD_MSD_COMPANY_ID >> 8 )& 0xFF; // MSB
    mnf_data.proprietary_data[0] = 0;
    mnf_data.proprietary_data[1] = 0;
}


static void mnf_data_update()
{    
	  uint16_t data;

    data = mnf_data.proprietary_data[0] | (mnf_data.proprietary_data[1] << 8);
    data += 1;
    mnf_data.proprietary_data[0] = data & 0xFF;
    mnf_data.proprietary_data[1] = (data >> 8) & 0xFF;

    if (data == 0xFFFF) {
         mnf_data.proprietary_data[0] = 0;
         mnf_data.proprietary_data[1] = 0;
    }
}
static void app_add_ad_struct(struct gapm_start_advertise_cmd *cmd, void *ad_struct_data, uint8_t ad_struct_len, uint8_t adv_connectable)
{
    uint8_t adv_data_max_size = (adv_connectable) ? (ADV_DATA_LEN - 3) : (ADV_DATA_LEN);
    
    if ((adv_data_max_size - cmd->info.host.adv_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to advertising data
        memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len], ad_struct_data, ad_struct_len);

        // Update Advertising Data Length
        cmd->info.host.adv_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the advertising data
        mnf_data_index = cmd->info.host.adv_data_len - sizeof(struct mnf_specific_data_ad_structure);
    }
    else if ((SCAN_RSP_DATA_LEN - cmd->info.host.scan_rsp_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to scan response data
        memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len], ad_struct_data, ad_struct_len);

        // Update Scan Response Data Length
        cmd->info.host.scan_rsp_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the scan response data
        mnf_data_index = cmd->info.host.scan_rsp_data_len - sizeof(struct mnf_specific_data_ad_structure);
        // Mark that manufacturer data is in scan response and not advertising data
        mnf_data_index |= 0x80;
    }
    else
    {
        // Manufacturer Specific Data do not fit in either Advertising Data or Scan Response Data
        ASSERT_WARNING(0);
    }
    // Store advertising data length
    stored_adv_data_len = cmd->info.host.adv_data_len;
    // Store advertising data
    memcpy(stored_adv_data, cmd->info.host.adv_data, stored_adv_data_len);
    // Store scan response data length
    stored_scan_rsp_data_len = cmd->info.host.scan_rsp_data_len;
    // Store scan_response data
    memcpy(stored_scan_rsp_data, cmd->info.host.scan_rsp_data, stored_scan_rsp_data_len);
}
static void adv_data_update_timer_cb()
{
    // If mnd_data_index has MSB set, manufacturer data is stored in scan response
    uint8_t *mnf_data_storage = (mnf_data_index & 0x80) ? stored_scan_rsp_data : stored_adv_data;

    // Update manufacturer data
    mnf_data_update();

    // Update the selected fields of the advertising data (manufacturer data)
    memcpy(mnf_data_storage + (mnf_data_index & 0x7F), &mnf_data, sizeof(struct mnf_specific_data_ad_structure));

    // Update advertising data on the fly
    app_easy_gap_update_adv_data(stored_adv_data, stored_adv_data_len, stored_scan_rsp_data, stored_scan_rsp_data_len);
    
    // Restart timer for the next advertising update
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);
}
static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}
void user_app_init(void)
{
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    
    // Initialize Manufacturer Specific Data
    mnf_data_init();
    
    // Initialize Advertising and Scan Response Data
    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
    
		#if 1
    //Initialize timer handlers
    X_timer = EASY_TIMER_INVALID_TIMER;
    Y_timer = EASY_TIMER_INVALID_TIMER;
    Z_timer = EASY_TIMER_INVALID_TIMER;
    g_timer = EASY_TIMER_INVALID_TIMER;
    #endif
	
    default_app_on_init();
}

#ifdef APP_NST1001
uint16_t cal_vall;
extern uint16_t otp_cs_get_adc_25_cal(void);

int32_t temp_sensor_read2(void)
{
    int32_t T;
		
		adc_config_t adc_cfg =
    {
        .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .input = ADC_INPUT_SE_TEMP_SENS,
    };

    adc_init(&adc_cfg);
		
    uint16_t temp = adc_get_temp2();
		
		cal_vall = otp_cs_get_adc_25_cal();
		
		if (cal_vall == 0)
			cal_vall = 30272;
		
		T = temp;
    T -= cal_vall;
		T *=1000000;
		T /= 145*64;
		T += 250000;
		
		//T = 25 + (temp - cal_vall )*100000/( 145 * 64);
		
    adc_disable();	

		
    return T;
}

int8_t temp_sensor_read(void)
{
    int8_t temp;

    /* Most of the ADC configuration is performed by the driver when the temperature
       sensor input is selected so we can just leave it blank. */
    adc_config_t adc_cfg =
    {
        .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .input = ADC_INPUT_SE_TEMP_SENS,
    };

    adc_init(&adc_cfg);
		
    temp = adc_get_temp();		
		
		if (cal_vall == 0)
		{
			cal_vall = 30272;
		}
		
    adc_disable();

		
    return temp;
}

void app_collect_callback(void){
		
//	if((tp_cunt > 1) && (tp_cunt <=3201)){
//		tem_vlaue = tp_cunt * APP_TEM_SCALE;
//		tem_vlaue -= APP_TEM_CONST;
//	}	
//	
//	  int8_t temp;
//    adc_config_t adc_cfg =
//    {
//        .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
//        .input = ADC_INPUT_SE_TEMP_SENS,
//    };

//    adc_init(&adc_cfg);
//    temp = adc_get_temp();
//    adc_disable();
		
		
}
void user_nst1001_release_cb(void)
{
	switch(nst_stat){
		case APP_NST_STAT_IDLE:
				
				if(GPIO_GetIRQInputLevel(GPIO0_IRQn) == GPIO_IRQ_INPUT_LEVEL_HIGH){
						tp_cunt = 0;
						nst_stat = APP_NST_STAT_COLLECT;
						nst_timer = app_easy_timer(APP_COLLECT_TIMER,app_collect_callback); //app_collect_callback
						//Falling edge trigger
						GPIO_EnableIRQ(NST1001_PORT, 
															NST1001_PIN, 
															GPIO0_IRQn, 
															true, 
															true, 
															0);
				}
			break;
		case APP_NST_STAT_READY:
			break;
		case APP_NST_STAT_COLLECT:
			tp_cunt++;
			break;
		case APP_NST_STAT_RELEASE:
			break;
	}
	
}
#endif

void user_app_adv_start(void)
{
//	#ifdef APP_NST1001
//	GPIO_EnableIRQ(NST1001_PORT, NST1001_PIN, GPIO0_IRQn, false, true, 0);									
//	// Register the proper GPIO callback function					
//	GPIO_RegisterCallback(GPIO0_IRQn, user_nst1001_release_cb);
//	#endif
																			
	// Schedule the next advertising data update
	app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);
	
	struct gapm_start_advertise_cmd* cmd;
	cmd = app_easy_gap_undirected_advertise_get_active();
	
	// Add manufacturer data to initial advertising or scan response data, if there is enough space
	app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

	app_easy_gap_undirected_advertise_start();
}

uint32_t tic=0;
uint32_t toc=0;
uint32_t count=0;
uint32_t Temp=0;

void my_timer_cb()
{
   			uint16_t cnt;
				
//				cnt=quad_decoder_get_x_counter()/2;
	
				cnt=quad_decoder_get_x_counter()/4;
	
				if(tic == cnt)
				{
					if(toc != cnt)
					{
						count=cnt-toc;
						Temp=count*625-500625;
					}
					toc = cnt;
				}
					
				tic=cnt;
		app_easy_timer(1, my_timer_cb);
}



void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        // Stop the advertising data update timer
        app_easy_timer_cancel(app_adv_data_update_timer_used);

        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
        user_app_adv_start();
    }

		{
			QUAD_DEC_INIT_PARAMS_t dd = {0};
			dd.chx_event_mode=QUAD_DEC_CHX_EDGE_COUNTING;
			dd.chx_port_sel = QUAD_DEC_CHXA_P08_AND_CHXB_P09;
			quad_decoder_init(&dd);
			app_easy_timer(1, my_timer_cb);
		}		
		
    default_app_on_connection(connection_idx, param);
}
void user_app_adv_undirect_complete(uint8_t status)
{
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
        user_app_adv_start();
    }
}
void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }
		
		#if 1
    // Stop the notification timers
    if(X_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(X_timer);
        X_timer = EASY_TIMER_INVALID_TIMER;
    }
    if(Y_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(Y_timer);
        Y_timer = EASY_TIMER_INVALID_TIMER;
    }
    if(Z_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(Z_timer);
        Z_timer = EASY_TIMER_INVALID_TIMER;
    }
    if(g_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(g_timer);
        g_timer = EASY_TIMER_INVALID_TIMER;
    }
		#endif
		
    // Update manufacturer data for the next advertsing event
    mnf_data_update();
    // Restart Advertising
    user_app_adv_start();
}


uint8_t user_int_to_string(int16_t input, uint8_t *s){
	uint8_t length = 1;
	if(input < 0){
		s[0] = '-';
	} else {
		s[0] = ' ';
	}
	input = abs(input);
	if(input  >= 10000){
		s[length++] = '0' + ((input / 10000) % 10);
	}
	if(input  >= 1000){
		s[length++] = '0' + ((input / 1000) % 10);
	}
	if(input  >= 100){
		s[length++] = '0' + ((input / 100) % 10);
	}
	if(input  >= 10){
		s[length++] = '0' + ((input / 10) % 10);
	}
	
	s[length++] = '0' + (input% 10);
	return length;
}


void user_svc2_g_timer_cb_handler(void)
{
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                        DEF_SVC1_ADC_VAL_1_CHAR_LEN);
		

		uint32_t temp = Temp; //temp_sensor_read2();
		
    //Initialize message fields
    req->handle = SVC1_IDX_ADC_VAL_1_VAL;
    req->length = 4;
    req->notification = true;
    memcpy(req->value, &temp, 4);
    
    //Send the message to the task
    ke_msg_send(req);
    
    //Set a timer for 100 ms (10*10)
    g_timer = app_easy_timer( NOTIFICATION_DELAY / 10, user_svc2_g_timer_cb_handler);  //
}


void user_svc2_g_wr_ntf_handler(struct custs1_val_write_ind const *param)
{
    //Check if the client has subscribed to notifications
	if(param->value[0])
    {
        //Start the timer if it's not running
        if(g_timer == EASY_TIMER_INVALID_TIMER)
        {
					g_timer = app_easy_timer(10, user_svc2_g_timer_cb_handler);
		}
	}
	else
    {
        //If the client has unsubscribed, invalidate the timer
        if(g_timer != EASY_TIMER_INVALID_TIMER)
        {
					app_easy_timer_cancel(g_timer);
					g_timer = EASY_TIMER_INVALID_TIMER;
		}
	}
}



void user_catch_rest_hndl(ke_msg_id_t const msgid, void const *param,ke_task_id_t const dest_id,ke_task_id_t const src_id)
{
    switch(msgid)
    {		
		case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_ADC_VAL_1_NTF_CFG:
                    user_svc2_g_wr_ntf_handler(msg_param);
                    break;
								
                default:
                    break;
            }
        } break;
				
		case CUSTS1_VAL_NTF_CFM:
        {
            struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);

            switch (msg_param->handle)
            {
							case SVC1_IDX_ADC_VAL_1_NTF_CFG:
                    //user_svc2_g_wr_ntf_handler(msg_param);
                    break;
							
                default:
                    break;
            }
        } break;
        
        default:
            break;
    }
}
#endif

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}


/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.h
 *
 * @brief Empty peripheral template project header file.
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _USER_EMPTY_PERIPHERAL_TEMPLATE_H_
#define _USER_EMPTY_PERIPHERAL_TEMPLATE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"
#include "app_task.h"                  // application task
#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "app.h"                       // application definitions
#include "co_error.h"                  // error code definitions
 

#if 1

#define APP_PARAM_UPDATE_REQUEST_TO         (1000)  
#define APP_ADV_DATA_UPDATE_TO              (10)   // 10*10ms = 100msec, The maximum allowed value is 41943sec (4194300 * 10ms)

#define APP_AD_MSD_COMPANY_ID       (0xABCD)
#define APP_AD_MSD_COMPANY_ID_LEN   (2)
#define APP_AD_MSD_DATA_LEN          6

int16_t extern X_data;
int16_t extern Y_data;
int16_t extern Z_data;

void user_app_init(void);
void user_app_adv_start(void);
void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);
void user_app_adv_undirect_complete(uint8_t status);
void user_app_disconnect(struct gapc_disconnect_ind const *param);
void user_catch_rest_hndl(ke_msg_id_t const msgid,void const *param,ke_task_id_t const dest_id,ke_task_id_t const src_id);

#endif

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);
void user_on_disconnect( struct gapc_disconnect_ind const *param );

/// @} APP

#endif // _USER_EMPTY_PERIPHERAL_TEMPLATE_H_

#ifndef _USER_CUSTS1_DEF_H_
#define _USER_CUSTS1_DEF_H_

#include "attm_db_128.h"

#define DEF_SVC1_UUID_128                {0x59, 0x5a, 0x08, 0xe4, 0x86, 0x2a, 0x9e, 0x8f, 0xe9, 0x11, 0xbc, 0x7c, 0x98, 0x43, 0x42, 0x18}
#define DEF_SVC1_ADC_VAL_1_UUID_128      {0x17, 0xB9, 0x67, 0x98, 0x4C, 0x66, 0x4C, 0x01, 0x96, 0x33, 0x31, 0xB1, 0x91, 0x59, 0x00, 0x15}
#define DEF_SVC1_ADC_VAL_1_CHAR_LEN      4
#define DEF_SVC1_ADC_VAL_1_USER_DESC     "TEMP Value"


#if 1
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,
    SVC1_IDX_ADC_VAL_1_CHAR,
    SVC1_IDX_ADC_VAL_1_VAL,
    SVC1_IDX_ADC_VAL_1_NTF_CFG,
    SVC1_IDX_ADC_VAL_1_USER_DESC,
    CUSTS1_IDX_NB
};
#else
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    SVC1_IDX_CONTROL_POINT_CHAR,
    SVC1_IDX_CONTROL_POINT_VAL,
    SVC1_IDX_CONTROL_POINT_USER_DESC,

    SVC1_IDX_LED_STATE_CHAR,
    SVC1_IDX_LED_STATE_VAL,
    SVC1_IDX_LED_STATE_USER_DESC,

    SVC1_IDX_ADC_VAL_1_CHAR,
    SVC1_IDX_ADC_VAL_1_VAL,
    SVC1_IDX_ADC_VAL_1_NTF_CFG,
    SVC1_IDX_ADC_VAL_1_USER_DESC,

    SVC1_IDX_ADC_VAL_2_CHAR,
    SVC1_IDX_ADC_VAL_2_VAL,
    SVC1_IDX_ADC_VAL_2_USER_DESC,

    SVC1_IDX_BUTTON_STATE_CHAR,
    SVC1_IDX_BUTTON_STATE_VAL,
    SVC1_IDX_BUTTON_STATE_NTF_CFG,
    SVC1_IDX_BUTTON_STATE_USER_DESC,

    SVC1_IDX_INDICATEABLE_CHAR,
    SVC1_IDX_INDICATEABLE_VAL,
    SVC1_IDX_INDICATEABLE_IND_CFG,
    SVC1_IDX_INDICATEABLE_USER_DESC,

    SVC1_IDX_LONG_VALUE_CHAR,
    SVC1_IDX_LONG_VALUE_VAL,
    SVC1_IDX_LONG_VALUE_NTF_CFG,
    SVC1_IDX_LONG_VALUE_USER_DESC,
    
    // Custom Service 2
    SVC2_IDX_SVC,
    
    SVC2_WRITE_1_CHAR,
    SVC2_WRITE_1_VAL,
    SVC2_WRITE_1_USER_DESC,

    SVC2_WRITE_2_CHAR,
    SVC2_WRITE_2_VAL,
    SVC2_WRITE_2_USER_DESC,
    
    // Custom Service 3
    SVC3_IDX_SVC,
    
    SVC3_IDX_READ_1_CHAR,
    SVC3_IDX_READ_1_VAL,
    SVC3_IDX_READ_1_NTF_CFG,
    SVC3_IDX_READ_1_USER_DESC,

    SVC3_IDX_READ_2_CHAR,
    SVC3_IDX_READ_2_VAL,
    SVC3_IDX_READ_2_USER_DESC,
    
    SVC3_IDX_READ_3_CHAR,
    SVC3_IDX_READ_3_VAL,
    SVC3_IDX_READ_3_IND_CFG,
    SVC3_IDX_READ_3_USER_DESC,

    CUSTS1_IDX_NB
};
#endif



#endif // _USER_CUSTS1_DEF_H_

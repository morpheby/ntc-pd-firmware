/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "oscillators.h"
#include "uart_base.h"
#include "flash_store.h"
#include "wdt.h"
#include "cn_inputs.h"
#include "mem_pool.h"
#include "timing.h"
#include "app_connector.h"

#include "modbus.h"
#include "ADC.h"
#include "math.h"
#include "D_I_O.h"


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

#define RAM_START_ADDRESS               0x900

// Modbus Registers
int BRG_VAL __attribute__ ((address(RAM_START_ADDRESS+0)));//REG 0

float ADC0 __attribute__  ((address(RAM_START_ADDRESS+2)));//REG 1, REG 2
float ADC1 __attribute__  ((address(RAM_START_ADDRESS+6)));//REG 3, REG 4
float ADC2 __attribute__  ((address(RAM_START_ADDRESS+10)));//REG 5, REG 6
float ADC3 __attribute__  ((address(RAM_START_ADDRESS+14)));//REG 7, REG 8
float ADC4 __attribute__  ((address(RAM_START_ADDRESS+18)));//REG 9, REG 10
float ADC5 __attribute__  ((address(RAM_START_ADDRESS+22)));//REG 11, REG 12
float ADC6 __attribute__  ((address(RAM_START_ADDRESS+26)));//REG 13, REG 14

float K0 __attribute__ ((address(RAM_START_ADDRESS+30)));//REG 15, REG 16
float K1 __attribute__ ((address(RAM_START_ADDRESS+34)));//REG 17, REG 18
float K2 __attribute__ ((address(RAM_START_ADDRESS+38)));//REG 19, REG 20
float K3 __attribute__ ((address(RAM_START_ADDRESS+42)));//REG 21, REG 22
float K4 __attribute__ ((address(RAM_START_ADDRESS+46)));//REG 23, REG 24
float K5 __attribute__ ((address(RAM_START_ADDRESS+50)));//REG 25, REG 26
float K6 __attribute__ ((address(RAM_START_ADDRESS+54)));//REG 27, REG 28

int OFS_ADC0 __attribute__        ((address(RAM_START_ADDRESS+58)));//REG 29
int OFS_ADC1 __attribute__        ((address(RAM_START_ADDRESS+60)));//REG 30
int OFS_ADC2 __attribute__        ((address(RAM_START_ADDRESS+62)));//REG 31
int OFS_ADC3 __attribute__        ((address(RAM_START_ADDRESS+64)));//REG 32
int OFS_ADC4 __attribute__        ((address(RAM_START_ADDRESS+66)));//REG 33
int OFS_ADC5 __attribute__        ((address(RAM_START_ADDRESS+68)));//REG 34
int OFS_ADC6 __attribute__        ((address(RAM_START_ADDRESS+70)));//REG 35

int n_a  __attribute__ ((address(RAM_START_ADDRESS+72)));           //REG 36
int n_b  __attribute__ ((address(RAM_START_ADDRESS+74)));           //REG 37
int n_c  __attribute__ ((address(RAM_START_ADDRESS+76)));           //REG 38
int ind_off __attribute__ ((address(RAM_START_ADDRESS+78)));        //REG 39

int N __attribute__ ((address(RAM_START_ADDRESS+80)));              //REG 40

int D_In  __attribute__ ((address(RAM_START_ADDRESS+82)));          //REG 41
int D_Out __attribute__ ((address(RAM_START_ADDRESS+84)));          //REG 42
int Ind_Delay __attribute__ ((address(RAM_START_ADDRESS+86)));      //REG 43

int profile __attribute__ ((address(RAM_START_ADDRESS+88)));        //REG 44
int ind_0_1 __attribute__ ((address(RAM_START_ADDRESS+90)));        //REG 45
int ind_1_1 __attribute__ ((address(RAM_START_ADDRESS+92)));        //REG 46
int ind_2_1 __attribute__ ((address(RAM_START_ADDRESS+94)));        //REG 47
int ind_0_2 __attribute__ ((address(RAM_START_ADDRESS+96)));        //REG 48
int ind_1_2 __attribute__ ((address(RAM_START_ADDRESS+98)));        //REG 49
int ind_2_2 __attribute__ ((address(RAM_START_ADDRESS+100)));       //REG 50
int ind_0_3 __attribute__ ((address(RAM_START_ADDRESS+102)));       //REG 51
int ind_1_3 __attribute__ ((address(RAM_START_ADDRESS+104)));       //REG 52
int ind_2_3 __attribute__ ((address(RAM_START_ADDRESS+106)));       //REG 53
int ind_0_4 __attribute__ ((address(RAM_START_ADDRESS+108)));       //REG 54
int ind_1_4 __attribute__ ((address(RAM_START_ADDRESS+110)));       //REG 55
int ind_2_4 __attribute__ ((address(RAM_START_ADDRESS+112)));       //REG 56

float P1   __attribute__  ((address(RAM_START_ADDRESS+114)));   //REG 57
float P2   __attribute__  ((address(RAM_START_ADDRESS+118)));   //REG 59
float P3   __attribute__  ((address(RAM_START_ADDRESS+122)));   //REG 61

float Q1   __attribute__ ((address(RAM_START_ADDRESS+126)));    //REG 63
float Q2   __attribute__ ((address(RAM_START_ADDRESS+130)));    //REG 65
float Q3   __attribute__ ((address(RAM_START_ADDRESS+134)));    //REG 67

float S1   __attribute__ ((address(RAM_START_ADDRESS+138)));    //REG 69
float S2   __attribute__ ((address(RAM_START_ADDRESS+142)));    //REG 71
float S3   __attribute__ ((address(RAM_START_ADDRESS+146)));    //REG 73

float cos_f1 __attribute__ ((address(RAM_START_ADDRESS+150)));    //REG 75
float cos_f2 __attribute__ ((address(RAM_START_ADDRESS+154)));    //REG 77
float cos_f3 __attribute__ ((address(RAM_START_ADDRESS+158)));    //REG 79

float AD4_AVG  __attribute__ ((address(RAM_START_ADDRESS+162)));       //REG 81
float AD5_AVG  __attribute__ ((address(RAM_START_ADDRESS+166)));       //REG 83
float AD6_AVG  __attribute__ ((address(RAM_START_ADDRESS+170)));       //REG 85

float AD4_RMS __attribute__ ((address(RAM_START_ADDRESS+174)));      //REG 87
float AD5_RMS __attribute__ ((address(RAM_START_ADDRESS+178)));      //REG 89
float AD6_RMS __attribute__ ((address(RAM_START_ADDRESS+182)));      //REG 91

int FLASH_WR __attribute__ ((address(RAM_START_ADDRESS+186)));      //REG 93
int FLASH_RD __attribute__ ((address(RAM_START_ADDRESS+188)));      //REG 94
int PROF_CHANGE_SOURCE __attribute__ ((address(RAM_START_ADDRESS+190)));//REG 95

unsigned int A0  __attribute__ ((address(RAM_START_ADDRESS+192)));      //REG 96
unsigned int A1  __attribute__ ((address(RAM_START_ADDRESS+194)));      //REG 97
unsigned int A2  __attribute__ ((address(RAM_START_ADDRESS+196)));      //REG 98
unsigned int A3  __attribute__ ((address(RAM_START_ADDRESS+198)));      //REG 99
unsigned int A4  __attribute__ ((address(RAM_START_ADDRESS+200)));      //REG 100
unsigned int A5  __attribute__ ((address(RAM_START_ADDRESS+202)));      //REG 101
unsigned int A6  __attribute__ ((address(RAM_START_ADDRESS+204)));      //REG 102

int ind_off_1 __attribute__ ((address(RAM_START_ADDRESS+206)));      //REG 103
int ind_off_2 __attribute__ ((address(RAM_START_ADDRESS+208)));      //REG 104
int ind_off_3 __attribute__ ((address(RAM_START_ADDRESS+210)));      //REG 105
int ind_off_4 __attribute__ ((address(RAM_START_ADDRESS+212)));      //REG 106

int  __attribute__ ((address(RAM_START_ADDRESS+214))) channel_num_U=0;      //REG 107
int  __attribute__ ((address(RAM_START_ADDRESS+216))) imp_kol=90;           //REG 108
int  __attribute__ ((address(RAM_START_ADDRESS+218))) V=0;                  //REG 109
int  __attribute__ ((address(RAM_START_ADDRESS+220))) D_Out_Init=0;         //REG 110
int  __attribute__ ((address(RAM_START_ADDRESS+222))) AD_12b=0;             //REG 111

int  __attribute__ ((address(RAM_START_ADDRESS+224))) Control=0;             //REG 112

int imp_kol_temp = 0;
int PARAM_SET;
int ind_off = 0x07;

// Flash storage for permanent modbus registers
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_K[7] = {1., 1., 1., 1., 1., 1., 1.};
int _FLASH_STORE _FLASH_ACCESS flash_data_buf_OFFSET[7] = {0, 0, 0, 0, 0, 0, 0};

int PROF=1;
char MENU_LEVEL = 0;

int _temp_BAUD=0;

int *RamData;

// Input filtering
static long int arr[7]= {0, 0, 0, 0, 0, 0, 0};
static unsigned int init[7] = {0, 0, 0, 0, 0, 0, 0};

#define FILTER_COUNT 1000

// Filters ADC input data
unsigned int filter (unsigned int  value,char channel_num) {
    if (init[channel_num] < FILTER_COUNT) {
        arr[channel_num] += value;
        ++init[channel_num];
        return 0;
    } else {
        arr[channel_num] += (-arr[channel_num]/FILTER_COUNT + value);
        return arr[channel_num]/FILTER_COUNT;
    }
}

void __attribute__((interrupt,no_auto_psv)) _QEIInterrupt() {
     IFS3bits.QEIIF = 0;
}

void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt() {
   A0=filter(ADC1BUF0,0);
   A1=filter(ADC1BUF1,1);
   A2=filter(ADC1BUF2,2);
//   A3=filter(ADC1BUF6,3);
//   A4=filter(ADC1BUF0,4);
//   A5=filter(ADC1BUF1,5);
//   A6=filter(ADC1BUF2,6);
   
   ADC0=((int)A0 - OFS_ADC0)*K0;   //AN6
   ADC1=((int)A1 - OFS_ADC1)*K1;   //AN3
   ADC2=((int)A2 - OFS_ADC2)*K2;   //AN4
//   ADC3=((int)A3 - OFS_ADC3)*K3;   //AN5
//   ADC4=((int)A4 - OFS_ADC4)*K4;   //AN0
//   ADC5=((int)A5 - OFS_ADC5)*K5;   //AN1
//   ADC6=((int)A6 - OFS_ADC6)*K6;   //AN2
 
   IFS0bits.AD1IF=0;
}


MAIN_DECL_LOOP_FN();

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main() {
    
    /* Initialize reset parser */
    system_init();

    mempool_init();
    
    cpu_ipl_set(0);
    
    /* Initialize Change-notification inputs */
    cni_init();
    
    /* Initialize oscillator switcher */
    osc_init();

    // Select Internal FRC with PLL
    osc_select(oscBoosted);
    
    /* Initialize UART */
    uart_init();
    
    /* Initialize RTSP */
    flash_init();
    
    /* Initialize system timing */
    timing_init();
    
    ADC_Init(1);
    discrete_init();
    
    FLASH_WR = 0;
    
    Ind_Delay = 100; 
    N = 4000;
    BRG_VAL = 19200;
   
    uint16_t i = 0;
    float *adcCoeffPtr = &K0;
    int *adcOffsetPtr = OFS_ADC0;
    uint16_t *tmpPtr;
    RamData = &(BRG_VAL);

    // Init permanently stored values for coeffs
    for (i = 0; i < 7; ++i) {
        adcCoeffPtr[i]  = flash_data_buf_K[i];
        adcOffsetPtr[i] = flash_data_buf_OFFSET[i];
    }
    
    // Initialize application-specific module
    app_init();
    
    // Main cycle
    while (1) {
        // Perform RTSP, if externally requested
        if (FLASH_WR == 1) {
            for (i = 0; i < 7; i++) {
                if (flash_data_buf_K[i] != adcCoeffPtr[i]) {
                    // Only perform if the data has changed, spare memory
                    tmpPtr = adcCoeffPtr+i;
                    flash_set(FLASH_GETPAGE(flash_data_buf_K), FLASH_GETAOFFSET(flash_data_buf_K, i),
                            tmpPtr[0]);
                    flash_set(FLASH_GETPAGE(flash_data_buf_K), FLASH_GETAOFFSET(flash_data_buf_K, i)+2,
                            tmpPtr[1]);
                }
                if (flash_data_buf_OFFSET[i] != adcOffsetPtr[i]) {
                    // Only perform if the data has changed, spare memory
                    flash_set(FLASH_GETPAGE(flash_data_buf_OFFSET), FLASH_GETAOFFSET(flash_data_buf_OFFSET, i),
                            adcOffsetPtr[i]);
                }
            }
            flash_write();
            system_reset();
            FLASH_WR = 0;
        }
        
        // Perform Modbus protocol processing
        Modbus_RTU();
        
        // Internal Modbus function for framing
        RS_Update();
        
        // Update discrete outputs and resample discrete inputs
        discrete_update();
        
        // Call application-end function
        MAIN_CALL_LOOP_FN();
        
        // Clear WDT flag to indicate normal operation
        wdt_clr();
    }
}


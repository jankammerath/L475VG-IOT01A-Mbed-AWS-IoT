#include "mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "mbedtls/debug.h"

#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"

#include "core/cloud.h"

#define AWS_IOT_UPDATE_FREQUENCY    60

DigitalOut led(LED1);

/*
    Blinks or not
    val         1 = blink, 0 = don't
*/
void blink(int val){
    if(val == -1){
        for(int x=0; x<4; x++){
            led = 1;
            wait(0.8);
            led = 0;
            wait(0.8);
        } 
    }
    
    if(val == 0){
        led = 0;
        wait(0.5); 
    }
    
    if(val == 1){
        led = 1;
        wait(0.5);      
    }
}

/* connect services */
int connect(){
    int result = 0;

    int cloud_status = cloud_connect();
    if(cloud_status == 1){
        result = 1;    
    }
    
    return result;
}

/* just the funky start */
int main(){
    mbed_trace_init();
    
    printf("IoT device starting\r\n");
    
    /* define sensor vars */
    float temperature = 0;
    float humidity = 0;
    float pressure = 0;
    
    /* initalise sensors */
    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();
    
    /* init cloud (connect wifi) */
    cloud_init();
    
    /* connect to network */
    int connected = connect();
    while(1) {
        if(connected == 1){
            /* all good here */
            blink(1);
            
            /* get current sensor data */
            temperature = BSP_TSENSOR_ReadTemp();
            humidity = BSP_HSENSOR_ReadHumidity();
            pressure = BSP_PSENSOR_ReadPressure();
            
            /* send message to cloud */
            cloud_send(temperature,humidity,pressure);
            
            /* wait secs as defined */
            wait(AWS_IOT_UPDATE_FREQUENCY);
        }else{
            /* give it time */
            wait(2);
            
            /* network is off */    
            blink(-1);
        }
    }
}
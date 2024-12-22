/* ------------------------------------------
	Ka Yi Cheng
	ECS642 Lab 6
	KL28Z Version
				
    Start at green and red RGB LED flashing
    Check the input commands ('faster' and 'slower') through the serial terminal
    If the command is received, change the LED flahing delay and upade the active delay
    If 'slower' command is entered, the flashing time is increasing (like 4s -> 0.5s -> 1s)
    If 'faster' command is entered, the time delay will decrease (like 4s -> 3.5s)
    If an unknow command is entered, an error messsge is sent and the red RGB LED is on
  -------------------------------------------- */
#include "cmsis_os2.h"
#include "string.h"
#include <MKL28Z7.h>
#include <stdbool.h>
#include "..\inc\rgb.h"
#include "..\inc\mask.h"
#include "..\inc\serialPort.h"

/* --------------------------------------------
  Set the Green and Red RGB LEDs
*----------------------------------------------------------------------------*/
#define GREEN_LED 0
#define RED_LED 1

/* --------------------------------------------
  Global variables
*----------------------------------------------------------------------------*/
osEventFlagsId_t eventFlagHandler;
osMessageQueueId_t delayMessageQueue;

#define NUM_DELAYS 8   
const uint32_t delayOptions[NUM_DELAYS] = {500, 1000, 1500, 2000, 2500, 3000, 3500, 4000};
int currentDelayIndex = 0; // start from 0.5s

/* --------------------------------------------
  id of thread to control the LEDs and checking the user input 
*----------------------------------------------------------------------------*/
osThreadId_t t_ledController, t_inputHandler;

/* --------------------------------------------
  Thread t_ledController - control the LEDs
*----------------------------------------------------------------------------*/
void ledControllerT(void *arg) {
    uint32_t activeDelay = delayOptions[currentDelayIndex];
    uint32_t startTime = osKernelGetTickCount();
    uint32_t timeElapsed, timeRemaining;
    uint32_t receivedDelay;

    // Initialize LEDs
    bool isRedLEDActive = true;
    setRGB(RED_LED, RGB_ON);
    setRGB(GREEN_LED, RGB_OFF);

    while (1) {
        timeElapsed = osKernelGetTickCount() - startTime;

        // Check if there has a new delay value is received or not
        osStatus_t queueStatus = osMessageQueueGet(delayMessageQueue, &receivedDelay, NULL, (timeElapsed >= activeDelay) ? 0 : activeDelay - timeElapsed);

	// New delay is received
        if (queueStatus == osOK) {
            timeRemaining = activeDelay - timeElapsed;
            if (timeRemaining > 0) {
                activeDelay = receivedDelay - timeElapsed; 
                continue;
            }
        }

	//Changing the LEDs
        if (timeElapsed >= activeDelay) {
            if (isRedLEDActive) {
                setRGB(RED_LED, RGB_OFF);
                setRGB(GREEN_LED, RGB_ON);
            } else {
                setRGB(GREEN_LED, RGB_OFF);
                setRGB(RED_LED, RGB_ON);
            }

            isRedLEDActive = !isRedLEDActive; 
            startTime = osKernelGetTickCount(); // Reset the timer
            activeDelay = delayOptions[currentDelayIndex]; // Update the delay
        }
    }
}

/* --------------------------------------------
  Thread t_inputHandler - checking the user input from the terminal
*----------------------------------------------------------------------------*/
void inputHandlerT(void *arg) {
    char inputBuffer[10];
    uint32_t newDelay;

    while (1) {
        // Wait for input from the user
        if (readLine(inputBuffer, sizeof(inputBuffer) - 1)) {
	    // If the user type 'slower', the delay option will go right for one index, which means the time will increase
            if (strcmp(inputBuffer, "slower") == 0) {
                currentDelayIndex = (currentDelayIndex + 1) % NUM_DELAYS;
                newDelay = delayOptions[currentDelayIndex];
                osMessageQueuePut(delayMessageQueue, &newDelay, 0, NULL);
            } 
	    // If the user type 'faster', the delay option will go left for one index, which means the time will decrease
            else if (strcmp(inputBuffer, "faster") == 0) {
                currentDelayIndex = (currentDelayIndex - 1 + NUM_DELAYS) % NUM_DELAYS;
                newDelay = delayOptions[currentDelayIndex];
                osMessageQueuePut(delayMessageQueue, &newDelay, 0, NULL);
            } 
	    // If an unknow command is entered, an error messsge is sent
            else {
                sendMsg("Unknown command received", CRLF);
            }
        }
    }
}

int main(void) {
    // System initialization
    SystemCoreClockUpdate();
    configureRGB();
    init_UART0(115200);
    osKernelInitialize();

    // Create RTOS resources
    eventFlagHandler = osEventFlagsNew(NULL);
    delayMessageQueue = osMessageQueueNew(2, sizeof(uint32_t), NULL);

    // Create threads
    t_ledController = osThreadNew(ledControllerT, NULL, NULL);
    t_inputHandler = osThreadNew(inputHandlerT, NULL, NULL);

    // Start RTOS kernel
    osKernelStart();

    for (;;) {}
}

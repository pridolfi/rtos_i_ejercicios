/* Copyright 2015, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of lpc1769_template.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @brief This is a simple C example file.
 **
 **/

/** \addtogroup rtos_blink FreeRTOS blink example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "main.h"

#include "ciaaIO.h"
#include "ciaaUART.h"


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

xSemaphoreHandle sem[3];

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
    SystemCoreClockUpdate();

    Board_Init();

    ciaaIOInit();

    ciaaWriteOutput(0, 0);

    ciaaUARTInit();

}

static void task(void * a)
{
	taskData * t = (taskData *)a;
	uint8_t i;

	while (1) {

	   ciaaWriteOutput(t->nled, 1);
	   vTaskDelay(t->delay);
	   ciaaWriteOutput(t->nled, 0);

	   xSemaphoreGive(sem[t->ntarea]);
	   xSemaphoreGive(sem[t->ntarea]);

	   for (i=0; i<3; i++) {
		   if ( i != t->ntarea ) {
			   xSemaphoreTake(sem[i], portMAX_DELAY);
		   }
	   }
	}
}

/*==================[external functions definition]==========================*/


int main(void)
{
	static taskData t[3] =
	{
			{0, 0, 100},
			{1, 3, 500},
			{2, 5, 1000}
	};

	initHardware();

	sem[0] = xSemaphoreCreateCounting(2, 0);
	sem[1] = xSemaphoreCreateCounting(2, 0);
	sem[2] = xSemaphoreCreateCounting(2, 0);


	xTaskCreate(task, (const char *)"task1", configMINIMAL_STACK_SIZE*2, &(t[0]), tskIDLE_PRIORITY+1, 0);
	xTaskCreate(task, (const char *)"task3", configMINIMAL_STACK_SIZE*2, &(t[1]), tskIDLE_PRIORITY+1, 0);
	xTaskCreate(task, (const char *)"task2", configMINIMAL_STACK_SIZE*2, &(t[2]), tskIDLE_PRIORITY+1, 0);

	vTaskStartScheduler();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/

/*
 * MIT License
 *
 * Copyright (c) 2020 Newracom, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "nrc_sdk.h"

//#define L3G4200D	/* VDD: 3.3v, SPI_mode: 11(CPOL=1,CPHA=1), MSB is 1 when read */
//#define AT45DBXX
#define LIS331HH
//#define BME680	  /* VDD: 3.3v, SPI_mode: 00(CPOL=0,CPHA=0) or 11(CPOL=1,CPHA=1), MSB is 1 when read */

#define TEST_COUNT 10
#define TEST_INTERVAL 2000 /* msec */

/******************************************************************************
 * FunctionName : run_sample_spi
 * Description  : sample test for spi
 * Parameters   : count(test count), interval(test interval)
 * Returns      : 0 or -1 (0: success, -1: fail)
 *******************************************************************************/
int run_sample_spi(int count, int interval)
{
	int i=0;
	nrc_usr_print("[%s] Sample App for SPI API \n",__func__);

	nrc_spi_init(SPI_MODE3, SPI_BIT8, 1000000);
	nrc_spi_enable(true);
	_delay_ms(100);

#if defined (L3G4200D)
	nrc_usr_print("[%s] L3G4200D : WHO_AM_I is 0xD3.\n", __func__);		/* WHO_AM_I: 0xD3 */
	for(i=0; i<count; i++) {
		_delay_ms(interval);

		if (nrc_spi_readbyte(0x0F|0x80) == 0xD3){
			nrc_usr_print("[%s] WORKS FINE!!!!!\n", __func__);
		} else {
			nrc_usr_print("[%s] ERROR..........\n", __func__);
			return RUN_FAIL;
		}
	}
#elif defined (BME680)
	nrc_usr_print("[%s] BME680 : spi_mem_page default value=0x%02x\n", __func__, nrc_spi_readbyte(0x73|0x80));
	while (1)
	{
		nrc_spi_writebyte(0x73, 0x00);
		uint8_t ret = nrc_spi_readbyte(0x73|0x80);
		if (ret == 0) {
			nrc_usr_print("[%s] spi_mem_page changed successfully!!\n", __func__);
			break;
		} else  {
			nrc_usr_print("[%s] spi_mem_page change failed.retry...........\n", __func__);;
		}
		_delay_ms(300);
	}

	/* chip_id: 0x61 */
	nrc_usr_print("[%s] BME680 Chid_ID is 0x61.\n", __func__);
	for(i=0; i<count; i++) {
		_delay_ms(interval);
		if (nrc_spi_readbyte(0x50|0x80) == 0x61){
			nrc_usr_print("[%s] WORKS FINE!!!!!\n", __func__);
		}else {
			nrc_usr_print("[%s] ERROR..........\n", __func__);
			return RUN_FAIL;
		}
	}
#elif defined (AT45DBXX)
	uint8_t tx[] = {0x9F, 0x00, 0x00, 0x00, 0x00};
	uint8_t rx[8] = {0,};

	nrc_usr_print("[AT45DBXX]\n");
	for(i=0; i<count; i++) {
		_delay_ms(interval);
		uint8_t ret = nrc_spi_xfer(tx, rx, 5);
		if (ret == 5) {
			nrc_usr_print("[%s] 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x \n", __func__,
			rx[0], rx[1], rx[2], rx[3], rx[4]);

			for (int i = 0; i < 8; i++){
				rx[i] = 0;
			}
		}else {
			nrc_usr_print("Does not match the actual number of bytes transmitted.\n\n");
			return RUN_FAIL;
		}
	}
#elif defined (LIS331HH)
	uint8_t tx[] = {0xA0, 0x00};
	uint8_t rx[8] = {0,};

	nrc_usr_print("[LIS331HH] '0xff, 0x7, 0X7, 0X7, 0X7' should be printed.\n");

	for(i=0; i<count; i++) {
		_delay_ms(interval);
		uint8_t ret = nrc_spi_xfer(tx, rx, 5);
		if (ret == 5) {
			nrc_usr_print("[%s] 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x \n", __func__,
			rx[0], rx[1], rx[2], rx[3], rx[4]);

			for (int i = 0; i < 8; i++){
				rx[i] = 0;
			}
		}else {
			nrc_usr_print("Does not match the actual number of bytes transmitted.\n\n");
			return RUN_FAIL;
		}
	}
#endif
	return RUN_SUCCESS;
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : Start Code for User Application, Initialize User function
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void user_init(void)
{
	int ret = 0;

	//Enable Console for Debugging
	nrc_uart_console_enable();

	ret = run_sample_spi(TEST_COUNT, TEST_INTERVAL);
	nrc_usr_print("[%s] test result!! %s \n",__func__, (ret==0) ?  "Success" : "Fail");
}

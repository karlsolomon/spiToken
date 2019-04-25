#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define TEST_DEBUG_FULL  0
#define SPI_CS_PIN 	     17
#define LOFO 		     18
#define LED_TOKEN 	     19
#define LED_INPROGRESS   26
#define LED_FAIL 	     6
#define LED_SUCCESS 	 13

#define MIN(a,b)    ((a < b) ? a : b)

#define FILE_PATH        "/home/pi/Documents/CODE/spiToken/src/Pluto_FULL_TOKEN.bin"

#define TEST_TOKEN_RW_SIZE      256
#define TOK_F_WRITE             ((WriteAndVerifyHook) TokenFlash_Write)
#define TOK_F_ERASE             ((EraseHook) TokenFlash_Erase)
#define TOK_F_READ              ((WriteAndVerifyHook) TokenFlash_Read)
#define TEST_TOKEN_START_ADDR   0

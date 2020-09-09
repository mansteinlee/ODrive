
/* Includes ------------------------------------------------------------------*/

#include "communication.h"

#include "interface_usb.h"
#include "interface_uart.h"
#include "interface_can.hpp"
#include "interface_i2c.h"

#include "odrive_main.h"
#include "freertos_vars.h"
#include "utils.hpp"

#include <cmsis_os.h>
#include <memory>
//#include <usbd_cdc_if.h>
//#include <usb_device.h>
//#include <usart.h>
#include <gpio.h>

#include <type_traits>

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Global constant data ------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

uint64_t serial_number;
char serial_number_str[13]; // 12 digits + null termination

float oscilloscope[OSCILLOSCOPE_SIZE] = {0};
size_t oscilloscope_pos = 0;

/* Private constant data -----------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Function implementations --------------------------------------------------*/

void init_communication(void) {
    printf("hi!\r\n");

    if (odrv.config_.enable_uart0 && uart0) {
        start_uart_server();
    }

    start_usb_server();

    if (odrv.config_.enable_i2c0) {
        start_i2c_server();
    }

    if (odrv.config_.enable_can0) {
        odCAN->start_can_server();
    }
}

#include <fibre/../../async_stream.hpp>


extern "C" {
int _write(int file, const char* data, int len);
}

// @brief This is what printf calls internally
int _write(int file, const char* data, int len) {
    fibre::cbufptr_t buf{(const uint8_t*)data, (const uint8_t*)data + len};

    if (odrv.config_.uart0_protocol == ODrive::STREAM_PROTOCOL_TYPE_STDOUT ||
        odrv.config_.uart0_protocol == ODrive::STREAM_PROTOCOL_TYPE_ASCII_AND_STDOUT) {
        uart0_stdout_sink.write(buf);
    }

    if (odrv.config_.usb_cdc_protocol == ODrive::STREAM_PROTOCOL_TYPE_STDOUT ||
        odrv.config_.usb_cdc_protocol == ODrive::STREAM_PROTOCOL_TYPE_ASCII_AND_STDOUT) {
        usb_cdc_stdout_sink.write(buf);
    }

    return len; // Always pretend that we processed everything
}


#include "../autogen/function_stubs.hpp"

ODrive& ep_root = odrv;
#include "../autogen/endpoints.hpp"

/*
 * uart_cli.c
 *
 *  Created on: Mar 25, 2021
 *      Author: Wiktor Lechowicz
 */

/* private includes */
#include "uart_cli.h"
#include <stdint.h>
#include "string.h"
#include "stm32f3xx.h"

/* private defines */
#define MAX_LINE_LEN                        40
#define BUFF_LEN                            400

/* private variables */

static struct cli {
    UART_HandleTypeDef      *huart;
    char                    message_buff[BUFF_LEN];
} base;

/* exported functions */
void CLI_Init(UART_HandleTypeDef *huart)
{
    base.huart = huart;
}

bool CLI_print_mem_content(void *mem_p,  struct CLI_field_descriptor *descriptor, uint16_t num_elements)
{

    if(mem_p == NULL || descriptor == NULL) return false;

    uint16_t message_len = 0;
    for(uint16_t i = 0; i < num_elements; i++){

        for(uint16_t j = 0; j < descriptor[i].num_elements; j++){
            switch(descriptor[i].type){

            case CLI_FIELD_UINT8:
                if(descriptor[i].num_elements == 1){
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s: %u\n\r" , descriptor[i].cli_tag, *((uint8_t*) mem_p));
                } else {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s[%d]: %u\n\r" , descriptor[i].cli_tag, j, *((uint8_t*) mem_p));
                }
                mem_p += sizeof(uint8_t);
                break;

            case CLI_FIELD_UINT16:
                if(descriptor[i].num_elements == 1) {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s: %u\n\r", descriptor[i].cli_tag, *((uint16_t*) mem_p));
                } else {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s[%d]: %u\n\r", descriptor[i].cli_tag, j, *((uint16_t*) mem_p));
                }
                mem_p += sizeof(uint16_t);
                break;

            case CLI_FIELD_UINT32:
                if(descriptor[i].num_elements == 1) {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s: %lu\n\r", descriptor[i].cli_tag, *((uint32_t*) mem_p));
                } else {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s[%d]: %lu\n\r", descriptor[i].cli_tag, j, *((uint32_t*) mem_p));
                }
                mem_p += sizeof(uint32_t);
                break;

            case CLI_FIELD_INT8:
                if(descriptor[i].num_elements == 1) {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s: %d\n\r", descriptor[i].cli_tag, *((int8_t*) mem_p));
                } else {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s[%d]: %d\n\r", descriptor[i].cli_tag, j, *((int8_t*) mem_p));
                }
                mem_p += sizeof(int8_t);
                break;

            case CLI_FIELD_INT16:
                if(descriptor[i].num_elements == 1) {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s: %d\n\r", descriptor[i].cli_tag, *((int16_t*) mem_p));
                } else {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s[%d]: %d\n\r", descriptor[i].cli_tag, j, *((int16_t*) mem_p));
                }
                mem_p += sizeof(int16_t);
                break;

            case CLI_FIELD_INT32:
                if(descriptor[i].num_elements == 1) {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s: %ld\n\r", descriptor[i].cli_tag, *((int32_t*) mem_p));
                } else {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s[%d]: %ld\n\r", descriptor[i].cli_tag, j, *((int32_t*) mem_p));
                }
                mem_p += sizeof(int32_t);
                break;

            case CLI_FIELD_CHAR:
                if(descriptor[i].num_elements == 1) {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s: %c\n\r", descriptor[i].cli_tag, *((char*) mem_p));
                } else {
                    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "%s[%d]: %c\n\r", descriptor[i].cli_tag, j, *((char*) mem_p));
                }
                mem_p += sizeof(char);
                break;

            default:
                break;
            }
        }

    }
    /* transmit whole buffer using DMA */
    base.message_buff[message_len++] = '\n';
    base.message_buff[message_len++] = '\r';
    HAL_UART_Transmit_DMA(base.huart, (uint8_t*)&base.message_buff, message_len);

    return true;
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(base.huart);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}



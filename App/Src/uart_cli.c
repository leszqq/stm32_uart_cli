/*
 * uart_cli.c
 *
 *  Created on: Mar 25, 2021
 *      Author: Wiktor Lechowicz
 */
#define BACKSPACE 127
#define ENTER     13
#define DELETE    0x7F

/* === private macros === */
#define PRINT(S, ...) do { \
                            while(!(base.huart->gState == HAL_UART_STATE_READY)){};\
                            HAL_UART_Transmit_DMA(base.huart,(uint8_t*) base.message_buff, snprintf(base.message_buff, MESSAGE_BUFF_LEN, S, ##__VA_ARGS__)); \
                         }while(0)

/* private includes */
#include "uart_cli.h"
#include <stdint.h>
#include "string.h"
#include "stm32f3xx.h"
#include "main.h"

/* private defines */
#define MAX_LINE_LEN                        40
#define MESSAGE_BUFF_LEN                   400
#define RECEIVED_BUFF_LEN                   30

/* === private variables === */

static struct cli {
    UART_HandleTypeDef              *huart;
    void                            *mem_p;
    struct CLI_field_descriptor     *mem_descriptor;
    uint16_t                        num_mem_elements;
    char                            message_buff[MESSAGE_BUFF_LEN];
    char                            received_buff[MESSAGE_BUFF_LEN];
    uint8_t                         received_index;
} base;

/* === static functions === */
/* print memory content */
static void print_mem_content(void *mem_p,  struct CLI_field_descriptor *descriptor, uint16_t num_elements)
{
    assert_param(descriptor);

    uint16_t message_len = 2;
    base.message_buff[0] = '\n';
    base.message_buff[1] = '\r';
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
    message_len += snprintf(&base.message_buff[message_len], MAX_LINE_LEN, "\n\r>>");
    HAL_UART_Transmit_DMA(base.huart, (uint8_t*)&base.message_buff, message_len);

    return;
}

/* === exported functions === */
void CLI_Init(UART_HandleTypeDef *huart, void *mem_p, struct CLI_field_descriptor *descriptor, uint16_t num_elements)
{
    assert_param(huart);
    assert_param(mem_p);
    assert_param(descriptor);

    base.huart = huart;
    base.mem_p = mem_p;
    base.mem_descriptor = descriptor;
    base.num_mem_elements = num_elements;
    base.received_index = 0;

    PRINT("Type in HELP for command list\n\r>>");

    HAL_UART_Receive_IT(base.huart, (uint8_t *)&base.received_buff[0], 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

    if(huart == base.huart){
        if(base.received_buff[base.received_index] == BACKSPACE){
            if(base.received_index > 0) {
                PRINT("\b \b");
                base.received_index--;
            }

        } else if ( base.received_buff[base.received_index] == ENTER){
            if(base.received_index == 0){
                PRINT("\n\r>>");

            }else if(strncmp((char *)&base.received_buff, "LED ON", 6) == 0){
                HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
                PRINT("\n\r>>");

            }else if(strncmp((char *)&base.received_buff, "LED OFF", 7) == 0) {
                HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
                PRINT("\n\r>>");

            }else if(strncmp((char*)&base.received_buff, "HELP", 4) == 0) {
                PRINT("\n\ravailable commands:\n\r"
                        "LED ON\n\r"
                        "LED OFF\n\r"
                        "HELP\n\r"
                        "PRINT MEM\n\r>>");

            } else if(strncmp((char*)&base.received_buff, "PRINT MEM", 9) == 0) {
                print_mem_content(base.mem_p, base.mem_descriptor, base.num_mem_elements);
            } else{
                PRINT("\n\rwrong command\n\r>>");
            }
            base.received_index = 0;

        }else {
             HAL_UART_Transmit_IT(base.huart, (uint8_t *)&base.received_buff[base.received_index], 1);
             base.received_index++;
        }
        HAL_UART_Receive_IT(base.huart, (uint8_t *)&base.received_buff[base.received_index], 1);
    }
}


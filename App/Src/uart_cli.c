/*
 * uart_cli.c
 *
 *  Created on: Mar 25, 2021
 *      Author: Wiktor Lechowicz
 */

/* private includes */
#include "uart_cli.h"
#include <stdint.h>
#include <assert.h>
#include "string.h"

/* private defines */
#define MAX_LINE_LEN                            40

/* exported functions */

bool CLI_print_mem_content(UART_HandleTypeDef *huart, void *mem_p,  struct CLI_field_descriptor *descriptor, uint16_t num_elements)
{
    assert(huart);
    assert(mem_p);
    assert(descriptor);

    if(huart == NULL || mem_p == NULL || descriptor == NULL) return false;

    uint8_t message_buff[MAX_LINE_LEN];
    for(uint16_t i = 0; i < num_elements; i++){

        for(uint16_t j = 0; j < descriptor[i].num_elements; j++){
            switch(descriptor[i].type){

            case CLI_FIELD_UINT8:
                if(descriptor[i].num_elements == 1){
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s: %u\n\r" , descriptor[i].cli_tag, *((uint8_t*) mem_p));
                } else {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s[%d]: %u\n\r" , descriptor[i].cli_tag, j, *((uint8_t*) mem_p));
                }
                mem_p += sizeof(uint8_t);
                break;

            case CLI_FIELD_UINT16:
                if(descriptor[i].num_elements == 1) {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s: %u\n\r", descriptor[i].cli_tag, *((uint16_t*) mem_p));
                } else {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s[%d]: %u\n\r", descriptor[i].cli_tag, j, *((uint16_t*) mem_p));
                }
                mem_p += sizeof(uint16_t);
                break;

            case CLI_FIELD_UINT32:
                if(descriptor[i].num_elements == 1) {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s: %lu\n\r", descriptor[i].cli_tag, *((uint32_t*) mem_p));
                } else {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s[%d]: %lu\n\r", descriptor[i].cli_tag, j, *((uint32_t*) mem_p));
                }
                mem_p += sizeof(uint32_t);
                break;

            case CLI_FIELD_INT8:
                if(descriptor[i].num_elements == 1) {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s: %d\n\r", descriptor[i].cli_tag, *((int8_t*) mem_p));
                } else {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s[%d]: %d\n\r", descriptor[i].cli_tag, j, *((int8_t*) mem_p));
                }
                mem_p += sizeof(int8_t);
                break;

            case CLI_FIELD_INT16:
                if(descriptor[i].num_elements == 1) {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s: %d\n\r", descriptor[i].cli_tag, *((int16_t*) mem_p));
                } else {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s[%d]: %d\n\r", descriptor[i].cli_tag, j, *((int16_t*) mem_p));
                }
                mem_p += sizeof(int16_t);
                break;

            case CLI_FIELD_INT32:
                if(descriptor[i].num_elements == 1) {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s: %ld\n\r", descriptor[i].cli_tag, *((int32_t*) mem_p));
                } else {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s[%d]: %ld\n\r", descriptor[i].cli_tag, j, *((int32_t*) mem_p));
                }
                mem_p += sizeof(int32_t);
                break;

            case CLI_FIELD_CHAR:
                if(descriptor[i].num_elements == 1) {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s: %c\n\r", descriptor[i].cli_tag, *((char*) mem_p));
                } else {
                    snprintf((char*) message_buff, MAX_LINE_LEN, "%s[%d]: %c\n\r", descriptor[i].cli_tag, j, *((char*) mem_p));
                }
                mem_p += sizeof(char);
                break;

            default:
                break;
            }
            HAL_UART_Transmit(huart, message_buff, strlen((char*)message_buff), HAL_MAX_DELAY);
        }

    }
    uint8_t new_l[2] = "\n\r";
    HAL_UART_Transmit(huart, new_l, 2, HAL_MAX_DELAY);

    return true;
}

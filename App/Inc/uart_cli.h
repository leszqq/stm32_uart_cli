/*
 * uart_cli.c
 *
 *  Created on: Mar 25, 2021
 *      Author: Wiktor Lechowicz
 *
 *      This code contain reusable code for usart based command line interface targeted for stm32 MCUs based on HAL drivers.
 *
 *          * HOW TO USE *
 *          Create struct CLI_field_destrictor array to describe content of memory with tag strings and type specifiers CLI_field_type.
 *
 *          Initialize CLI (CLI_Init()) with just created memory descriptor.
 *          Example in main.c
 */

/* === exported includes === */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f3xx_hal.h"

/* === exported defines === */
#define CLI_MAX_TAG_LEN         20                                 // maximum length of memory field tag displayed in CLI.

/* === exported types === */

enum CLI_field_type {
    CLI_FIELD_UINT8,
    CLI_FIELD_UINT16,
    CLI_FIELD_UINT32,

    CLI_FIELD_INT8,
    CLI_FIELD_INT16,
    CLI_FIELD_INT32,

    CLI_FIELD_CHAR,
};

struct CLI_field_descriptor {
    char                            cli_tag[CLI_MAX_TAG_LEN];   // string displayed in terminal which describes displayed memory content
    enum CLI_field_type             type;                       // field type, enum
    uint16_t                        size;                       // size of variable in bytes, use sizeof(variable), works as well for arrays as sizeof(array)
    uint16_t                        num_elements;               // number of variables in array, 1 for single variables
};

/* === exported functions === */

/** @brief cli initialization function
 * @param huart         - uart handle
 * @param mem_p         - pointer to printed out memory content
 * @param descriptor    - pointer to memory descriptor struct
 * @param num_elements  - number of variables in printed memory space
 * @retval void  */
void CLI_Init(UART_HandleTypeDef *huart, void *mem_p, struct CLI_field_descriptor *descriptor, uint16_t num_elements);

#ifndef APP_INC_UART_CLI_C_
#define APP_INC_UART_CLI_C_

#endif /* APP_INC_UART_CLI_C_ */

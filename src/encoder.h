#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include "pin.h"

#define ENC_STOP    0
#define ENC_INC     1
#define ENC_DEC     2

typedef struct {
    pin_t pin_A;
    pin_t pin_B;
    int8_t position;
    bool pin_A_last;
} encoder_t;

encoder_t enc_create(volatile uint8_t *port_pin_A, uint8_t pin_A, volatile uint8_t *port_pin_B, uint8_t pin_B);

void enc_update_position(encoder_t *enc);


int8_t get_position(encoder_t *enc);

void reset_position(encoder_t *enc);

#endif

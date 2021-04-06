#include "encoder.h"
#include "pin.h"

/**
 * @brief   Creates an encoder entity
 * 
 * @param   port_pin_A    The microprocessor port (&PORTA, &PORTB...) of the ENC_A pin
 * @param   pin_A         The port's pin of the ENC_A signal
 * @param   port_pin_B    The microprocessor port (&PORTA, &PORTB...) of the ENC_B pin
 * @param   pin_B         The port's pin of the ENC_B signal
 * 
 * @return  The encoder entity (encoder_t type)
 */
encoder_t enc_create(volatile uint8_t *port_pin_A, uint8_t pin_A, volatile uint8_t *port_pin_B, uint8_t pin_B){
    encoder_t enc;
    enc.pin_A = pin_bind(port_pin_A, pin_A, InputPullup);
    enc.pin_B = pin_bind(port_pin_B, pin_B, InputPullup);
    enc.pin_A_last = pin_r(enc.pin_A);
    enc.position = 0;
    return enc;
}


void enc_update_position(encoder_t *enc){
    bool pin_A = pin_r(enc->pin_A);
    if (pin_A == false && pin_A != enc->pin_A_last){  //Falling Edge detected
      if (pin_A == pin_r(enc->pin_B)){
	enc->position--;
      } else {
	enc->position++;
      }
    }
    enc->pin_A_last = pin_A;
}


int8_t get_position(encoder_t *enc){
    return enc->position;
}

void reset_position(encoder_t *enc){
    enc->position = 0;
}

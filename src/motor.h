/** @file motor.h
 *  @brief Abstraction of a stepper motor.
 *  
 *  Implements the functions to control a stepper motor
 *  using three pins (Enable, Direction and Steps) that
 *  are defined as constants inside the module to reduce
 *  processing time.
 */

#ifndef _MOTOR_H_
#define _MOTOR_H_


#define MOTOR_STEPS_REV 200     /**< Define number of steps per revolution as configured by hardware */


/**
 * @brief Defines de direction of the motor.
 * 
 * If phisical motor turns in the oposite sense, set 'motor_unwind=0' instead of 'motor_wind'.
 */
typedef enum {motor_wind=0, motor_unwind} motor_dir_t;


/**
 * @brief Setup motor module.
 * 
 * Must be called before the rest of functions.
 */
void motor_setup(void);


/**
 * @brief Order one step.
 * 
 * Motor needs to be previously enabled by motor_enable()
 */
void motor_step(void);


/**
 * @brief Set a given turning direction.
 * 
 * @param d The desired direction
 */
void motor_set_dir(motor_dir_t d);


/**
 * @brief Reverse the current direction.
 */
void motor_reverse(void);


/**
 * @brief Enable the motor.
 * 
 * Motor is power feeded only while enabled, thus braking torque 
 * only effective while enabled.
 */
void motor_enable(void);


/**
 * @brief Disable the motor.
 * 
 * When the motor is disabled, no steps are executed but
 * avoids power consumption by removing the braking torque.
 */
void motor_disable(void);

#endif

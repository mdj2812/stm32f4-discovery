/**
  ******************************************************************************
  * @file       KNX_Aux.c
  * @author     MA Dingjie
  * @version    V0.0.5
  * @date       20-July-2016
  * @brief      Auxiliary functions for KNX Physical Layer.
  *             This file provides functions to manage following functionalities:
  *              + Conversion functions from int to text and from text to int
  *              + A basic timer
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "KNX_Aux.h"

/** @addtogroup KNX_PH KNX Physical Layer
  * @{
  */

/** @defgroup KNX_Aux KNX Physical Layer Auxiliary
  * @{
  */
	
/* Private constants ---------------------------------------------------------*/
/** @defgroup KNX_Aux_Private_Consts Auxiliary Private constants
  * @{
  */
/** \brief Table of hexadecimal numbers */
static const char hex_num[] = {'0', '1', '2', '3', '4', '5', '6', '7' , '8', '9',
                                'A', 'B', 'C', 'D', 'E', 'F'};
/** \brief Status of the timer */
__IO TIMER_Status_t timer_state;
/** \brief Timeout duration of the timer */
__IO uint32_t timer_timeout;
/** \brief Counter of the timer, the unit is ms */
__IO uint32_t timer_tick;
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup KNX_Aux_Exported_Functions_Group1 KNX int/text conversion functions
  * @{
  */

/**
 *  @brief      Convert an \c uint8_t value into a \c char message
 *  @param      value: an 8 bits unsigned value.
 *  @param      msg: pointer to a buffer.
 *  @return     Error code, see \ref KNX_Aux_Error_Code
 */
uint8_t int2text(uint8_t value, char *msg)
{
  if(msg == NULL)
  {
    return AUX_ERROR_MSG;
  }
		
  *(msg+1) = hex_num[value%16]; /* get the value on digit 0 */
  *msg = hex_num[value/16];     /* get the value on digit 1 */
  
  return AUX_ERROR_NONE;
}

/**
 *  @brief      Convert a \c char message into an \c uint8_t value 
 *  @param      msg: pointer to a buffer of 2 digits.
 *  @param      value: pointer to ouput as an 8 bits unsigned value.
 *  @return     Error code, see \ref KNX_Aux_Error_Code
 */
uint8_t text2int(char *msg, uint8_t *value)
{
  uint8_t i;	/* indice */
  uint8_t value_0, value_1;	/* values on 2 digits */
	
  for(i=0; i<16; i++)
  {
    if(*msg == hex_num[i])
    {
      value_1 = i;
      break;
    }
    
    if(i == 15)                 /* didn't found the corresponding digit */
    {
      return AUX_ERROR_MSG;
    }
  }
  
  for(i=0; i<16; i++)
  {
    if(*(msg+1) == hex_num[i])
    {
      value_0 = i;
      break;
    }
    
    if(i == 15)                 /* didn't found the corresponding digit */
    {
      return AUX_ERROR_MSG;
    }
  }
  
  *value = value_0 + value_1*16;/* output value = d0*16^0+d1*16^1 */
  
  return AUX_ERROR_NONE;
}
/**
  * @}
  */

/** @defgroup KNX_Aux_Exported_Functions_Group2 KNX Auxiliary Timer Functions
  * @{
  */

/**
 *  @brief      Initilize the timer. 
 */
void KNX_InitTimer(void)
{
  timer_state = TIMER_RESET;
}

/**
 *  @brief      Start a timer of a timeout duration given 
 *  @param      Timeout: timeout duration of the timer.
 */
void KNX_StartTimer(uint32_t Timeout)
{
  /** Set timer state to running. */
  timer_state = TIMER_RUNNING;
  
  /** Set timer counter to 0. */
  timer_tick = 0;
  
  /** Set timeout duration. */
  timer_timeout = Timeout;
}

/**
 *  @brief      Get the timer. 
 *  @retval     Value of the timer.
 */
uint32_t KNX_GetTime(void)
{
  return timer_tick;
}

/**
 *  @brief      Get the timer's status. 
 *  @retval     Timer's status: ::TIMER_Status_t.
 */
TIMER_Status_t KNX_GetTimerState(void)
{
  return timer_state;
}

/**
 *  @brief      Stop the timer. 
 *  @retval     Value of the timer.
 */
uint32_t KNX_StopTimer(void)
{
  /** Set timer state to pause. */
  timer_state = TIMER_PAUSE;
  
  /** return ::timer_tick. */
  return timer_tick;
}

/**
 *  @brief      Reset the timer. 
 *  @retval     Value of the timer.
 */
uint32_t KNX_ResetTimer(void)
{
  uint32_t ret;
    
  /** Set timer state to reset. */
  timer_state = TIMER_RESET;
  
  /** return ::timer_tick and set it back to 0. */
  ret = timer_tick;
  timer_tick = 0;
  
  return ret;
}

/**
 *  @brief      Systick interrupt routines. 
 */
void KNX_systick_isr(void)
{
  if(timer_state == TIMER_RUNNING)
  {
    timer_tick++;
    
    if(timer_tick >= timer_timeout)
    {
      timer_state = TIMER_TIMEOUT;
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
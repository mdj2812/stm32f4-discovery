/*******************************************************************************
  * @file       debug.h 
  * @author  
  * @version    V3.0.0 
  * @date       14-July-2016
  * @brief      Debug tasks.
  ******************************************************************************
  */

#ifndef debug_H
#define debug_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#include "cola.h"
   
/** @addtogroup Cola_Debug
  * @{
  */

/** @addtogroup Debug
  * @{
  */

/* Defines -------------------------------------------------------------------*/
/** @defgroup Debug_Private_Define Debug Private Define
  * @{
  */
/** \brief Max size of the ::buffer */
#define BUFFER_SIZE 1024
/**
  * @}
  */

/* External variables --------------------------------------------------------*/
/** @defgroup Debug_External_Variables Debug External Variables
  * @{
  */
extern t_cola colaDebug;
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup Debug_Exported_Functions
  * @{
  */

/** @addtogroup Debug_Exported_Functions_Group1
  * @{
  */

/* Initialization functions ***************************************************/
void DebugInit(void);
/**
  * @}
  */

/** @addtogroup Debug_Exported_Functions_Group2
  * @{
  */

/* Debug tasks functions  *****************************************************/
void DebugTask(void * argument);
void DebugGuardar1sTask(void * argument);
void DebugGuardar2sTask(void * argument);
/**
  * @}
  */

/** @addtogroup Debug_Exported_Functions_Group3
  * @{
  */

/* Systick test isr functions  ************************************************/
void debug_systick_isr_begin(void);
void debug_systick_isr_1s(void);
void debug_systick_isr_2s(void);
void debug_systick_isr_end(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* debug_H */


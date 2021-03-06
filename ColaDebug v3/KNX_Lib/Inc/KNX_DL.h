/**
  ******************************************************************************
  * @file       KNX_DL.h
  * @author     MA Dingjie
  * @version    V0.0.1
  * @date       6-September-2016
  * @brief      This file contains KNX data link layer common defines,
  *             enumerations, macros, structures definitions, extern variables
  *             and all the functions prototypes.
  ******************************************************************************
  */

#ifndef __KNX_DL
#define __KNX_DL

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
   
/** @addtogroup KNX_Lib
  * @{
  */

/** @addtogroup KNX_DL
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup DL_Exported_Constants Data Link Layer Exported Constants
  * @brief    Data Link Layer Exported Constants
  * @{
  */

/** @defgroup DL_Error_Code Data Link Layer Error Code
  * @brief    Data Link Layer Error Code
  * @{
  */
#define DL_ERROR_NONE           ((uint8_t)0x00U)   /*!< No error              */
#define DL_ERROR_INIT           ((uint8_t)0x01U)   /*!< Initialization error  */
#define DL_ERROR_TIMEOUT        ((uint8_t)0x02U)   /*!< Timeout error         */
#define DL_ERROR_REQUEST        ((uint8_t)0x03U)   /*!< Invalid request error */
#define DL_ERROR_RESPONSE       ((uint8_t)0x04U)   /*!< Response error        */
#define DL_ERROR_DATA_CON_FAIL  ((uint8_t)0x05U)   /*!< Transmission failed   */
#define DL_ERROR_FRAME          ((uint8_t)0x06U)   /*!< Frame error           */
#define DL_ERROR_ADDRESS        ((uint8_t)0x06U)   /*!< Address error         */
#define DL_ERROR_BUSY           ((uint8_t)0x06U)   /*!< Busy                  */
/**
  * @}
  */
/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup KNX_DL_Exported_Types KNX Data Link Layer Exported Types
  * @brief    KNX Data Link Layer Status Enumeration
  * @{
  */

/**
  * @brief  DL Status Enumeration definition. State machine:
  *         \image html KNX_Ph_state_machine.jpg
  */
typedef enum
{
  DL_POWER_ON   = 0x00U,        /*!< Power On State                           */
  DL_RESET      = 0x01U,        /*!< Reset State                              */
  DL_NORMAL     = 0x02U,        /*!< Normal Mode                              */
  DL_MONITOR    = 0x03U,        /*!< Monitor Mode                             */
  DL_STOP       = 0x04U,        /*!< Stop Mode                                */
  DL_BUSY       = 0x05U         /*!< Busy Mode                                */
} DL_Status_t;
/**
  * @}
  */
  
/* Exported functions --------------------------------------------------------*/
/** @addtogroup KNX_DL_Exported_Functions
  * @{
  */

/** @addtogroup KNX_DL_Exported_Functions_Group1
  * @{
  */

/* Initialization functions  **************************************************/
uint8_t KNX_DL_Init(void);
/**
  * @}
  */

/** @addtogroup KNX_DL_Exported_Functions_Group2
  * @{
  */

/* Services functions  ********************************************************/
uint8_t KNX_DL_Data_req(uint8_t Tx_FT, uint8_t Tx_AT, uint16_t Tx_DA, uint8_t Tx_Pri, uint8_t *Tx_LSDU, uint8_t Tx_LG);
uint8_t KNX_DL_Data_rec(uint8_t *Rx_FT, uint8_t *Rx_AT, uint16_t *Rx_SA, uint8_t *Rx_Pri, uint8_t *Rx_LSDU, uint8_t *Rx_LG);
/**
  * @}
  */

/** @addtogroup KNX_DL_Exported_Functions_Group3
  * @{
  */

/* State functions  **********************************************************/
DL_Status_t KNX_DL_GetState(void);
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

#endif /* __KNX_DL */

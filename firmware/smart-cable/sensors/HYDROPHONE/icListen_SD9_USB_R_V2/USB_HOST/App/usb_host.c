/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v2.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_cdc.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
CDC_LineCodingTypeDef FrameFormat;
extern osMessageQId AppliEventHandle;
extern osMessageQId USB_rxHandle;

uint8_t usb_rx_buff[USB_RX_NUM_OF_BUFFERS][USB_RX_BUFF_SIZE];
uint8_t usb_rx_buff_active=0;

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
void USB_transmit_msg(uint8_t* data,uint32_t size){
  USBH_CDC_Stop(&hUsbHostFS);
  USBH_CDC_Transmit(&hUsbHostFS,data, size);
}

void USB_receive_msg(){
  USBH_CDC_Stop(&hUsbHostFS);
  USBH_CDC_Receive(&hUsbHostFS,usb_rx_buff[usb_rx_buff_active],USB_RX_BUFF_SIZE);
}


void USB_set_line_coding(){
	FrameFormat.b.dwDTERate = 1250000;
	FrameFormat.b.bCharFormat = 0;
	FrameFormat.b.bDataBits = 8;
	FrameFormat.b.bParityType = 0;

	USBH_CDC_SetLineCoding(&hUsbHostFS, &FrameFormat);
}

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

void USBH_CDC_TransmitCallback(USBH_HandleTypeDef *phost)
{
  USB_receive_msg();
}


void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost)
{
}

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */


  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_CDC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  osMessagePut(AppliEventHandle, APPLICATION_DISCONNECT, 0);
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
  osMessagePut(AppliEventHandle, APPLICATION_READY, 0);
  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  osMessagePut(AppliEventHandle, APPLICATION_START, 0);
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */


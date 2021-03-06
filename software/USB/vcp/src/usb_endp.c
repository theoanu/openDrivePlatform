/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "buffer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t  USB_Tx_State;

BUFFER(USB_RX, 256);
BUFFER(USB_TX, 256);

BUFFER_COPY(USB_RX, USB_TX);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
// write to USB
void EP1_IN_Callback (void)
{
	//uint16_t USB_Tx_ptr;
	uint16_t USB_TX_Length;
  
	if (USB_Tx_State == 1)
	{
		if(BUFFER_IS_EMPTY(USB_TX))
		{
			USB_Tx_State = 0;
		}
		else
		{
			USB_TX_Length = BUFFER_CONTIGUOUS_DATA_LENGTH(USB_TX);
			if (USB_TX_Length > VIRTUAL_COM_PORT_DATA_SIZE)
				USB_TX_Length = VIRTUAL_COM_PORT_DATA_SIZE;
      
#ifdef USE_STM3210C_EVAL
			USB_SIL_Write(EP1_IN, &USART_Rx_Buffer[USB_Tx_ptr], USB_TX_Length);
#else
			UserToPMABufferCopy(USB_TX_Head, ENDP1_TXADDR, USB_TX_Length);
			BUFFER_MOVE_HEAD(USB_TX, USB_TX_Length);
			SetEPTxCount(ENDP1, USB_TX_Length);
			SetEPTxValid(ENDP1);
#endif  
		}
	}
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  uint32_t USB_RX_DataLength = 0, USB_RX_TotalLength, USB_RX_DataLength2 = 0;

  /* Get the received data buffer and update the counter */
  USB_RX_DataLength = USB_SIL_Read(EP3_OUT, USB_RX_Tail, BUFFER_CONTIGUOUS_FREE_SPACE(USB_RX), &USB_RX_TotalLength, 0);
  BUFFER_MOVE_TAIL(USB_RX, USB_RX_DataLength);
  
  if(USB_RX_TotalLength > USB_RX_DataLength)
  {
	  USB_RX_DataLength2 = USB_SIL_Read(EP3_OUT, USB_RX_Tail, BUFFER_CONTIGUOUS_FREE_SPACE(USB_RX), &USB_RX_TotalLength, USB_RX_DataLength);
	  BUFFER_MOVE_TAIL(USB_RX, USB_RX_DataLength2);
  }

  /* USB data will be immediately processed, this allow next USB traffic being 
  NAKed till the end of the USART Xfer */
  
  // send back to USB
  uint16_t RXLen = BUFFER_DATA_LENGTH(USB_RX);
  uint16_t TXLen = USB_RX_USB_TX_Copy(RXLen);

#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
#endif /* STM32F10X_CL */
  SetEPRxValid(ENDP3);
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#ifdef STM32F10X_CL
void INTR_SOFINTR_Callback(void)
#else
void SOF_Callback(void)
#endif /* STM32F10X_CL */
{
  static uint32_t FrameCount = 0;
  
  if(bDeviceState == CONFIGURED)
  {
    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
    {
      /* Reset the frame counter */
      FrameCount = 0;
      
      /* Check the data to be sent through IN pipe */
      Handle_USBAsynchXfer();
    }
  }  
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


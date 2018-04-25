LAZER-MESSENGER 1.0:
Lazer-Messenger is completely turn-key and and functional on both ends.
`HAL_UART_RxCpltCallback` successfully receives data from the laser diode.
```C
else if (huart->Instance == USART2)  				        //USART2: for message reception
{
    	if (Rx_indx2==0) {for (j=0;j<100;j++) Rx_Buffer2[j]=0;}         //clear Rx_Buffer before receiving
		if (Rx_data2[0]!=10) 					//if data differs ascii 10 (a.k.a. '\n')
			{
			Rx_Buffer2[Rx_indx2++]=Rx_data2[0];    		//add data to Rx_Buffer
			}
		else
			{
			flag_message = 1;		                //transfer complete, data is ready to read
			strcpy(messageReceived, Rx_Buffer2); 	        //copy the string for use outside the function
			Rx_Buffer2[Rx_indx2] = 0; 			//begin clearing buffer
			Rx_indx2=0;
			for (j = 0; j < 100; j++) Rx_Buffer2[j] = 0;    // clear buffer
			}
		HAL_UART_Receive_IT(&huart2, Rx_data2, 1);   		//activate UART receive interrupt
		}
```

## Purdue University ECE 362 Mini-Projec
# Team members include: 
- Bradford Hawthorne
- Kevin Hunckler
- Sahil Bhalla
- Andrew St. Pierre

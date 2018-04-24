## ece362_miniproj
Purdue University ECE 362 Mini-Project. 

# Tuesday, April 24, 2018
Known bugs: receiving UART Rx data post laser transfer
```C
//USART VARIABLES
char Rx_indx1, Rx_data1[2], Rx_Buffer1[100];
char Rx_indx2, Rx_data2[2], Rx_Buffer2[100];
char inputReceived[100];
char messageReceived[100];
int flag_uart;
int new_message;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//Interrupt callback routine
{
    //this callback routine allows the user to parse their data and submit it using a the ENTER key
    //assigns each bit of Rx_data to a char the user input
    uint8_t i;
    uint8_t j;
    if (huart->Instance == USART1)  //current UART
        {
        if (Rx_indx1==0) {for (i=0;i<100;i++) Rx_Buffer1[i]=0;}   //clear Rx_Buffer before receiving new data

        if (Rx_data1[0]!=13) //if received data different from ascii 13 (enter)
            {
            Rx_Buffer1[Rx_indx1++]=Rx_data1[0];    //add data to Rx_Buffer
            }
        else //if received data = 13 (a.k.a. ENTER)
            {
        	flag_uart = 1;//transfer complete, data is ready to read
        	strcpy(inputReceived, Rx_Buffer1);//copy the string for use outside the function
        	Rx_Buffer1[Rx_indx1] = 0;//begin clearing buffer
        	Rx_indx1=0;
			for (i = 0; i < 100; i++) Rx_Buffer1[i] = 0;// clear buffer
            }
        HAL_UART_Receive_IT(&huart1, Rx_data1, 1);   //activate UART receive interrupt every time
        }
    //for the second USART to receive string transmitted over laser
    else if (huart->Instance == USART2)  //current UART
		{
    	if (Rx_indx2==0) {for (j=0;j<100;j++) Rx_Buffer2[j]=0;}   //clear Rx_Buffer before receiving new data

		if (Rx_data2[0]!='\0') //if received data different from ascii 13 (enter)
			{
			Rx_Buffer2[Rx_indx2++]=Rx_data2[0];    //add data to Rx_Buffer
			}
		else
		{
			//new_message = 1;//transfer complete, data is ready to read
			//Rx_Buffer[Rx_indx++]=Rx_data[0];
			strcpy(messageReceived, Rx_Buffer2); //copy the string for use outside the function
			Rx_Buffer2[Rx_indx2] = 0; //begin clearing buffer
			Rx_indx2=0;
			for (j = 0; j < 100; j++) Rx_Buffer2[j] = 0; // clear buffer
			}
		HAL_UART_Receive_IT(&huart2, Rx_data2, 1);   //activate UART receive interrupt every time
		}
}
 ```
  
# Team members include: 
- Bradford Hawthorne
- Kevin Hunckler
- Sahil Bhalla
- Andrew St. Pierre

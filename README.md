## ece362_miniproj
Purdue University ECE 362 Mini-Project. 
# Saturday, April 14, 2018
/* USER CODE BEGIN PFP */
void transmitString(UART_HandleTypeDef *huart, char message[]);
/* USER CODE END PFP */
int main(void)
{

  /* USER CODE BEGIN 2 */
  char txData[30] = "Hello world \r\n";

  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  transmitString(&huart1, txData);
	  HAL_Delay(500);
  }
  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */
void transmitString(UART_HandleTypeDef *huart, char message[])
{
    HAL_UART_Transmit(huart, (uint8_t *)message, strlen(message), 100);
    return;
}
/* USER CODE END 4 */


# Saturday, April 7, 2018
1. welcome message and display animation
    - 'introduction()'
2. method of verifying connection between Tx-Rx
    - infinite while loop
```C
while (1)
{
  //STATE MACHINE
  if (state == 0)//initializing
  {
    introduction();
    state =1;
  }
  if (tim2_flag==1)//interrupt driven
	{
	  tim2_flag=0;//clear flag
    if (count == 7)//about a half second (may need to change this #)
    {
      count = 0;//reset counter to 0
      if (adc_reading <= 2000)//if laser was on photodiode, 2000 was easily broken
      {
        HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_7);//toggle the red led
      }
      else if (adc_reading >= 2000)//photodiode connected to laser
      {
        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);//set red led to ON
      }
    }
    else//increment count
    {
       count = count+1;//increment timer
    }
 }
 }
 }
 ```
  
# Team members include: 
- Bradford Hawthorne
- Kevin Hunckler
- Sahil Bhalla
- Andrew St. Pierre

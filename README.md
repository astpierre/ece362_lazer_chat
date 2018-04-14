## ece362_miniproj
Purdue University ECE 362 Mini-Project. 
# Saturday, April 14, 2018
Testing
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

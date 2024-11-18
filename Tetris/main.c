uint8_t podatki;
uint32_t pozicija_za_char=0;
uint32_t pozicija_za_konec=0;
char vnos[100];
int main(void)
{
  /* USER CODE BEGIN 1 */


	  /* Enable the CPU Cache */
	  CPU_CACHE_Enable();

	  /* STM32H7xx HAL library initialization:
	       - Configure the Systick to generate an interrupt each 1 msec
	       - Set NVIC Group Priority to 4
	       - Low Level Initialization
	     */
	  HAL_Init();

	  /* Configure the system clock to 400 MHz */
	  SystemClock_Config();

	  /* Configure LED1 */
	  BSP_LED_Init(LED1);

	  /*##-1- LCD Initialization #################################################*/
	  /* Initialize the LCD */
	  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
	  UTIL_LCD_SetFuncDriver(&LCD_Driver);

	  /* Set Foreground Layer */
	  UTIL_LCD_SetLayer(0);

	  /* Clear the LCD Background layer */
	  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
	  uint32_t x_size, y_size;

	  BSP_LCD_GetXSize(0, &x_size);
	  BSP_LCD_GetYSize(0, &y_size);
	  hTSs->Width = x_size;
	  hTSs->Height = y_size;
	  hTSs->Orientation = TS_SWAP_XY;
	  hTSs->Accuracy = 5;
	  /* Touchscreen initialization */
	  BSP_TS_Init(0, hTSs);

	  srand(time(NULL));   // Initialization, should only be called once.
	  HAL_IncTick();

	  USART3_Init(&huart3);
	  //HAL_UART_Receive_IT(&huart3, &podatki, 1);

	  /* Infinite loop */
	  while (1)
	  {
		  char znak='\0';
		  char znak_temp;

		  static uint8_t square_dim=10;
		  static uint32_t x_pos=10;
		  static uint32_t y_pos=10;
		  static uint8_t rot=1;
		  static uint8_t fall_speed=500;
		  static uint8_t fall_acceleration=1;
		  static uint8_t LR_step=10;
		  static uint8_t change_flag=0;

		  static uint32_t stevec=0;
		  	while(znak=='\0' && stevec<1000)
		  	{
		  	if((USART3->ISR)&(1<<5))
		  	  {znak_temp=USART3->RDR;
		  	  if(znak_temp=='a' || znak_temp=='s' || znak_temp=='d' || znak_temp=='r')
		  	  {
		  		  znak=znak_temp;
		  	  }
		  	  }
		  	stevec++;
		    }
		  stevec=0;


		  	if(znak=='r')
		  	{
		  		if((rot==0||rot==2)&&(y_pos>=(y_size-4*square_dim)))y_pos-=LR_step;
		  		rot++;
		  		if(rot>3)rot=0;
		  		znak='\0';
		  		change_flag=1;
		  	}
		  	else if(znak=='d')
		  	{
		  		if(y_pos>=10)
		  		{
		  			y_pos-=LR_step;
		  			change_flag=1;
		  		}
		  	}
		  	else if(znak=='a')
		  	{
		  		if(y_pos<(y_size-4*square_dim*(rot==1||rot==3)-3*square_dim*(rot==0||rot==2)))
		  		{
		  			y_pos+=LR_step;
		  			change_flag=1;
		  		}
		  	}
		  	if(change_flag==1)
	  		{
		  	UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
	  		block_draw(0,UTIL_LCD_COLOR_LIGHTGREEN,x_pos,y_pos,rot);
	  		change_flag=0;
	  		}
		  	/*
			pozicija_za_konec=pozicija_za_char;
			pozicija_za_char=0;
			USART2->TDR='\r';
			while(!((USART3->ISR)&(1<<6)));
			USART2->TDR='\n';


		//ECHO
		for(int i=0;i<pozicija_za_konec;i++)
		{
	      USART3->TDR=vnos[i];
	      while(!((USART3->ISR)&(1<<6)));
		}
		USART3->TDR='\r';
		while(!((USART3->ISR)&(1<<6)));
		USART3->TDR='\n';*/
	  }
}

uint8_t podatki;
uint32_t pozicija_za_char=0;
uint32_t pozicija_za_konec=0;
char vnos[100];
uint32_t y_size_global;
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

	  y_size_global=y_size;
	  /* Touchscreen initialization */
	  BSP_TS_Init(0, hTSs);

	  srand(time(NULL));   // Initialization, should only be called once.
	  HAL_IncTick();

	  USART3_Init(&huart3);
	  //HAL_UART_Receive_IT(&huart3, &podatki, 1);
	  TETRIS_SCORE_SETUP();
	  TETRIS_SCORE_DISPLAY(123);
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



/* UART2 Interrupt Service Routine */
//void USART3_IRQHandler(void)
//{
//  HAL_UART_IRQHandler(&huart3);
//}

void TETRIS_SCORE_SETUP()
{
	const uint8_t pixel_size=3;
	const uint8_t x_offset=10;
	const uint8_t y_offset=10;

	uint8_t score_napis[5][9][8]={
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,0,0,0},
	 {0,0,1,1,1,0,0,0},
	 {0,0,0,1,1,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0}
	},
	{{0,0,1,1,1,1,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,0,1,1,1,1,1,0},
	},
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0},
	},
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,1,0,0,0},
	 {0,1,0,0,1,1,0,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0}
	},
	{{0,0,1,1,1,1,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,0,1,1,1,1,1,0}
	}
	};

	UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

	for(uint32_t crta=0;crta<y_size_global;crta++)
	{
		UTIL_LCD_SetPixel(pixel_size*8+20,crta,UTIL_LCD_COLOR_RED);
		UTIL_LCD_SetPixel(pixel_size*8+21,crta,UTIL_LCD_COLOR_RED);
	}

	for(uint32_t score=y_size_global-y_offset;score>(y_size_global-y_offset-5*8*pixel_size);score-=pixel_size)
	{
	  for(uint8_t score_x=0;score_x<9;score_x++)
	  {
		for(uint8_t y_temp=0;y_temp<pixel_size;y_temp++)
		{
			for(uint8_t x_temp=0;x_temp<pixel_size;x_temp++)
			{
				if(score_napis[((y_size_global-y_offset)-score)/pixel_size/8][score_x][((y_size_global-y_offset)-score)/pixel_size-((y_size_global-y_offset)-score)/pixel_size/8*8]==1)UTIL_LCD_SetPixel(x_offset+score_x*pixel_size+x_temp,score-y_temp,UTIL_LCD_COLOR_RED);

			}
		}
	  }
	}
}


uint32_t potenca(uint32_t stevilka,uint32_t POTENCA)
{
	if(POTENCA==0)
	return 1;
	else
	{
	for(uint32_t ponovitev=0;ponovitev<(POTENCA-1);ponovitev++)
	{
		stevilka*=stevilka;
	}
	}
	return stevilka;

	}

void TETRIS_SCORE_DISPLAY(uint32_t score)
{
	const uint32_t score_start_y=145;
	const uint8_t pixel_size=3;
	const uint8_t x_offset=15;
	const uint8_t y_offset=10;

	uint8_t score_cifre[10][9][8]={
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,1,1,1,0},
	 {0,1,0,0,1,0,1,0},
	 {0,1,0,1,1,0,1,0},
	 {0,1,0,1,0,0,1,0},
	 {0,1,1,1,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0}
	},
	{{0,0,0,1,1,0,0,0},
	 {0,0,1,1,1,0,0,0},
	 {0,1,1,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,1,1,1,1,1,1,0},
	},
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,0,0,0,1,1,0},
	 {0,0,0,1,1,1,0,0},
	 {0,0,1,1,0,0,0,0},
	 {0,1,1,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,1,1,1,1,0},
	},
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,0,0,0,1,1,0},
	 {0,0,0,0,1,1,0,0},
	 {0,0,0,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0},
	},
	{{0,0,0,0,1,1,0,0},
	 {0,0,0,1,1,0,0,0},
	 {0,0,1,1,0,0,0,0},
	 {0,1,1,0,0,0,0,0},
	 {0,1,0,0,1,0,0,0},
	 {0,1,1,1,1,1,1,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0}
	},
	{{0,1,1,1,1,1,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,1,1,1,0,0},
	 {0,0,0,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0}
	},
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0}
	},
	{{0,1,1,1,1,1,1,0},
	 {0,0,0,0,0,0,1,0},
	 {0,0,0,0,0,0,1,0},
	 {0,0,0,0,0,1,1,0},
	 {0,0,0,0,1,1,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,1,1,0,0,0},
	 {0,0,0,1,0,0,0,0},
	 {0,0,0,1,0,0,0,0}
	},
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0}
	},
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0},
	 {0,0,0,0,0,0,1,0},
	 {0,0,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0}
	},
	};

	uint32_t st_stevk=1;
	uint32_t cifra_copy=score;

	while(cifra_copy>9)
	{
			cifra_copy/=10;
			st_stevk++;
	}


	//prvi for (score) je čudn in tretji naslov/argument arraya
	cifra_copy=score;
	uint32_t stevke=st_stevk; //kopija da for loop normalno tece
	for(uint32_t score=score_start_y-y_offset;score>(score_start_y-y_offset-st_stevk*8*pixel_size);score-=pixel_size)
	{
	  for(uint8_t score_x=0;score_x<9;score_x++)
	  {
		for(uint8_t y_temp=0;y_temp<pixel_size;y_temp++)
		{
			for(uint8_t x_temp=0;x_temp<pixel_size;x_temp++)
			{
				if(score_cifre[cifra_copy/potenca(10,(stevke-1))][score_x][((score_start_y-y_offset)-score)/pixel_size-((score_start_y-y_offset)-score)/pixel_size/8*8]==1)UTIL_LCD_SetPixel(x_offset+score_x*pixel_size+x_temp,score-y_temp,UTIL_LCD_COLOR_BLUE);
			}
		}
	  }
	  cifra_copy%=potenca(10,(stevke-1));
	  stevke--;
	}

}

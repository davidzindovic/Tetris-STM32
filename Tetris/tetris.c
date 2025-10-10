
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tetris.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SQUARE_X_DIM 10
#define SQUARE_Y_DIM 10
#define SQUARE_DIM 10

#define FILL

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//seznam oblik blockov, pri čemer prve tri vrednosti predstavljajo prvo vrstio, zadnje tri vrednosti pa drugo vrstico
static const bool blocks[5][6]={{0,1,0,1,1,1},{0,0,1,1,1,1},{0,0,0,1,1,1},{1,0,0,1,1,1},{0,1,1,0,1,1}};

//seznam možnih barv blockov
static const uint32_t COLORS[10]={UTIL_LCD_COLOR_ORANGE,UTIL_LCD_COLOR_RED,UTIL_LCD_COLOR_GREEN,
								UTIL_LCD_COLOR_BLUE,UTIL_LCD_COLOR_BLACK,UTIL_LCD_COLOR_CYAN,
								UTIL_LCD_COLOR_BROWN,UTIL_LCD_COLOR_MAGENTA,UTIL_LCD_COLOR_DARKGRAY,
								UTIL_LCD_COLOR_DARKYELLOW};

//shrani stanja vseh lokacij "parkiranih" blokov. Blok ima zgoraj desno prijemališče, tako da shrani te koordinate.
static bool game_state[(480-50)/10][(272)/10];

//hrani indexe barv, ki jih kličeš iz COLORS nabora, za "parkirane" blocke
static uint8_t color_game_state[(480-50)/10][(272)/10];

static bool first_set_game_state=0;
extern cbc[7];

/* Private function prototypes -----------------------------------------------*/
void block_draw(uint8_t block_pick, uint32_t block_color, uint32_t block_x_pos, uint32_t block_y_pos, uint8_t rotation, uint8_t *cbc);
void *check_block_collision(uint32_t x, uint32_t y, uint8_t block_shape, uint8_t block_rot,uint32_t color,uint8_t *cbc);
void erase_moved_part(uint8_t block, uint32_t x, uint32_t y, uint32_t old_x, uint32_t old_y, uint8_t rot, uint8_t old_rot,uint8_t *cbc);
void TETRIS_SCORE_DISPLAY(uint32_t rezultat);
uint32_t potenca(uint32_t stevilka,uint32_t POTENCA);
void TETRIS_SCORE_SETUP(void);
void GAME_OVER(void);
void START_GAME(void);
void game_state_init(void);

uint64_t LINE_FILL_CHECK();
void LINE_DELETE(uint64_t line_delete_mask);
void LINE_DOWN_SHIFT(uint64_t gap_mask);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Block Draw - Nariše izbran block v izbrani barvi ter izbrani orientaciji na določeni poziciji ob upoštevanju ovir okoli podane pozicije
  * @param  block_pick: številka oblike blocka, block_color: številka barve blocka, block_x_pos: x koordinata objekta, block_y_pos: y koordinata objekta, rotation: številka za orientiranost, cbc: vektor ovir objekta
  * @retval None
  */
void block_draw(uint8_t block_pick, uint32_t block_color, uint32_t block_x_pos, uint32_t block_y_pos, uint8_t rotation, uint8_t *cbc)
{

//v primeru da je številka orientacije večja od maksimuma, jo znižamo
if(rotation>3)
{
	rotation=rotation-rotation/3*3;
}

//narišemo block v odvisnosti od orientacije (3x2)
if(rotation==0 || rotation==2)
{
  for(uint8_t x=0;x<3;x++)
	{
	  for(uint8_t y=0;y<2;y++)
		{
		  if(blocks[block_pick][((1-y)*3+x)*(rotation==0) + (y*3+(2-x))*(rotation==2)])
		  {
			  //preden nariše block preveri če so kje ovire (da ne riše čez že narisane blocke)
			  //primerja konkreten block v cbc z konkretno masko
			  uint8_t ovira_L=(cbc[x*2+y]&0b100)>0; //je 1 če je ovira LEFT
			  uint8_t ovira_R=(cbc[x*2+y]&0b001)>0; //je 1 če je ovira RIGHT
			  uint8_t ovira_D=(cbc[x*2+y]&0b010)>0; //je 1 če je ovira DOWN

			for(uint8_t block_x_dim=0;block_x_dim<SQUARE_X_DIM;block_x_dim++)
			  {
				if(ovira_R!=1)UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+(1-y)*SQUARE_Y_DIM,COLORS[block_color]);
				if(ovira_L!=1)UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+(1-y)*SQUARE_Y_DIM+SQUARE_Y_DIM,COLORS[block_color]);
			  }
			for(uint8_t block_y_dim=0;block_y_dim<SQUARE_Y_DIM;block_y_dim++)
			  {
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM,block_y_dim+block_y_pos+(1-y)*SQUARE_Y_DIM,COLORS[block_color]);
				if(ovira_D!=1)UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+SQUARE_X_DIM,block_y_dim+block_y_pos+(1-y)*SQUARE_Y_DIM,COLORS[block_color]);
			  }

			//zapolni narisan block - pusti levo in dol od polnila prazen pixel za vizuelni učinek
			//polnilo onemogočimo tako, da zakomentiramo vrstico #define FILL
			#ifdef FILL

			UTIL_LCD_FillRect(block_x_pos+1+x*SQUARE_X_DIM,block_y_pos+(1-y)*SQUARE_Y_DIM+1,8,8,COLORS[block_color]);

			#endif

		  }
		}
	}
}

//narišemo block v odvisnosti od orientacije (2x3)
else if (rotation==1 || rotation==3)
{
	for(uint8_t x=0;x<2;x++)
	{
	  for(uint8_t y=0;y<3;y++)
		{
		  if(blocks[block_pick][(x*3+y)*(rotation==1) + ((1-x)*3+(2-y))*(rotation==3)])
		  {
			  //preden nariše block preveri če so kje ovire (da ne riše čez že narisane blocke)
			  //primerja konkreten block v cbc
			  uint8_t ovira_L=(cbc[y+3*x]&0b100)>0; //je 1 če je ovira LEFT
			  uint8_t ovira_R=(cbc[y+3*x]&0b001)>0; //je 1 če je ovira RIGHT
			  uint8_t ovira_D=(cbc[y+3*x]&0b010)>0;	//je 1 če je ovira DOWN

			for(uint8_t block_x_dim=0;block_x_dim<SQUARE_X_DIM;block_x_dim++)
			  {
				if(ovira_R!=1)UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+(2-y)*SQUARE_Y_DIM,COLORS[block_color]); //če ni ničesar na desno
				if((ovira_L!=1)||(block_y_pos==240))UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+(2-y)*SQUARE_Y_DIM+SQUARE_Y_DIM,COLORS[block_color]);
			  }
			for(uint8_t block_y_dim=0;block_y_dim<SQUARE_Y_DIM;block_y_dim++)
			  {
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM,block_y_dim+block_y_pos+(2-y)*SQUARE_Y_DIM,COLORS[block_color]);
				if(ovira_D!=1)UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+SQUARE_X_DIM,block_y_dim+block_y_pos+(2-y)*SQUARE_Y_DIM,COLORS[block_color]);
			  }

		//zapolni narisan block - pusti levo in dol od polnila prazen pixel za vizuelni učinek
		//polnilo onemogočimo tako, da zakomentiramo vrstico #define FILL
		#ifdef FILL

		UTIL_LCD_FillRect(block_x_pos+1+x*SQUARE_X_DIM,block_y_pos+(2-y)*SQUARE_Y_DIM+1,8,8,COLORS[block_color]);

		#endif
		  }
		}
	}
}

}

/**
  * @brief Game State Init - inicializira spomin za igro
  * @param  None
  * @retval None
  */

void game_state_init(void)
{
	uint16_t y_size=272; //max y koordinata

	//na koliko polj v vsako smer je razdeljen spomin
	const uint8_t game_x_num_blocks=(480-50)/10;
	const uint8_t game_y_num_blocks=27;

	const uint8_t square_dim=10;

	if(first_set_game_state==0)
	{
		for(uint8_t i=0;i<game_x_num_blocks;i++)
		{
			for(uint8_t j=0;j<game_y_num_blocks;j++)
			{
				game_state[i][j]=0;			//nastavimo cel spomin igre na 0, ker nikjer ni blokov
				color_game_state[i][j]=100; //nastavimo cel spomin barv igre na 100, ker nikjer ni blokov
			}
		}
	first_set_game_state=1;				    //označimo, da smo opravili prvi inicializacijo igre
	}

	for(uint8_t cbc_set=0;cbc_set<7;cbc_set++)cbc[cbc_set]=0; //ponastavimo vektor ovir objekta
}

/**
  * @brief Check Block Collision
  * @param  x: x koordinata za postavitev ki bi jo radi preverili,y: y koordinata za postavitev ki bi jo radi preverili, block_shape: številka oblike blocka, block_rot: številka rotacije blocka, color: barva blocka, cbc: vektor celic kamor zapiše funkcija stanje posamezne celice v smislu ovir
  * @retval None
  */

void * check_block_collision(uint32_t x, uint32_t y, uint8_t block_shape, uint8_t block_rot,uint32_t color,uint8_t *cbc)
{
	uint8_t collision_array[7]; //array sedmih elementov, in sicer collision state za vsak block (skrajno desni 3 biti vsake pozicije)
								//7. podatek je movement collision, da vem v katero smer se ne sme premikati (stanje za celoten block)

	uint8_t collision_flag=0;
	uint16_t y_size=272;

	//dimenzije matrike spomina igre:
	const uint8_t game_x_num_blocks=(480-50)/10;
	const uint8_t game_y_num_blocks=(y_size-10)/10;

	const uint8_t square_dim=10;

	for(uint8_t array_reset=0;array_reset<7;array_reset++)collision_array[array_reset]=0; //ponastavimo array ovir

	uint8_t collision_flag_temp=0;

	//glede na orientacijo pogledamo za vsako celico v spomin igre, če se nahaja "parkirana" ovira levo, pod ali desno od celice
	//upoštevamo tudi skrajno levo in skrajno desno lego ter skrajno/najnižjo lego

	if(block_rot==1||block_rot==3)
	{	//sprehodimo se po vsaki celici blocka (2x3)
		//gledamo zaporedno od indexov 0 (zgoraj levo) do 6 (spodaj desno), upoštevamo seveda zarotiranost

		for(uint8_t block_y=0;block_y<2;block_y++)
		{
			for(uint8_t block_x=0;block_x<3;block_x++)
			{
				uint32_t koordinata_block=(block_y*3+block_x)*(block_rot==1)+((2-block_x)+(1-block_y)*3)*(block_rot==3); //0-5, informacija glede gledane celice
				bool block_state=blocks[block_shape][koordinata_block]; //pogledamo če je tam sploh kaj v blocku, da moramo biti pozorni na okolico
				if(block_state==1)
				{
					//sprehodimo se levo, dol in desno po vsaki celici blocka
					for(uint8_t blockLDR=0;blockLDR<3;blockLDR++)
					{
							uint32_t koordinata_x=x+block_y*square_dim+square_dim*(blockLDR==1); //x lokalno (dol po ekranu)
							uint32_t koordinata_y=y+(2-block_x)*square_dim+square_dim*(blockLDR==0); //y lokalno (levo po ekranu)
							if(koordinata_y!=0)koordinata_y-=square_dim*(blockLDR==2);

							//preverimo da bomo gledali znotraj meja
							if((koordinata_y==260)&&(blockLDR==1))collision_flag_temp|=0b100;
							if((koordinata_y==0)&&(blockLDR==1))collision_flag_temp|=0b001;

							//za dol dodan poseben pogoj ker nekatere oblike imajo blind kocke
							if((x>=(480-(1*(blocks[block_shape][(3+block_x)*(block_rot==1)+(2-block_x)*(block_rot==3)]==0)+2*(blocks[block_shape][(3+block_x)*(block_rot==1)+(2-block_x)*(block_rot==3)]==1))*square_dim)))collision_flag_temp|=0b010;

							//preverimo če za trenuten primer LRD ne smemo pogledati, torej če ne smemo pogledati npr. levo (ker bi pogledali iz array bound-ov)
							if(!(((blockLDR==0)&&(collision_flag_temp&0b100))||((blockLDR==1)&&(collision_flag_temp&0b010))||((blockLDR==2)&&(collision_flag_temp&0b001))))
							{
								if(game_state[(koordinata_x-50)/square_dim][koordinata_y/square_dim]==1)//če je v game state na izračunani koordinati, ki jo preverjamo, prisoten del objekta, to zapišemo v array
									{
										collision_array[block_y*3+block_x]|=(1<<(2-blockLDR));
									}
							}
					}
				}
				else collision_flag_temp=0;

					collision_array[block_y*3+block_x]|=collision_flag_temp;
					collision_flag_temp=0;
			}
		}
	}
	else if(block_rot==2||block_rot==0)
	{	//sprehodimo se po vsaki celici blocka (3x2)
		//gledamo zaporedno od indexov 0 (zgoraj levo) do 6 (spodaj desno), upoštevamo seveda zarotiranost

		for(uint8_t block_y=0;block_y<3;block_y++)
		{
			for(uint8_t block_x=0;block_x<2;block_x++)
			{
				uint32_t koordinata_block=((2-block_y)+block_x*3)*(block_rot==2)+(block_y+(1-block_x)*3)*(block_rot==0); //0-5, samo informacija glede oblike blocka
				bool block_state=blocks[block_shape][koordinata_block]; //pogledamo če je tam sploh kaj v blocku, da moramo biti pozorni na okolico
				if(block_state==1)
				{
					//sprehodimo se levo, dol in deesno po vsakem blocku
					for(uint8_t blockLDR=0;blockLDR<3;blockLDR++)
					{
							uint32_t koordinata_x=x+block_y*square_dim+square_dim*(blockLDR==1); //x lokalno (dol po ekranu); dodana izjema, da pogleda pod predzadnjo vrstico zarotirane kocke
							uint32_t koordinata_y=y+(1-block_x)*square_dim+square_dim*(blockLDR==0); //y lokalno (levo po ekranu)
							if(koordinata_y!=0)koordinata_y-=square_dim*(blockLDR==2);

							//preverimo da bomo gledali znotraj meja
							if((koordinata_y==260)&&(blockLDR==1))collision_flag_temp|=0b100;
							if((koordinata_y==0)&&(blockLDR==1))collision_flag_temp|=0b001;

							//za dol dodan poseben pogoj ker nekatere oblike imajo blind kocke
							if(block_y<2)if(((x+block_y*square_dim)>=(480-(1*(blocks[block_shape][((2-block_y)+3*block_x-1)*(block_rot==2)+((3+block_y)-3*block_x+1)*(block_rot==0)]==0)+(/*3-block_y*/2)*(blocks[block_shape][((2-block_y)+3*block_x-1)*(block_rot==2)+((3+block_y)-3*block_x+1)*(block_rot==0)]==1))*square_dim)))collision_flag_temp|=0b010;


							//preverimo če za trenuten primer LRD ne smemo pogledati, torej če ne smemo pogledati npr. levo (ker bi pogledali iz array bound-ov)
							if(!(((blockLDR==0)&&(collision_flag_temp&0b100))||((blockLDR==1)&&(collision_flag_temp&0b010))||((blockLDR==2)&&(collision_flag_temp&0b001))))
							{
								if(game_state[(koordinata_x-50)/square_dim][koordinata_y/square_dim]==1)//če je v game state na izračunani koordinati, ki jo preverjamo, prisoten del objekta, to zapišemo v array
									{
										collision_array[block_y*2+block_x]|=(1<<(2-blockLDR));
									}
							}
					}
				}
				else collision_flag_temp=0;

					collision_array[block_y*2+block_x]|=collision_flag_temp;
					collision_flag_temp=0;
			}
		}
	}

	//collision podatek za premik (če katerikoli square ne sme iti v kero smer, potem cel objekt tudi ne sme
	collision_array[6]=0;

	for(uint8_t neki=0;neki<6;neki++)
	{
		collision_array[6]|=collision_array[neki];
		cbc[neki]=collision_array[neki]; //prekopiramo stanje za posamezno celico blocka v globalen array
	}

	cbc[6]=collision_array[6]; //prekopiramo stanje za celoten block v globalen array
	collision_flag=collision_array[6];

	//če ugotovimo, da ne sme block iti več dol, ga štejemo kot "parkiranega" in ga zapišemo v game_state (spomin) skupaj z njegovo barvo
	//pri tem upoštevamo orientacijo (2x3) ali (3x2)
	if(collision_flag&0b010)
	{
		if(block_rot==1||block_rot==3)
		{
			for(uint8_t block_y_dim=0;block_y_dim<2;block_y_dim++)
			{
				for(uint8_t block_x_dim=0;block_x_dim<3;block_x_dim++)
				{
					if(blocks[block_shape][(block_x_dim+block_y_dim*3)*(block_rot==1)+((2-block_x_dim)+(1-block_y_dim)*3)*(block_rot==3)]==1)
					{
						game_state[(x-50)/10+block_y_dim][(y)/10+(2-block_x_dim)]=1;
						color_game_state[(x-50)/10+block_y_dim][(y)/10+(2-block_x_dim)]=color;
					}
				}
			}
		}

		if(block_rot==0||block_rot==2)
		{
			for(uint8_t block_y_dim=0;block_y_dim<3;block_y_dim++)
			{
				for(uint8_t block_x_dim=0;block_x_dim<2;block_x_dim++)
				{
					if(blocks[block_shape][(block_x_dim*3+(2-block_y_dim))*(block_rot==2)+((1-block_x_dim)*3+block_y_dim)*(block_rot==0)]==1)
					{
						game_state[(x-50)/10+block_y_dim][(y)/10+(1-block_x_dim)]=1;
						color_game_state[(x-50)/10+block_y_dim][(y)/10+(1-block_x_dim)]=color;
					}
				}
			}
		}
	}
}

/**
  * @brief potenca - vrne potencirano številko
  * @param  stevilka: osnova potence, POTENCA: eksponent (nenegativno število)
  * @retval potencirana številka
  */

uint32_t potenca(uint32_t stevilka,uint32_t POTENCA)
{
	uint32_t potencirana_cifra=stevilka;

	if(POTENCA==0)return 1;

	else
	for(uint32_t ponovitev=0;ponovitev<(POTENCA-1);ponovitev++)
	{
		potencirana_cifra=potencirana_cifra*stevilka;
	}

	return potencirana_cifra;

}

/**
  * @brief Tetris Score Display - izpiše trenuten highscore
  * @param  rezultat: številka, ki predstavlja highscore
  * @retval None
  */

void TETRIS_SCORE_DISPLAY(uint32_t rezultat)
{
	const uint32_t score_start_y=160;
	const uint8_t pixel_size=3;
	const uint8_t x_offset=10;
	const uint8_t y_offset=15;

	const uint8_t score_cifre[10][9][8]={
	//0
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
	//1
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
	//2
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
	//3
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
	//4
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
	//5
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
	//6
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
	//7
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
	//8
	{{0,0,1,1,1,1,0,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0}
	},
	//9
	{{0,0,1,1,1,1,0,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0},
	 {0,0,0,0,0,0,1,0},
	 {0,0,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,0,1,1,1,1,0,0}
	}
	};

	int32_t st_stevk=1;
	uint32_t cifra_copy=rezultat; //naredimo lokalno kopijo highscore-a

	//izbriše polje s prejšnjim highscore-om
	UTIL_LCD_FillRect(0, 0, 44, score_start_y-y_offset, UTIL_LCD_COLOR_WHITE);

	//ugotovi število števk števila
	while(cifra_copy>9)
	{
			cifra_copy/=10;
			st_stevk++;
	}

	cifra_copy=rezultat;
	uint32_t stevke=st_stevk; //kopija da for loop normalno tece
	uint32_t stolpci_cifre=0;

	//izpišemo števke števila eno po eno (od leve proti desni) in sicer po stolpcih od zgoraj navzdol
	for(uint32_t y_pos=score_start_y-y_offset;y_pos>(score_start_y-y_offset-st_stevk*8*pixel_size);y_pos-=pixel_size)
	{
	  for(uint8_t score_x=0;score_x<9;score_x++)
	  {
		for(uint8_t y_temp=0;y_temp<pixel_size;y_temp++)
		{
			for(uint8_t x_temp=0;x_temp<pixel_size;x_temp++)
			{
				uint32_t index_1=cifra_copy/potenca(10,(stevke-1));
				uint32_t index_2=score_x;
				uint32_t index_3=((score_start_y-y_offset)-y_pos)/pixel_size-((score_start_y-y_offset)-y_pos)/pixel_size/8*8;
				uint32_t x_draw=x_offset+score_x*pixel_size+x_temp;
				uint32_t y_draw=y_pos-y_temp;
				if(score_cifre[index_1][score_x][index_3]==1)UTIL_LCD_SetPixel(x_draw,y_draw,UTIL_LCD_COLOR_BLUE);
			}
		}
	  }
	  stolpci_cifre++;

	  //ko končamo z izpisom ene števke preklopimo na novo števko
	  //(spremenimo index1 posledično, ki gleda array vseh številk za izpis)
	  if(stolpci_cifre==8)
	  {
	  cifra_copy=cifra_copy%potenca(10,(stevke-1));
	  stevke=stevke-1;
	  stolpci_cifre=0;
	  }
	}

}

/**
  * @brief Tetris Score Setup - izpiše "SCORE" in nariše črto pod napisom
  * @param  None
  * @retval None
  */

void TETRIS_SCORE_SETUP()
{
	const uint8_t pixel_size=3;
	const uint8_t x_offset=10;
	const uint8_t y_offset=5;
	const uint32_t y_size_global=272;

	uint8_t score_napis[5][9][8]={
	//S
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
	//C
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
	//O
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
	//R
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
	//E
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

	UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE); //pobriše celoten ekran z belo barvo

	//narišemo dva piksla debelo črto
	for(uint32_t crta=0;crta<y_size_global;crta++)
	{
		UTIL_LCD_SetPixel(pixel_size*8+20,crta,UTIL_LCD_COLOR_RED);
		UTIL_LCD_SetPixel(pixel_size*8+21,crta,UTIL_LCD_COLOR_RED);
	}

	//izpišemo "SCORE"
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


/**
  * @brief Erase Moved Part - izbriše block, ki smo ga premaknilis
  * @param  block: oblika blocka, x: trenutna x koordinata, y: trenutna y koordinata, old_x: prejšnja x koordinata, old_y: prejšnja y koordinata, rot: rotacija (0-3), old_ rot: prejšnja rotacija, cbc: array stanja ovir za vsako celico blocka oz. za cel block
  * @retval None
  */

void erase_moved_part(uint8_t block, uint32_t x, uint32_t y, uint32_t old_x, uint32_t old_y, uint8_t rot, uint8_t old_rot,uint8_t *cbc)
{
  	uint32_t square_dim=10;

  	if(rot!=old_rot) // v primeru spremembe rotacije brišemo 2x3 oz. 3x2 v enem kosu in ne eno po eno celico
  	{
  		if(old_rot==1 || old_rot==3)
  		{
  			UTIL_LCD_FillRect(old_x, old_y, square_dim*2+1, square_dim*3+1, UTIL_LCD_COLOR_WHITE);
  		}
  		else
  		{
  			UTIL_LCD_FillRect(old_x, old_y, square_dim*3+1, square_dim*2+1, UTIL_LCD_COLOR_WHITE);
  		}
  	}

  	//zbriše primerne dele bloka če je padel dol, pri tem upošteva orientaciji 1 in 3 (2x3) oz. 0 in 2 (3x2)
  	if(x>old_x)
  	{
  	if(rot==1||rot==3)
  	{
		for(uint32_t lik_y=0;lik_y<(2);lik_y++)
		{
			for(uint32_t lik_x=0;(lik_x<(3));lik_x++)
			{
				if((blocks[block][(lik_x+3*lik_y)*(rot==1)+((2-lik_x)+(1-lik_y)*3)*(rot==3)]==1)) //pogleda če je v celici "1" -> torej če je tam sploh kaj narisano
				{
						UTIL_LCD_FillRect(old_x+lik_y*square_dim, old_y+((cbc[lik_y*3+lik_x]&0b001)>0)*(old_y>0)+(2-lik_x)*square_dim, square_dim-((cbc[lik_y*3+lik_x]&0b010)>0)+1, square_dim-((cbc[lik_y*3+lik_x]&0b001)>0)*(old_y>0)-((cbc[lik_y*3+lik_x]&0b100)>0)*(old_y<240)+1, UTIL_LCD_COLOR_WHITE);
					}
			}
		}
  	}
  	if(rot==0||rot==2)
  	{
  	  	for(uint32_t lik_y=0;lik_y<(3);lik_y++)
  	  	{
  	  		for(uint32_t lik_x=0;(lik_x<(2));lik_x++)
  	  		{
  					if(blocks[block][(lik_y+3*(1-lik_x))*(rot==0)+((lik_x)*3+(2-lik_y))*(rot==2)]==1) //pogleda če je v celici "1" -> torej če je tam sploh kaj narisano
  					{
  						UTIL_LCD_FillRect(old_x+lik_y*square_dim, old_y+((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)+(1-lik_x)*square_dim, square_dim-((cbc[lik_y*2+lik_x]&0b010)>0)+1, square_dim-((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)-((cbc[lik_y*2+lik_x]&0b100)>0)*(old_y<240)+1, UTIL_LCD_COLOR_ORANGE);
  						UTIL_LCD_FillRect(old_x+lik_y*square_dim, old_y+((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)+(1-lik_x)*square_dim, square_dim-((cbc[lik_y*2+lik_x]&0b010)>0)+1, square_dim-((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)-((cbc[lik_y*2+lik_x]&0b100)>0)*(old_y<240)+1, UTIL_LCD_COLOR_WHITE);
  					}
  	  		}
  	  	}
  	}
  	}

  	if(y>old_y)//če premaknemo v levo, pri tem upošteva orientaciji 1 in 3 (2x3) oz. 0 in 2 (3x2)
  	{
  	if(rot==1||rot==3)
  	{
		for(uint8_t lik_y=0;lik_y<(2);lik_y++)
		{
			for(uint8_t lik_x=0;lik_x<(3);lik_x++)
			{
					if((blocks[block][(lik_x+3*lik_y)*(rot==1)+((2-lik_x)+(1-lik_y)*3)*(rot==3)]==1)) //pogleda če je v celici "1" -> torej če je tam sploh kaj narisano
					{
						UTIL_LCD_FillRect(old_x+lik_y*square_dim, old_y+((cbc[lik_y*3+lik_x]&0b001)>0)*(old_y>0)+(2-lik_x)*square_dim, square_dim-((cbc[lik_y*3+lik_x]&0b010)>0)+1, square_dim-((cbc[lik_y*3+lik_x]&0b001)>0)*(old_y>0)-((cbc[lik_y*3+lik_x]&0b100)>0)*(old_y<240)+1, UTIL_LCD_COLOR_WHITE);
					}
			}
		}
  	}
  	if(rot==0||rot==2)
  	{
  	  	for(uint8_t lik_y=0;lik_y<(3);lik_y++)
  	  	{
  	  		for(uint8_t lik_x=0;lik_x<(2);lik_x++)
  	  		{
  					if(blocks[block][(lik_y+3*(1-lik_x))*(rot==0)+(lik_x*3+(2-lik_y))*(rot==2)]==1) //pogleda če je v celici "1" -> torej če je tam sploh kaj narisano
  					{
  						UTIL_LCD_FillRect(old_x+lik_y*square_dim, old_y+((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)+(1-lik_x)*square_dim, square_dim-((cbc[lik_y*2+lik_x]&0b010)>0)+1, square_dim-((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)-((cbc[lik_y*2+lik_x]&0b100)>0)*(old_y<240)+1, UTIL_LCD_COLOR_WHITE);
  					}
  	  		}
  	  	}
  	}
  	}

  	if(y<old_y)//če premarknemo v desno, pri tem upošteva orientaciji 1 in 3 (2x3) oz. 0 in 2 (3x2)
  	{
  	  	if(rot==1||rot==3)
  	  	{
  			for(uint8_t lik_y=0;lik_y<(2);lik_y++)
  			{
  				for(uint8_t lik_x=0;lik_x<(3);lik_x++)
  				{
  						if((blocks[block][(lik_x+3*lik_y)*(rot==1)+((2-lik_x)+(1-lik_y)*3)*(rot==3)]==1)) //pogleda če je v celici "1" -> torej če je tam sploh kaj narisano
  						{
  							UTIL_LCD_FillRect(old_x+lik_y*square_dim, old_y+((cbc[lik_y*3+lik_x]&0b001)>0)*(old_y>0)+(2-lik_x)*square_dim, square_dim-((cbc[lik_y*3+lik_x]&0b010)>0)+1, square_dim-((cbc[lik_y*3+lik_x]&0b001)>0)*(old_y>0)-((cbc[lik_y*3+lik_x]&0b100)>0)*(old_y<240)+1, UTIL_LCD_COLOR_WHITE);
  						}
  				}
  			}
  	  	}
  	  	if(rot==0||rot==2)
  	  	{
  	  	  	for(uint8_t lik_y=0;lik_y<(3);lik_y++)
  	  	  	{
  	  	  		for(uint8_t lik_x=0;lik_x<(2);lik_x++)
  	  	  		{
  	  					if(blocks[block][(lik_y+3*(1-lik_x))*(rot==0)+(lik_x*3+(2-lik_y))*(rot==2)]==1) //pogleda če je v celici "1" -> torej če je tam sploh kaj narisano
  	  					{
  	  						UTIL_LCD_FillRect(old_x+lik_y*square_dim, old_y+((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)+(1-lik_x)*square_dim, square_dim-((cbc[lik_y*2+lik_x]&0b010)>0)+1, square_dim-((cbc[lik_y*2+lik_x]&0b001)>0)*(old_y>0)-((cbc[lik_y*2+lik_x]&0b100)>0)*(old_y<240)+1, UTIL_LCD_COLOR_WHITE);
  	  					}
  	  	  		}
  	  	  	}
  	  	}
  	}

}

/**
  * @brief Game Over - izpiše "GAME OVER"
  * @param  None
  * @retval None
  */

void GAME_OVER()
{
	//zbriše ekran pod score napisom in črto
	UTIL_LCD_FillRect(50,0,480-50,272,UTIL_LCD_COLOR_WHITE);

	const uint8_t pixel_size=4;
	const uint8_t x_offset=180; //offset napisa v smeri x
	const uint8_t y_offset=70; //offset napisa v smeri y

	const uint8_t game_over_napis[8][9][8]={
	//G
	{{0,0,0,1,1,1,0,0},
	 {0,0,1,1,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,1,1,0,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,0,1,1,0,1,1,0},
	 {0,0,0,1,1,1,0,0}
	},
	//A
	{{0,0,0,1,1,0,0,0},
	 {0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,1,1,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	},
	//M
	{{0,1,1,0,0,1,1,0},
	 {0,1,1,1,1,1,1,0},
	 {0,1,0,1,1,0,1,0},
	 {0,1,0,1,1,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	},
	//E
	{{0,0,1,1,1,1,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,0,1,1,1,1,1,0}
	},
	//O
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
	//V
	{{0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0},
	 {0,0,0,1,1,0,0,0}
	},
	//E
	{{0,0,1,1,1,1,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,0,1,1,1,1,1,0}
	},
	//R
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,1,0,0,0},
	 {0,1,0,0,1,1,0,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0}
	}
	};

//izpišemo črke, ki so v arrayu že v pravilnem zaporedju,
//pri tem upoštevamo da po četrtem znaku izpis premaknemo v novo vrstico
for(uint8_t crka_num=0;crka_num<8;crka_num++)
{
	for(uint8_t score_y=0;score_y<8;score_y++)
	{
	  for(uint8_t score_x=0;score_x<9;score_x++)
	  {
		  if(game_over_napis[crka_num][score_x][score_y]==1){
		for(uint8_t y_temp=0;y_temp<pixel_size;y_temp++)
		{
			for(uint8_t x_temp=0;x_temp<pixel_size;x_temp++)
			{
				UTIL_LCD_SetPixel(x_offset+50*(crka_num>3)+score_x*pixel_size+x_temp,272-y_offset-y_temp-score_y*pixel_size-(crka_num-4*(crka_num>3))*pixel_size*8,UTIL_LCD_COLOR_RED);
			}
		}
		  }}
	}
}
}

/**
  * @brief Start Game - izpiše "PRESS ANY BUTTON"
  * @param  None
  * @retval None
  */

void START_GAME()
{

	//array za crke: S,A,E,O,R,P,B,N,Y,T,U
	uint8_t p[11][9][8]={
	//S
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
	//A
	{{0,0,0,1,1,0,0,0},
	 {0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,1,1,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	},
	//E
	{{0,0,1,1,1,1,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,1,0,0,0,1,0},
	 {0,0,1,1,1,1,1,0}
	},
	//O
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
	//R
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
	//P
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0},
	 {0,1,0,0,0,0,0,0}
	},
	//B
	{{0,0,1,1,1,1,0,0},
	 {0,1,1,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,1,1,1,1,0,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,1,1,1,1,0,0}
	},
	//N
	{{0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,0,1,0},
	 {0,1,1,1,0,0,1,0},
	 {0,1,0,1,0,0,1,0},
	 {0,1,0,1,1,0,1,0},
	 {0,1,0,0,1,0,1,0},
	 {0,1,0,0,1,1,1,0},
	 {0,1,0,0,0,1,1,0},
	 {0,1,0,0,0,0,1,0}
	},
	//Y
	{{0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0},
	 {0,0,0,1,1,1,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0}
	},
	//T
	{{0,1,1,1,1,1,1,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0},
	 {0,0,0,0,1,0,0,0}
	},
	//U
	{{0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,0,0,0,0,1,0},
	 {0,1,1,0,0,1,1,0},
	 {0,0,1,1,1,1,0,0}
	}
	};

	UTIL_LCD_FillRect(50,0,480-50,272,UTIL_LCD_COLOR_WHITE); //zapolni vse pod napisom SCORE in črto z belo barvo
	const uint8_t pixel_size=4;
	 uint16_t x_offset=180; //odmik od zgornjega roba ekrana
	 uint16_t y_offset=60; //odmik od levega roba ekrana

	//array za crke: S,A,E,O,R,P,B,N,Y,T,U
	//array za izpis: PRESS ANY BUTTON
	 uint8_t zaporedje[14]={5,4,2,0,0,1,7,8,6,10,9,9,3,7}; //indexi za izpis

	 //končen array za izpis
	 uint8_t napis[14][9][8];

	 //zapolnitev končnega arraya za izpis (p je array črk)
	 for(uint8_t q=0;q<14;q++)
	 {
		 for(uint8_t w=0;w<9;w++)
		 {
			 for(uint8_t e=0;e<8;e++)
			 {
				  napis[q][w][e]=p[zaporedje[q]][w][e];
			 }
		 }
	 }

	//izpis - press
	for(uint8_t crka_num=0;crka_num<5;crka_num++)
	{
		for(uint8_t score_y=0;score_y<8;score_y++)
		{
		  for(uint8_t score_x=0;score_x<9;score_x++)
		  {
			  if(napis[crka_num][score_x][score_y]==1){
			for(uint8_t y_temp=0;y_temp<pixel_size;y_temp++)
			{
				for(uint8_t x_temp=0;x_temp<pixel_size;x_temp++)
				{
					UTIL_LCD_SetPixel(x_offset+score_x*pixel_size+x_temp,272-y_offset-y_temp-score_y*pixel_size-crka_num*pixel_size*8,UTIL_LCD_COLOR_RED);
				}
			}
			  }}
		}
	}

	//premik koordinat:
	  x_offset+=50;
	  y_offset=80; //odmik od levega roba

	//izpis - any
	for(uint8_t crka_num=0;crka_num<3;crka_num++)
	{
		for(uint8_t score_y=0;score_y<8;score_y++)
		{
		  for(uint8_t score_x=0;score_x<9;score_x++)
		  {
			  if(napis[5+crka_num][score_x][score_y]==1){
			for(uint8_t y_temp=0;y_temp<pixel_size;y_temp++)
			{
				for(uint8_t x_temp=0;x_temp<pixel_size;x_temp++)
				{
					UTIL_LCD_SetPixel(x_offset+score_x*pixel_size+x_temp,272-y_offset-y_temp-score_y*pixel_size-crka_num*pixel_size*8,UTIL_LCD_COLOR_RED);
				}
			}
			  }}
		}
	}

	//premik koordinat:
	  x_offset+=50;
	  y_offset=50; //odmik od levega roba

	//izpis - button
	for(uint8_t crka_num=0;crka_num<6;crka_num++)
	{
		for(uint8_t score_y=0;score_y<8;score_y++)
		{
		  for(uint8_t score_x=0;score_x<9;score_x++)
		  {
			  if(napis[8+crka_num][score_x][score_y]==1){
			for(uint8_t y_temp=0;y_temp<pixel_size;y_temp++)
			{
				for(uint8_t x_temp=0;x_temp<pixel_size;x_temp++)
				{
					UTIL_LCD_SetPixel(x_offset+score_x*pixel_size+x_temp,272-y_offset-y_temp-score_y*pixel_size-crka_num*pixel_size*8,UTIL_LCD_COLOR_RED);
				}
			}
			  }}
		}
	}
}

/**
  * @brief Line Fill Check
  * @param  None
  * @retval Line Mask: maska vrstic, zapolnjene vrstice označene z "1"
  */

uint64_t LINE_FILL_CHECK() //vrne masko zapolnjenih vrstic (st. vrstice je enaka odmiku <<)
{
	uint8_t max_num_elements=27;
	uint8_t num_elements=0;

	uint64_t line_state=0;

	for(uint8_t vrstice=0;vrstice<43;vrstice++)
	{
		for(uint8_t stolpci=0;stolpci<27;stolpci++)
		{
			if(game_state[vrstice][stolpci]==1)num_elements++;
		}
		if(num_elements==max_num_elements)
		{
			uint64_t maska=1;
			for(uint8_t vrstice_temp=0;vrstice_temp<vrstice;vrstice_temp++)
			{
				maska=maska<<1;
			}
			line_state|=maska;
		}
		num_elements=0;
	}
return line_state;
}

void LINE_DELETE(uint64_t line_delete_mask)
{
	for(uint8_t line=0;line<43;line++)
	{
		uint64_t maska=1;
		for(uint8_t shift=0;shift<line;shift++)maska=maska<<1;

		if((line_delete_mask&maska)>0)
		{
			for(uint8_t element=0;element<27;element++)
			{
					//če je eno vrstico višje block premaknemo x risanja kvadrata eno dol

				//če je zadnja vrstica ne smemo gledati nižje v arrayu
				if(line==42)UTIL_LCD_FillRect(50+line*10,element*10,10,10,UTIL_LCD_COLOR_WHITE);
				//če pa ni zadnja vrstica lahko
				else UTIL_LCD_FillRect(50+line*10+1,element*10,9+(game_state[line+1][element]==0),10,UTIL_LCD_COLOR_WHITE);

			}
		}
	}
}

/**
  * @brief Line Down Shift
  * @param  gap_mask: maska zapolnjenih vrstic (polne vrstice so "1")
  * @retval None
  */

void LINE_DOWN_SHIFT(uint64_t gap_mask)
{
	uint8_t bottom_line=200;
	uint8_t num_lines_deleted=0;

	for(uint8_t mask_check=43;mask_check>0;mask_check--)
	{
		uint64_t mask=1;
		for(uint8_t shift=0;shift<mask_check;shift++)mask=mask<<1;

		if((gap_mask&mask)>0)
		{
			if(bottom_line==200)bottom_line=mask_check; //si zapise najnizjo vrstico ki je bila izbrisana; first flag
			num_lines_deleted++;
		}
	}

	UTIL_LCD_FillRect(50,0,430-(42-bottom_line)*10,272,UTIL_LCD_COLOR_WHITE); //Zbriše vse blocke ki jih bo premaknil (narisal na novo nižje)

	for(uint8_t vrstice=bottom_line-num_lines_deleted;vrstice>0;vrstice--)
	{
		for(uint8_t stolpci=0;stolpci<27;stolpci++)
		{
			uint16_t x_start=50+(vrstice+num_lines_deleted)*10; //zgornja crta
			uint16_t x_end=x_start+10;			//spodnja crta
			uint16_t y_start=stolpci*10;		//desna crta
			uint16_t y_end=y_start+10;			//leva crta

			if(color_game_state[vrstice][stolpci]!=100) //ČE JE bila barva kadarkoli overwritana (default je 100)
			{
				for(uint8_t pixel=0;pixel<10;pixel++) //narišemo obrobo celice
				{

					UTIL_LCD_SetPixel(x_start+pixel,y_start,COLORS[color_game_state[vrstice][stolpci]]); //preveri -1  (zaradi 0 index)
					UTIL_LCD_SetPixel(x_start+pixel,y_end,COLORS[color_game_state[vrstice][stolpci]]);
					UTIL_LCD_SetPixel(x_start,y_start+pixel,COLORS[color_game_state[vrstice][stolpci]]);
					if(x_end<480)UTIL_LCD_SetPixel(x_end,y_start+pixel,COLORS[color_game_state[vrstice][stolpci]]);
				}

					#ifdef FILL

					UTIL_LCD_FillRect(x_start+1,y_start+1,8,8,COLORS[color_game_state[vrstice][stolpci]]);

					#endif

			}

			//posodobimo game_state in color_game_state
			color_game_state[vrstice+num_lines_deleted][stolpci]=color_game_state[vrstice][stolpci]; //po izrisu celice (premiku dol) overwriteamo barvo v spominu
			color_game_state[vrstice][stolpci]=100; 												//premaknjen podatek pobrišemo
			game_state[vrstice+num_lines_deleted][stolpci]=game_state[vrstice][stolpci];
			game_state[vrstice][stolpci]=0;															//premaknjen podatek pobrišemo
		}
	}
}

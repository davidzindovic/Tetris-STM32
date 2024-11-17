


//definiraš blocke kot 2x3 matriko

//definiraš štartno pozicijo v zgornjem levem kotu. pri rotaciji se spremeni, lahko bi imel vnaprej definirane rotirane kocke
//štartna pozicija lahko zunaj ekrana /frame-a (v primeru rotacije)
//brisanje ekrana oz. stare pozicije od neke točke gor nek omejen doseg

//to do:
//scoring system
//meje kdaj se parkirajo blocki
//speedup

#define SQUARE_X_DIM 10
#define SQUARE_Y_DIM 10

/*struct block_struct{ //2x3 grid
  bool block1; //11
  bool block2; //12
  bool block3; //13
  bool block4; //21
  bool block5; //22
  bool block6; //23
}*/
blocks[5][6]={{0,1,0,1,1,1},
{0,0,1,1,1,1},
{0,0,0,1,1,1},
{1,0,0,1,1,1},
{0,1,1,0,1,1}}

bool block1[6]={0,1,0,1,1,1};
bool block2[6]={0,0,1,1,1,1};
bool block3[6]={0,0,0,1,1,1};
bool block4[6]={1,0,0,1,1,1};
bool block5[6]={0,1,1,0,1,1};


void block_draw(uint8_t block_pick, uint32_t block_color, uint32_t block_x_pos, uint32_t block_y_pos)
{
  for(uint8_t y=0;y<2;y++)
    {
      for(uint8_t x=0;x<3;x++)
        {
          if(blocks[block_pick][y*3+x+1])
          {
            for(uint8_t block_x_dim=0;block_dim<SQUARE_X_DIM;block_dim++)
              {
              draw(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+y*SQUARE_Y_DIM,block_color);
              draw(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+y*SQUARE_DIM+SQUARE_Y_DIM,block_color);
              }
            for(uint8_t block_y_dim=0;block_dim<SQUARE_Y_DIM;block_dim++)
              {
              draw(block_x_pos+x*SQUARE_X_DIM,block_y_dim+block_y_pos+y*SQUARE_Y_DIM,block_color);
              draw(block_x_pos+x*SQUARE_X_DIM+SQUARE_X_DIM,block_y_dim+block_y_pos+y*SQUARE_Y_DIM,block_color);
              }
          }
        } 
    }
}

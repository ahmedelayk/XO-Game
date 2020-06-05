#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
char checkWinner(void);
unsigned long TimerCount;
unsigned long Semaphore;
char pos,stepX,stepY,player,game_element;
char Winner=0;
char position[3][3];

	//PORTE F INITIATE
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}

	//PORTE E INITIATE
void PORTE_INIT(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock PortE
  GPIO_PORTE_CR_R = 0x03;           // allow changes to PE2       
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R = 0x02;          // 5) PE0 input, PE1 output   
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTE_PDR_R= 0x01;          // enable pulldown resistors on PE0       
  GPIO_PORTE_DEN_R = 0x01;          // 7) enable digital pins PE0-PE1
}
	//GAME INITIATE
void game_Init(){
	
		char i,j;
		pos=0; 
		game_element = (3*3-1);
		stepX=4,stepY=2;
		player=0;													// 0 for 'X' player and 1 for 'O' player
	 for(i=0;i<3;i++){
		 for(j=0;j<3;j++)
			position[i][j]=0;
		}
}
	
void Draw(){
	char i,j,x,y;
	x=84/3;
	y=48/3;
	for(i=0;i<3;i++){
		for(j=0;j<48;j++)Nokia5110_SetPixel(x*i,j);
	  for(j=0;j<84;j++)Nokia5110_SetPixel(j,y*i);
	}
	 Nokia5110_SetCursor(0,5);
	 Nokia5110_OutString(" 'X' Player");
}
	// START SCREEN
void start(){
		Nokia5110_ClearBuffer();
		Nokia5110_SetCursor(2,0);
		Nokia5110_OutString("XO Game");
		Delay100ms(2);
		Nokia5110_SetCursor(2,2);
		Nokia5110_OutString("made by");
		Delay100ms(2);	
		Nokia5110_SetCursor(3,4);
		Nokia5110_OutString("Elayek");
		Nokia5110_ClearBuffer();
		Delay100ms(4);
		Nokia5110_Clear();
	}

void set_Cursor() {
	 char posX,posY;
	 if( position[pos/3][pos%3]==0){
			posX=((pos%3)*stepX);
			posY=((pos/3)*stepY);
			position[pos/3][pos%3]='_';
			Nokia5110_SetCursor(posX,posY);
			Nokia5110_OutChar('_');
			Nokia5110_SetCursor(posX,posY);
	}
		 
}
void remove_Cursor(){
		 char posX,posY;
		 if(position[pos/3][pos%3]=='_'){
			    posX=((pos%3)*stepX);
				  posY=((pos/3)*stepY);
					position[pos/3][pos%3]=0;
					Nokia5110_SetCursor(posX,posY);
			  	Nokia5110_OutChar(' ');
		}
}



int main(void){
	
	 unsigned long SW1,SW2,SW3;
	 TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
	 Random_Init(1);
 	 Nokia5110_Init();
	 Nokia5110_ClearBuffer();
	 Nokia5110_DisplayBuffer();     
	 PortF_Init();
	 PORTE_INIT();
	 start();
	 game_Init();
	 Nokia5110_Clear();
	 Delay100ms(1);
	 Draw();
	 set_Cursor();
	 Nokia5110_SetCursor(0,0);
	
	while(1){
			 SW1 = GPIO_PORTF_DATA_R&0x10;     // read PF4 into SW1
			 SW2 = GPIO_PORTF_DATA_R&0x01;     // read PF0 into SW2
			 SW3 = GPIO_PORTE_DATA_R&0x01;     // read PE0 into SW3
			
				if(!(SW1))		//FOR MOVING RIGHT
					{ 
					remove_Cursor();				
					pos++;
					if(pos>game_element)pos=game_element;
					 while(!(GPIO_PORTF_DATA_R&0x10));
						set_Cursor();
					}
					
				if(!(SW2))		//FOR MOVING LEFT
				{
					remove_Cursor();
					pos--;
					if(pos<0)pos=0;
					while(!(GPIO_PORTF_DATA_R&0x01))
						set_Cursor();
				}
				
				if((SW3))		
					{
					while(GPIO_PORTE_DATA_R&0x01);
					if(!(player)){
							if(position[pos/3][pos%3]=='_')
							{
								Nokia5110_OutChar('X');
								position[pos/3][pos%3]='X';
								// increment cursor
								pos++;
								if(pos>game_element)pos=game_element;
								Nokia5110_SetCursor(0,5);
								Nokia5110_OutString(" 'O' Player "); 
								set_Cursor();
								player^=1;
							}
					}
					else{
							if(position[pos/3][pos%3]=='_')
								{
									Nokia5110_OutChar('O');
									position[pos/3][pos%3]='O';
									// increment cursor
									pos++;
									if(pos>game_element)pos=game_element;
									Nokia5110_SetCursor(0,5);						
									Nokia5110_OutString(" 'X' Player ");
		 							set_Cursor();
									player^=1;
							}
					}
					
					Winner=checkWinner();
				}
						//PRINT WIN PLAYER
				if(Winner){
						if(Winner=='x') {
								Nokia5110_Clear();
								Nokia5110_SetCursor(0,2);
								Nokia5110_OutString("player X Win");
						}
						if(Winner=='o') {
								Nokia5110_Clear();
								Nokia5110_SetCursor(0,2);
								Nokia5110_OutString("player O Win");
						}
						if(Winner=='q') {
								Nokia5110_Clear();
								Nokia5110_SetCursor(0,2);
								Nokia5110_OutString("_Game Over_");
						}
				}
  }
}
		//CHECK IF WINNER IS 'X' OR 'O'
char checkWinner(void)
	{
	   char i,j,countrX=0,countrY=0,countcX=0,countcY=0,countdX=0,countdY=0,countdrX=0,countdrY=0,Full=1;
		 for(i=0;i<3;i++){
				countrX=0,countrY=0,countcX=0,countcY=0;
				for(j=0;j<3;j++){
					if(position[i][j]=='X')countrX++;  // counter check x in rows
					if(position[i][j]=='O')countrY++;	// counter check o in rows
					if(position[j][i]=='X')countcX++;  // counter check x in cols
					if(position[j][i]=='O')countcY++;	// counter check o in cols
					if(position[i][j]==0) Full=0;			
			 }
				if(position[i][i]=='X')countdX++;  // counter check x in diagonal
				if(position[i][i]=='O')countdY++;  // counter check o in diagonal
				if(position[i][3-1-i]=='X')countdrX++; // counter check x in other diagonal
				if(position[i][3-1-i]=='O')countdrY++; // counter check o in other diagonal
				if(countrX==3||countcX==3||countdX==3||countdrX==3)return 'x';
				if(countrY==3||countcY==3||countdY==3||countdrY==3)return 'o';
					
	 }
		 if(Full) return 'q';
	   return 0;
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

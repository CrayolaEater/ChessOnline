#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include<SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<stdio.h>
extern int errno;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
bool RandulMeu = 0;
int PlayerNumber, Scor; // numarul jucatorului curent
int port;
SDL_Rect TablaGrafica[32];
char TablaInfo[8][8];
SDL_Texture ** piese_albe;
SDL_Texture ** piese_negre;
SDL_Renderer * renderer;
SDL_Event event;
bool A_selectat = 0, aMutat = 0;
int lg_tablaGf, MutareX = -1, MutareY =-1, DestinatieX=-1, DestinatieY=-1, NrMutari;
int DirectiiCalL[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
int DirectiiCalC[8] = {-1, -2, -2, -1, 1, 2, 2, 1};
int DirectiiRegeL[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int DirectiiRegeC[8] = {0, -1, -1, -1, 0, 1, 1, 1};
struct celula
{
	int x;
	int y;
}PosibileMutari[27];
//The window we'll be rendering to
SDL_Window* window = NULL;
void ComandaInCoordonate(char * comanda, int * x, int * y, int * x1, int * y1)
{
	*x = comanda[0] - 'a';
	*x1 = comanda[3] - 'a';
	*y = comanda[1] - 49;
	*y1 = comanda[4] - 49;
}
void GenereazaTabla(SDL_Renderer* renderer, int cell_width, int cell_height)
{
	piese_albe = (SDL_Texture**)malloc(sizeof(SDL_Texture*)*6);
	piese_negre = (SDL_Texture**)malloc(sizeof(SDL_Texture*)*6);
	TablaInfo[1][0] = TablaInfo[1][1] = TablaInfo[1][2] = TablaInfo[1][3] = TablaInfo[1][4] = TablaInfo[1][5] = TablaInfo[1][6] = TablaInfo[1][7] = 101;
	TablaInfo[0][0] = TablaInfo[0][7] = 105;
	TablaInfo[0][1] = TablaInfo[0][6] = 113;
	TablaInfo[0][2] = TablaInfo[0][5] = 103;
	TablaInfo[0][3] = 109; TablaInfo[0][4] = 107;
	
	TablaInfo[6][0] = TablaInfo[6][1] = TablaInfo[6][2] = TablaInfo[6][3] = TablaInfo[6][4] = TablaInfo[6][5] = TablaInfo[6][6] = TablaInfo[6][7] = 1;
	TablaInfo[7][0] = TablaInfo[7][7] = 5;
	TablaInfo[7][1] = TablaInfo[7][6] = 13;
	TablaInfo[7][2] = TablaInfo[7][5] = 3;
	TablaInfo[7][3] = 9; TablaInfo[7][4] = 7;
	SDL_Surface * s = IMG_Load("piese/pion1.png");
	piese_albe[0]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/nebun1.png");
	piese_albe[1]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/turla1.png");
	piese_albe[2]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/cal1.png");
	piese_albe[3]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/regina1.png");
	piese_albe[4]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/rege1.png");
	piese_albe[5]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	
	s = IMG_Load("piese/pion2.png");
	piese_negre[0]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/nebun2.png");
	piese_negre[1]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/turla2.png");
	piese_negre[2]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/cal2.png");
	piese_negre[3]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/regina2.png");
	piese_negre[4]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	s = IMG_Load("piese/rege2.png");
	piese_negre[5]=SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	for(int i = 0; i < 8 ; i ++)
	{
		for(int j = !(i%2); j < 8; j += 2)
		{
			SDL_Rect * casuta = &TablaGrafica[lg_tablaGf++];
			casuta->x = i * cell_width;
			casuta->y = j * cell_height;
			casuta->w = cell_width;
			casuta->h = cell_height;
		}
	}
}
void DeseneazaPiesele()
{
	SDL_Texture ** piese;
	for(int i = 0 ; i < 8; i ++)
	{
		for(int j = 0 ; j < 8 ; j ++)
		{
			if(TablaInfo[i][j] > 0)
			{
				if(TablaInfo[i][j] > 100) // daca sunt piese negre
				{
					piese = piese_negre;
				}
				else
				{
					piese = piese_albe;
				}
				SDL_Rect * destination = (SDL_Rect*)malloc(sizeof(SDL_Rect));
				destination->x = j*80;
				destination->y = i*60;
				destination-> w= 80;
				destination-> h= 60;
				if(TablaInfo[i][j] % 100 == 1) // pion
				{
					SDL_RenderCopy(renderer, piese[0], NULL, destination);
				}
				else if(TablaInfo[i][j] % 100 == 3) // nebun
				{
					SDL_RenderCopy(renderer, piese[1], NULL, destination);
				}
				else if(TablaInfo[i][j] % 100 == 13) // cal
				{
					SDL_RenderCopy(renderer, piese[3], NULL, destination);
				}
				else if(TablaInfo[i][j] % 100 == 5) // turla
				{
					SDL_RenderCopy(renderer, piese[2], NULL, destination);
				}
				else if(TablaInfo[i][j] % 100 == 9) // regina
				{
					SDL_RenderCopy(renderer, piese[4], NULL, destination);
				}
				else //regele
				{
					SDL_RenderCopy(renderer, piese[5], NULL, destination);
				}
				free(destination);
			}
		}
	}
}
void DeseneazaMutariPosibile()
{
    SDL_SetRenderDrawColor( renderer, 235, 216, 52, 100);
	for(int i = 0 ; i < NrMutari; i++)
	{
		SDL_Rect mutare;
		mutare.x = (PosibileMutari[i].x * 80) + 5;
		mutare.y = (PosibileMutari[i].y * 60) + 5;
		mutare.w = 70;
		mutare.h = 50;
	    SDL_RenderFillRect( renderer, &mutare);
	}
}
void DeseneazaTabla()
{
    SDL_SetRenderDrawColor( renderer, 0, 186, 146, 255 );
    SDL_RenderClear( renderer );
    SDL_SetRenderDrawColor( renderer, 16, 125, 102, 255 );
	for(int i = 0 ; i < lg_tablaGf; i++)
	{
	    SDL_RenderFillRect( renderer, &TablaGrafica[i]);
	}
	DeseneazaPiesele();
	DeseneazaMutariPosibile();
    SDL_RenderPresent(renderer);
}
void SelecteazaPiesa()
{
	NrMutari = 0;
	if(PlayerNumber==1)
	{
		if(TablaInfo[MutareY][MutareX]==1) // daca e pion selectat
		{
			struct celula celula1;
			celula1.x=MutareX;
			celula1.y=MutareY-1;
			if(celula1.y < 0)
			{
				return;
			} 
			if(TablaInfo[celula1.y][celula1.x] == 0)
			{
				PosibileMutari[NrMutari++]=celula1;
			}
			if(MutareY == 6 && TablaInfo[MutareY-1][MutareX] == 0) //inseamna ca pionul e pe pozitia de inceput
			{
				struct celula celula2;
				celula2.x=MutareX;
				celula2.y=MutareY-2;
				if(TablaInfo[celula2.y][celula2.x] == 0)
				{
					PosibileMutari[NrMutari++]=celula2;	
				}
			}
			if(TablaInfo[celula1.y][celula1.x-1] > 100)
			{
				PosibileMutari[NrMutari].x = celula1.x-1;
				PosibileMutari[NrMutari++].y = celula1.y;
			}
			if(TablaInfo[celula1.y][celula1.x+1] > 100)
			{
				PosibileMutari[NrMutari].x = celula1.x+1;
				PosibileMutari[NrMutari++].y = celula1.y;
			}
		}
		else if(TablaInfo[MutareY][MutareX] == 13)
		{
			for(int i = 0; i < 8; i ++)
			{
				int x = MutareX + DirectiiCalC[i];
				int y = MutareY + DirectiiCalL[i];
				if(x>= 0 && x < 8 && y >= 0 && y < 8 &&(TablaInfo[y][x] == 0 || TablaInfo[y][x] > 100))
				{
					PosibileMutari[NrMutari].x=x;
					PosibileMutari[NrMutari++].y=y;
				}
			}
		}
		else if(TablaInfo[MutareY][MutareX] == 7)
		{
			for(int i = 0; i < 8; i ++)
			{
				int x = MutareX + DirectiiRegeC[i];
				int y = MutareY + DirectiiRegeL[i];
				if(x>= 0 && x < 8 && y >= 0 && y < 8 &&(TablaInfo[y][x] == 0 || TablaInfo[y][x] > 100))
				{
					PosibileMutari[NrMutari].x=x;
					PosibileMutari[NrMutari++].y=y;
				}
			}
		}
		if(TablaInfo[MutareY][MutareX] == 5 || TablaInfo[MutareY][MutareX] == 9)
		{
			for(int i = MutareY - 1; i >= 0; i--)
			{
				if(TablaInfo[i][MutareX] > 0)
				{
					if(TablaInfo[i][MutareX] > 100)
					{
						struct celula poz;
						poz.x = MutareX;
						poz.y = i;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = MutareX;
				poz.y = i;
				PosibileMutari[NrMutari++] = poz;
			}
			for(int i = MutareY + 1; i < 8; i++)
			{
				if(TablaInfo[i][MutareX] > 0)
				{
					if(TablaInfo[i][MutareX] > 100)
					{
						struct celula poz;
						poz.x = MutareX;
						poz.y = i;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = MutareX;
				poz.y = i;
				PosibileMutari[NrMutari++] = poz;
			}
			for(int i = MutareX -1; i >=0; i--)
			{
				if(TablaInfo[MutareY][i] > 0)
				{
					if(TablaInfo[MutareY][i] > 100)
					{
						struct celula poz;
						poz.x = i;
						poz.y = MutareY;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = i;
				poz.y = MutareY;
				PosibileMutari[NrMutari++] = poz;
			}
			for(int i = MutareX +1; i < 8; i++)
			{
				if(TablaInfo[MutareY][i] > 0)
				{
					if(TablaInfo[MutareY][i] > 100)
					{
						struct celula poz;
						poz.x = i;
						poz.y = MutareY;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = i;
				poz.y = MutareY;
				PosibileMutari[NrMutari++] = poz;
			}
		}
		if(TablaInfo[MutareY][MutareX] == 3 || TablaInfo[MutareY][MutareX] == 9)
		{
			int i, j;
			for(i = MutareY - 1, j = MutareX + 1; i >= 0 && j < 8; i --, j++)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] > 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
			for(i = MutareY + 1, j = MutareX + 1; i < 8 && j < 8; i ++, j++)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] > 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
			for(i = MutareY + 1, j = MutareX - 1; i < 8 && j >= 0; i ++, j--)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] > 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
			for(i = MutareY - 1, j = MutareX - 1; i >= 0 && j >= 0; i --, j--)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] > 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
		}
	}
	else if(PlayerNumber==2)
	{
		if(TablaInfo[MutareY][MutareX]==101) // daca e pion selectat
		{
			struct celula celula1;
			celula1.x=MutareX;
			celula1.y=MutareY+1;
			if(celula1.y > 7)
			{
				return;
			} 
			if(TablaInfo[celula1.y][celula1.x] == 0)
			{
				PosibileMutari[NrMutari++]=celula1;
			}
			if(MutareY == 1 && TablaInfo[MutareY + 1][MutareX] == 0) //inseamna ca pionul e pe pozitia de inceput
			{
				struct celula celula2;
				celula2.x=MutareX;
				celula2.y=MutareY+2;
				if(TablaInfo[celula2.y][celula2.x] == 0)
				{
					PosibileMutari[NrMutari++]=celula2;	
				}
			}
			if(TablaInfo[celula1.y][celula1.x-1] < 100 && TablaInfo[celula1.y][celula1.x-1] > 0) 
			{
				PosibileMutari[NrMutari].x = celula1.x-1;
				PosibileMutari[NrMutari++].y = celula1.y;
			}
			if(TablaInfo[celula1.y][celula1.x+1] < 100 && TablaInfo[celula1.y][celula1.x+1] > 0)
			{
				PosibileMutari[NrMutari].x = celula1.x+1;
				PosibileMutari[NrMutari++].y = celula1.y;
			}
		}
		else if(TablaInfo[MutareY][MutareX] == 113)
		{
			for(int i = 0; i < 8; i ++)
			{
				int x = MutareX + DirectiiCalC[i];
				int y = MutareY + DirectiiCalL[i];
				if(x>= 0 && x < 8 && y >= 0 && y < 8 && TablaInfo[y][x]<100)
				{
					PosibileMutari[NrMutari].x=x;
					PosibileMutari[NrMutari++].y=y;
				}
			}
		}
		else if(TablaInfo[MutareY][MutareX] == 107)
		{
			for(int i = 0; i < 8; i ++)
			{
				int x = MutareX + DirectiiRegeC[i];
				int y = MutareY + DirectiiRegeL[i];
				if(x>= 0 && x < 8 && y >= 0 && y < 8 && TablaInfo[y][x]<100)
				{
					PosibileMutari[NrMutari].x=x;
					PosibileMutari[NrMutari++].y=y;
				}
			}
		}
		if(TablaInfo[MutareY][MutareX] == 103 || TablaInfo[MutareY][MutareX] == 109)
		{
			int i, j;
			for(i = MutareY - 1, j = MutareX + 1; i >= 0 && j < 8; i --, j++)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] < 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
			for(i = MutareY + 1, j = MutareX + 1; i < 8 && j < 8; i ++, j++)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] < 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
			for(i = MutareY + 1, j = MutareX - 1; i < 8 && j >= 0; i ++, j--)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] < 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
			for(i = MutareY - 1, j = MutareX - 1; i >= 0 && j >= 0; i --, j--)
			{
				if(TablaInfo[i][j] > 0)
				{
					if(TablaInfo[i][j] < 100)
					{
						struct celula poz;
						poz.x = j;
						poz.y = i;
						PosibileMutari[NrMutari++]=poz;
					}
					break;
				}
				struct celula poz;
				poz.x = j;
				poz.y = i;
				PosibileMutari[NrMutari++]=poz;	
			}
		}
		if(TablaInfo[MutareY][MutareX] == 105 || TablaInfo[MutareY][MutareX] == 109)
		{
			for(int i = MutareY - 1; i >= 0; i--)
			{
				if(TablaInfo[i][MutareX] > 0)
				{
					if(TablaInfo[i][MutareX] < 100)
					{
						struct celula poz;
						poz.x = MutareX;
						poz.y = i;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = MutareX;
				poz.y = i;
				PosibileMutari[NrMutari++] = poz;
			}
			for(int i = MutareY + 1; i < 8; i++)
			{
				if(TablaInfo[i][MutareX] > 0)
				{
					if(TablaInfo[i][MutareX] < 100)
					{
						struct celula poz;
						poz.x = MutareX;
						poz.y = i;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = MutareX;
				poz.y = i;
				PosibileMutari[NrMutari++] = poz;
			}
			for(int i = MutareX -1; i >=0; i--)
			{
				if(TablaInfo[MutareY][i] > 0)
				{
					if(TablaInfo[MutareY][i] < 100)
					{
						struct celula poz;
						poz.x = i;
						poz.y = MutareY;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = i;
				poz.y = MutareY;
				PosibileMutari[NrMutari++] = poz;
			}
			for(int i = MutareX +1; i < 8; i++)
			{
				if(TablaInfo[MutareY][i] > 0)
				{
					if(TablaInfo[MutareY][i] < 100)
					{
						struct celula poz;
						poz.x = i;
						poz.y = MutareY;
						PosibileMutari[NrMutari++] = poz;
					}
					break;
				}
				struct celula poz;
				poz.x = i;
				poz.y = MutareY;
				PosibileMutari[NrMutari++] = poz;
			}
		}
	}
}
bool Input()
{
	int x, y;
	while(SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
		    	return 0;
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						SDL_GetMouseState(&x, &y);
						if(MutareX == -1)
						{
							if(TablaInfo[y/60][x/80] == 0)
							{
								break;
							}
							MutareX = x/80;
							MutareY = y/60;
							if(TablaInfo[MutareY][MutareX] == 0 || (PlayerNumber==1 && TablaInfo[MutareY][MutareX] > 100) || (PlayerNumber==2 && TablaInfo[MutareY][MutareX]<100))
							{
								MutareX = -1;
							}
							else 
							{
								SelecteazaPiesa();
							}
						}
						else if(DestinatieX == -1)
						{
							DestinatieX = x/80;
							DestinatieY = y/60;
							for(int i = 0 ; i < NrMutari; i++)
							{
								if(x >= PosibileMutari[i].x*80 && x <PosibileMutari[i].x*80+80 && y >= PosibileMutari[i].y*60 && y < PosibileMutari[i].y*60+60)
								{
									NrMutari = 0;
									DestinatieX = PosibileMutari[i].x;
									DestinatieY = PosibileMutari[i].y;
									A_selectat = 1;
									break;
								}
							}
							if(A_selectat == 0)
							{
								MutareX = DestinatieX;
								MutareY = DestinatieY;
								DestinatieX = -1;
								SelecteazaPiesa();
							}
						}
						//SDL_ShowSimpleMessageBox(0, "Mouse", "Left button was pressed!", window);
						break;
					case SDL_BUTTON_RIGHT:
						//SDL_ShowSimpleMessageBox(0, "Mouse", "Right button was pressed!", window);
						break;
					default:
						//SDL_ShowSimpleMessageBox(0, "Mouse", "Some other button was pressed!", window);
						break;
				}
			break;
		}
    }
    return 1;
}
bool Rand()
{
	while(SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
		    	return 0;
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						if(PlayerNumber < 3)
						{
							SDL_ShowSimpleMessageBox(0, "Mesaj", "Nu este randul tau!", window);
							break;
						}
				}
			break;
		}
    }
    return 1;
}
void InitializeSDL()
{	
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
    window = SDL_CreateWindow
    (
        "Chess Online!", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	GenereazaTabla(renderer, 80, 60);
	DeseneazaTabla();
}

int main (int argc, char **argv)
{
    int socket_descriptor;            // descriptorul de socket
    struct sockaddr_in server;    // structura folosita pentru conectare
    // mesajul trimis
    char mutare[5]= {0};
    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }
    port = atoi (argv[2]);
    socket_descriptor = socket (AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (port);
    connect (socket_descriptor, (struct sockaddr *) &server,sizeof (struct sockaddr));
    if(read(socket_descriptor, &PlayerNumber, sizeof(PlayerNumber)) <=0)
    {
        perror("Eroare la asign-area numarului jucatorului!\n");
    }
    else
    {
    	if(PlayerNumber == 1){SDL_ShowSimpleMessageBox(0, "Mesaj", "Esti Player 1 (Alb)!", window);}
    	else if(PlayerNumber == 2) {SDL_ShowSimpleMessageBox(0, "Mesaj", "Esti Player 2 (Negru)!", window);}
    	else SDL_ShowSimpleMessageBox(0, "Mesaj", "Esti spectator!", window);
    }
    int PotMuta = -1;
    RandulMeu = (PlayerNumber == 1);
    InitializeSDL();
    while(1) // trebuie schimbat cu conditia clientului de iesire
    {
    	if(RandulMeu == 0)
    	{
    		if(Rand() == 0)
    		{
    			break;
    		}
    	}
    	if(NrMutari > 0) 
    	{
	    	DeseneazaTabla();
    	}
    	if(aMutat == 0)
    	{
		    if(write(socket_descriptor, &PlayerNumber, sizeof(PlayerNumber))<=0)
		    {
		        printf("Eroare la aflarea randului!\n");
		        break;
		    }
	    	read(socket_descriptor, &PotMuta, sizeof(PotMuta));
        }
        if(PotMuta == 0)
        {
	        if(aMutat == 0)
	        {
		        read(socket_descriptor, mutare, sizeof(mutare));
		        if(strlen(mutare) > 0)
		        {
		            //printf("!!!!\nS-a efectuat mutarea[%s]\n!!!!\n", mutare);
		            int x1,x2,y1,y2;
		        	ComandaInCoordonate(mutare, &x1, &y1, &x2, &y2);
		        	if(TablaInfo[x2][y2] % 10 == 7)
		        	{
		        		SDL_ShowSimpleMessageBox(0, "Mesaj", "Ai pierdut!", window);
		        	}
				    TablaInfo[x2][y2] = TablaInfo[x1][y1];
			        TablaInfo[x1][y1] = 0;
		        	DeseneazaTabla();
		        	RandulMeu =1;
		        }
		        //inseamna ca mi s-a permis de catre server sa fac mutarea
		        //printf("Este randul tau!\nIntrodu mutarea: \n");
            }
        	if(Input() == 0)
			{
				break;
			}
			if(!A_selectat)
			{
				aMutat = 1;
				continue;
			}
            mutare[0] = 97 + MutareY; mutare[1] = MutareX + 49; mutare[2] = '>'; mutare[3] = DestinatieY + 97; mutare[4] = DestinatieX + 49;
            aMutat = 0;
            write(socket_descriptor, mutare, sizeof(mutare));
            //printf("Ai mutat cu succes!\n");
            A_selectat = false;
            Scor += TablaInfo[DestinatieY][DestinatieX]%10;
        	if(TablaInfo[DestinatieY][DestinatieX] % 10 == 7)
        	{
        		SDL_ShowSimpleMessageBox(0, "Mesaj", "Ai castigat!", window);
        	}
	        TablaInfo[DestinatieY][DestinatieX] = TablaInfo[MutareY][MutareX];
            TablaInfo[MutareY][MutareX] = 0;
            MutareX = DestinatieX = -1;
        	DeseneazaTabla();
        	RandulMeu = 0;
        }
        SDL_Delay(20);
    }
    close (socket_descriptor);
    
    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

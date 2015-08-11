/**
********************************* PI - Tower Defense 2048 ********************************
Alunos: Vinicius Magaton Bussola, Rodrigo Benegas, Jilter, Bruno Eduardo, Nicholas Ribeiro
*/
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <SDL/SDL_gfxPrimitives.h>	// SDL_gfx Primitives
#include <math.h>

#define ALTURA 700
#define COMPRIMENTO 900

using namespace std;

// Variaveis globais
SDL_Surface *screen = NULL; //superficie que representa a tela do computador
//SDL_Surface **torreMouse = NULL; //Array de Torre que fica no mouse
SDL_Surface* menu = NULL;
SDL_Surface* caminho = NULL;
SDL_Surface* portal = NULL;
SDL_Surface* naoCaminho = NULL;
SDL_Surface* castelo = NULL;
SDL_Surface* torreNoMouse = NULL;
SDL_Surface *torreMenu = NULL; //Imagem da torre no menu
SDL_Surface *paginaInicial = NULL;
SDL_Surface *tutorial = NULL;
SDL_Surface *gameOver = NULL;
SDL_Surface *parabens = NULL;
SDL_Surface *reiniciar = NULL;
SDL_Surface *iniciar = NULL;
SDL_Rect torreMouse_destino;// onde irei desenhar a torre do mouse
SDL_Rect torreMenu_destino;//onde irei desenhar a torre do menu
SDL_Rect reiniciar_destino;//onde irei desenhar o botao de reiniciar
SDL_Rect iniciar_destino;//onde irei desenhar o botao de reiniciar
SDL_Rect menu_destino;
SDL_Rect paginaInicial_destino;
SDL_Rect portal_destino;
SDL_Rect **localCenario;
SDL_Rect *localCaminho;
SDL_Rect *localNaoCaminho;
int qntTorres = 0;
int torre_no_mouse = 0;
int id = 1;
int inicio_do_quadriculadoX; //Onde come o cenario do jogo
int inicio_do_quadriculadoY; //Onde come o cenario do jogo
int dinheiro, vida;//Dinheiro e vida do jogador
int vidaDoCastelo;
int tela = 0;
int idTorres = 0, idTiros = 0, idInimigo = 0;
int qntCaminho; //Quantos quadrados caminho existe no mapa
bool teste; //Para nao cair no caso da imagem da torre estar nula
bool caminhoCriado = false;
int numero = 0;
char bufferVidaInimigo[5];
int coordenadaX, coordenadaY; // Coordenada do inimigo
int contador_tempo; // Conta a cada tick, para fazer a frequencia do tiro da torre
int contador_inimigo; // Conta a cada tick, para criar um inimigo
bool insere_inimigo = false;
int qntInimigos = 0;
int frequencia_tiro = 65;
int ganhou = 0;


struct Cenario{
   int caminho; //0 = nao caminho.   1 = caminho.   9 = castelo
   int x;
   int y;
   int usada; //0 para nao e 1 para sim
};

// Lista de inimigos
struct Inimigo {
   int id;
   SDL_Rect coord;
   SDL_Surface* imagem;
   Inimigo *prox;
   int direcao;
   int posicaoLocalCaminho;
   int vida;
};

struct Tiro{
    int id;
    int forca;
    int velocidade;
    int level;
    SDL_Surface *imagem = NULL;
    SDL_Rect coord;
};

struct Torre{
   SDL_Surface *imagem1 = NULL;
   SDL_Surface *tiroImagem = NULL;
   Torre *prox;
   SDL_Rect coord;
   SDL_Rect tiroCoord;
   int level;
   int id;
   bool usandoTiro;

   Tiro *tiro;
};

struct PilhaTorres{
   Torre *topo;
};

struct ListaInimigos{
    Inimigo *inicio;
};

Torre *torreMouse = new Torre;
Cenario ***criaMatriz();
Cenario ***matriz;
PilhaTorres *criaPilha();
PilhaTorres *pilha;
ListaInimigos *criaLista();
ListaInimigos *listaInimigo;

Inimigo *inimigo;
int pegaOrdemDaMatriz();
int ordemDaMatriz = pegaOrdemDaMatriz();
void imprimeTorres(PilhaTorres *p);
void imprimeInimigos(ListaInimigos *i);
void insere(PilhaTorres *p, SDL_Rect rect);
void desenha(PilhaTorres *p, ListaInimigos *l);
void criaCenario(Cenario ***matriz);
void trocaRetira(PilhaTorres *p, Torre *e);
void trocaRetiraTorre(PilhaTorres *p, Torre *e);
void libera(PilhaTorres *p);
void retiraNula(PilhaTorres *p);
void imprimeTiros(PilhaTorres *p);
void adicionaInimigo(ListaInimigos *lista);
void moveInimigos(ListaInimigos *i);
void meteBala(PilhaTorres *p, ListaInimigos *l);
void addInimigoTela(ListaInimigos *l);
void retiraInimigo(ListaInimigos *l, Inimigo *i);
SDL_Rect *criaCaminho();
SDL_Rect *pegaNaoCaminho();
Torre *retira(PilhaTorres *p);
Tiro *criaTiro();

int main(int argc, char *argv[]){
   //Inimigo *inimigo = NULL;
   Torre *t;
   SDL_Event event; //um evento enviado pela SDL
   int JogoAtivo = 1; //Inicializa a condicao para GameLoop do Jogo
   matriz = criaMatriz();

   //- Inicializa a SDL -//
   if(SDL_Init(SDL_INIT_VIDEO) < 0){
       //printf("SDL_Init: %s\n", SDL_GetError());
       exit(1);
   }

   // Inicializa a tela
   screen = SDL_SetVideoMode(COMPRIMENTO ,ALTURA, 32, SDL_SWSURFACE|SDL_DOUBLEBUF );
   if(screen == NULL){
       //printf("SDL_SetVideoMode: %s\n", SDL_GetError());
       SDL_Quit(); //encerrar a SDL
       exit(2); //- Configura??o de v?deo n?o foi aceita
   }

   //- t?tulo da janela
   SDL_WM_SetCaption("Tower Defense", NULL);

   // carrega a imagem usando a funcao
   // SDL_Surface *SDL_LoadBMP(const char *file);
   torreMenu = SDL_LoadBMP("torre2.bmp");
   torreNoMouse = SDL_LoadBMP("torre2Peq.bmp");
   menu = SDL_LoadBMP("menu.bmp");
   naoCaminho = SDL_LoadBMP("grama.bmp");
   caminho = SDL_LoadBMP("rua.bmp");
   castelo = SDL_LoadBMP("castelo.bmp");
   portal = SDL_LoadBMP("portal.bmp");
   paginaInicial = SDL_LoadBMP("telaInicial.bmp");
   tutorial = SDL_LoadBMP("tutorial.bmp");
   gameOver = SDL_LoadBMP("gameOver.bmp");
   parabens = SDL_LoadBMP("parabens.bmp");
   reiniciar = SDL_LoadBMP("reiniciar.bmp");
   iniciar = SDL_LoadBMP("iniciar.bmp");
   //SDL_MixAudio(stream, mixData, len, SDL_MIX_MAXVOLUME / 2);

   //Local onde estara a torre no menu
   torreMenu_destino.x = 10;
   torreMenu_destino.y = 80;
   //Local onde estara o menu
   menu_destino.x = 0;
   menu_destino.y = 0;
   //Local onde estara a torre no menu
   reiniciar_destino.x = 10;
   reiniciar_destino.y = ALTURA/4+100;

   //Local onde estara o botao para iniciar a orda
   iniciar_destino.x = 10;
   iniciar_destino.y = ALTURA/4+400;

   paginaInicial_destino.x = 0;
   paginaInicial_destino.y = 0;

   //Local onde sera criado o primeiro quadrado do cenario
   localCenario = new SDL_Rect*[ordemDaMatriz];
   // aloca as colunas nas linhas
   for(int aux=0;aux<ordemDaMatriz;aux++)
       localCenario[aux]=new SDL_Rect[ordemDaMatriz];
   //le a matriz a atribue para cada elemento do array um local que sera criado o cenario
   for(int i=0;i<ordemDaMatriz;i++){
       for(int j=0;j<ordemDaMatriz;j++){
           localCenario[i][j].x = (COMPRIMENTO/2+menu->w/2-naoCaminho->w*(ordemDaMatriz/2))+i*(naoCaminho->w);// Ajusta o cenario
           localCenario[i][j].y = (ALTURA/2-naoCaminho->h*(ordemDaMatriz/2))+j*(naoCaminho->h);//Ajusta o cenario no centro da tela
       }
   }
   inicio_do_quadriculadoX = localCenario[0][0].x;
   inicio_do_quadriculadoY = localCenario[0][0].y;


   // Game Loop
   while( JogoAtivo ){
       // Aqui tratamos os comandos
       while(SDL_PollEvent(&event)){
           // Processar o tipo do evento
           switch(event.type){
               case SDL_KEYDOWN:
                   if(tela != 2)
                       tela++;
                   //printf("Foi pressionada uma tecla!\n");
                   break;
                   //Seta a imagem da torre para ficar no mouse
               case SDL_MOUSEMOTION:{
                   // pega o x,y do clique do mouse
                   if( torre_no_mouse == 1 || torre_no_mouse == 3){
                       torreMouse_destino.x = event.motion.x -(torreNoMouse->w/2);
                       torreMouse_destino.y = event.motion.y -(torreNoMouse->h/2);
                   }
               }
               break;

                //Caso: Mouse apertado
                case SDL_MOUSEBUTTONDOWN:{
                    int x = event.motion.x;
                    int y = event.motion.y;
                    // Botao esquerdo apertado
                    if(event.button.button==SDL_BUTTON_LEFT && tela == 2){
                        //Iniciar a orda
                        if(x>iniciar_destino.x
                        && x<(iniciar_destino.x+iniciar->w)
                        && y>iniciar_destino.y
                        && y<(iniciar_destino.y+iniciar->h)
                        && torre_no_mouse == 0
                        ){//Se o clique for dentro do botao iniciar
                            //adicionaInimigo(listaInimigo);
                            insere_inimigo = true;
                        }
                        //Reiniciar o jogo
                        if(x>reiniciar_destino.x
                        && x<(reiniciar_destino.x+reiniciar->w)
                        && y>reiniciar_destino.y
                        && y<(reiniciar_destino.y+reiniciar->h)
                        && torre_no_mouse == 0
                        ){//Se o clique for dentro do botao reiniciar
                            if(pilha->topo != NULL)
                                libera(pilha);
                           numero = 0;
                           tela = 0;
                        }
                        //fora do menu
                        if((x<torreMenu_destino.x
                        || x>(torreMenu_destino.x+torreMenu->w)
                        || y<torreMenu_destino.y
                        || y>(torreMenu_destino.y+torreMenu->h)) //fora do menu
                        && x>inicio_do_quadriculadoX             //dentro do cenario
                        && x<(inicio_do_quadriculadoX+ordemDaMatriz*torreNoMouse->w)
                        && y>inicio_do_quadriculadoY
                        && y<(inicio_do_quadriculadoY+ordemDaMatriz*torreNoMouse->h)
                        ){
                            //mouse com torre -- Cria uma torre nova, fora do menu
                            if(torre_no_mouse == 1){
                                for(int i = 0;i<ordemDaMatriz*ordemDaMatriz-qntCaminho;i++){
                                    if((x>localNaoCaminho[i].x && x<localNaoCaminho[i].x+torreNoMouse->w
                                    && y>localNaoCaminho[i].y && y<localNaoCaminho[i].y+torreNoMouse->h)
                                    ){ //Se o clique do mouse estiver dentro do nao caminho
                                        //printf("LOCAL X: %d, local y: %d \n", localNaoCaminho[i].x, localNaoCaminho[i].y);
                                        torreMouse_destino.x = event.button.x;
                                        torreMouse_destino.y = event.button.y;
                                        insere(pilha, torreMouse_destino);
                                        torre_no_mouse = 2;
                                        dinheiro = dinheiro - 50;
                                    }
                                    else{
                                        //break;
                                        continue;
                                    }
                                }
                               // break;
                            }
                            //mouse com torre -- Move uma torre ja existente, fora do menu
                            teste = true; //Para nao cair no caso da imagem da torre estar nula
                            if(torre_no_mouse == 3){
                                int level;
                                for(t = pilha->topo; t!= NULL; t = t->prox){
                                    if(t->imagem1 == NULL){
                                        level = t->level;
                                        break;
                                    }
                                }
                                for(int i = 0;i<ordemDaMatriz*ordemDaMatriz-qntCaminho;i++){
                                    if((x>localNaoCaminho[i].x && x<localNaoCaminho[i].x+torreNoMouse->w //Se o clique do mouse estiver dentro do nao caminho
                                    && y>localNaoCaminho[i].y && y<localNaoCaminho[i].y+torreNoMouse->h) //Se o clique do mouse estiver dentro do nao caminho
                                    ){
                                        for(t = pilha->topo; t!= NULL; t = t->prox){
                                            if(t->imagem1 != NULL  /*&& t->level == level*/){ //Percorre pelas torres com imagem nao nula
                                                if(event.button.x > t->coord.x && event.button.x < t->coord.x+t->imagem1->w//Se o clique do mouse for em cima de uma torre (Upa a torre)
                                                && event.button.y > t->coord.y && event.button.y < t->coord.y+t->imagem1->h//Se o clique do mouse for em cima de uma torre (Upa a torre)
                                                ){//Upa a torre
                                                    if(t->level == level){
                                                        teste = true;
                                                        switch(t->level){
                                                            case 1:
                                                                t->level = 2;
                                                                t->imagem1 = SDL_LoadBMP("torre4Peq.bmp");
                                                                teste = false;
                                                                torre_no_mouse = 2;
                                                                dinheiro -= 50;
                                                                break;
                                                            case 2:
                                                                t->level = 3;
                                                                t->imagem1 = SDL_LoadBMP("torre8Peq.bmp");
                                                                teste = false;
                                                                torre_no_mouse = 2;
                                                                dinheiro -= 50;
                                                                break;
                                                            case 3:
                                                                t->level = 4;
                                                                t->imagem1 = SDL_LoadBMP("torre2.bmp");
                                                                teste = false;
                                                                torre_no_mouse = 2;
                                                                dinheiro -= 50;
                                                                break;
                                                            default:
                                                                t->imagem1 = SDL_LoadBMP("inimigo.bmp");
                                                                break;
                                                        }
                                                        retiraNula(pilha);
                                                    }
                                                    else
                                                        teste = false;
                                                }
                                            }
                                        }
                                        //Se o clique do mouse nao for em cima de outra torre (Move a torre ja existente)
                                       for(t = pilha->topo; t!= NULL; t = t->prox){
                                           if(t->imagem1 == NULL && teste){ //Quando encontrar alguma torre com imagem nula
                                               //Ajusta o X e Y da torre para encaixar corretamente, fora do menu
                                               int auxX = (event.button.x - inicio_do_quadriculadoX)/torreNoMouse->w;
                                               t->coord.x = inicio_do_quadriculadoX + torreNoMouse->w * auxX;
                                               int auxY = (event.button.y - inicio_do_quadriculadoY)/torreNoMouse->h;
                                               t->coord.y = inicio_do_quadriculadoY + torreNoMouse->h * auxY;
                                               t->tiroCoord.x = t->coord.x + torreNoMouse->w/2;
                                               t->tiroCoord.y = t->coord.y + torreNoMouse->h/2;
                                               switch(t->level){
                                                   case 1:
                                                       t->imagem1 = SDL_LoadBMP("torre2Peq.bmp");
                                                       t->tiroImagem = SDL_LoadBMP("bala.bmp");
                                                       break;
                                                   case 2:
                                                       t->imagem1 = SDL_LoadBMP("torre4Peq.bmp");
                                                       t->tiroImagem = SDL_LoadBMP("bala.bmp");
                                                       break;
                                                   case 3:
                                                       t->imagem1 = SDL_LoadBMP("torre8Peq.bmp");
                                                       t->tiroImagem = SDL_LoadBMP("bala.bmp");
                                                       break;
                                                   default:
                                                       t->imagem1 = SDL_LoadBMP("torre2.bmp");
                                                       break;
                                               }
                                               torre_no_mouse = 2;
                                               break;
                                           }
                                       }
                                       break;
                                    }
                                }
                           }
                           if(torre_no_mouse == 0){
                               for(Torre *t1 = pilha->topo; t1!= NULL; t1 = t1->prox){
                                   //Se o mouse estiver em cima de alguma torre, fora do menu
                                   if(t1->imagem1 != NULL){
                                       if(event.button.x > t1->coord.x && event.button.x < t1->coord.x+t1->imagem1->w
                                       && event.button.y > t1->coord.y && event.button.y < t1->coord.y+t1->imagem1->h
                                       ){
                                           switch(t1->level){
                                               case 1:
                                                   torreNoMouse = SDL_LoadBMP("torre2Peq.bmp");
                                                   break;
                                               case 2:
                                                   torreNoMouse = SDL_LoadBMP("torre4Peq.bmp");
                                                   break;
                                               case 3:
                                                   torreNoMouse = SDL_LoadBMP("torre8Peq.bmp");
                                                   break;
                                           }
                                           torreMouse_destino.x = event.button.x -(torreNoMouse->w/2);
                                           torreMouse_destino.y = event.button.y -(torreNoMouse->h/2);
                                           t1->imagem1 = NULL;
                                           t1->tiroImagem = NULL;
                                           torre_no_mouse = 3;
                                           //printf("t=0, fora, posX: %d, mouseX: %d ", t1->coord.x, event.button.x);
                                           //printf("t=0, fora, posY: %d, mouseY: %d\n", t1->coord.y, event.button.y);
                                       }
                                   }
                               }
                                 //printf("t=0 fora \n");
                                 break;
                           }
                       }
                       //dentro do menu
                       if(x>torreMenu_destino.x
                       && x<(torreMenu_destino.x+torreMenu->w)
                       && y>torreMenu_destino.y
                       && y<(torreMenu_destino.y+torreMenu->h)
                       && torre_no_mouse == 0
                       //&& dinheiro >= 50
                       ){
                           //printf("t=0: dentro\n");
                           torreMouse_destino.x = event.button.x -(torreNoMouse->w/2);
                           torreMouse_destino.y = event.button.y -(torreNoMouse->h/2);
                           torreNoMouse = SDL_LoadBMP("torre2Peq.bmp");
                           torre_no_mouse = 1;
                       }
                       break;
                   }
               }
               case SDL_MOUSEBUTTONUP:{
                   // estou tratando o caso do mouse up
                   if(event.button.button==SDL_BUTTON_LEFT && torre_no_mouse == 2){
                       //printf("t=2 mouseUP\n");
                       torre_no_mouse = 0;
                   }
                break;
                }
                case SDL_QUIT: //fechar a janela?
                    JogoAtivo = 0; //sair do loop principal
                    break;
                default://Evento n?o tratado
                    printf("Evento nao tratado!\n");
            }
        }

        // Desenha a tela
        if(tela == 2){
            desenha(pilha, listaInimigo);

            if(!caminhoCriado){
                criaCaminho();
                pegaNaoCaminho();
            }
        }
        else if(tela == 0){
            pilha = criaPilha();
            listaInimigo = criaLista();
            dinheiro = 150;
            vida = 100;
            SDL_FillRect(screen, NULL, 0x000000);
            //stringColor(screen, comprimento/2 -150, altura/2, "Aperte algum botao do teclado para iniciar o jogo",0xFFFFFFFF);
            SDL_BlitSurface(paginaInicial, NULL, screen, &paginaInicial_destino);
            SDL_Flip(screen); //atualizar a tela
        }
        else if(tela == 1){
            SDL_FillRect(screen, NULL, 0x000000);
            //stringColor(screen, comprimento/2 -150, altura/2, "Aperte algum botao do teclado para iniciar o jogo",0xFFFFFFFF);
            SDL_BlitSurface(tutorial, NULL, screen, &paginaInicial_destino);
            SDL_Flip(screen); //atualizar a tela
        }
        else if(tela ==3){
            SDL_FillRect(screen, NULL, 0x000000);
            //stringColor(screen, comprimento/2 -150, altura/2, "Aperte algum botao do teclado para iniciar o jogo",0xFFFFFFFF);
            SDL_BlitSurface(gameOver, NULL, screen, &paginaInicial_destino);
            SDL_Flip(screen); //atualizar a tela
        }
        else if(tela ==4){
            SDL_FillRect(screen, NULL, 0x000000);
            //stringColor(screen, comprimento/2 -150, altura/2, "Aperte algum botao do teclado para iniciar o jogo",0xFFFFFFFF);
            SDL_BlitSurface(parabens, NULL, screen, &paginaInicial_destino);
            SDL_Flip(screen); //atualizar a tela
        }
        //FPS
        contador_tempo++;
        contador_inimigo++;
        if(vida <= 0)
            tela = 3;
        if(ganhou >= 3)
            tela = 4;
        SDL_Delay(16);
    }

    SDL_Quit(); //encerrar a SDL
    return 0;
}
// Fun??o desenha os objetos na tela
void desenha(PilhaTorres *p, ListaInimigos *l){
    char bufferGrana[5];
    char bufferVida[5];
    itoa(dinheiro, bufferGrana, 10);
    itoa(vida, bufferVida, 10);
    // Pinta a janela de branco (limpa tela)
    SDL_FillRect(screen, NULL, 0x005000);

    //Cria Imagem do Menu
    SDL_BlitSurface(menu, 0, screen, &menu_destino);
    //Desenha a torre do menu
    SDL_BlitSurface(torreMenu, NULL, screen, &torreMenu_destino);
    //Desenha o botao de reiniciar
    SDL_BlitSurface(reiniciar, NULL, screen, &reiniciar_destino);
    //Desenha o botao de iniciar
    SDL_BlitSurface(iniciar, NULL, screen, &iniciar_destino);
    //Nome do jogo, na tela
    stringColor(screen, COMPRIMENTO/2, 5, "Tower Defense -- 2048",0xFFFFFFFF);
    stringColor(screen, 10, 20, "Vida: ",0xFFFFFFFF);
    stringColor(screen, 60, 20, bufferVida,0xFFFFFFFF);
    stringColor(screen, 10, 50, "Grana: ",0xFFFFFFFF);
    stringColor(screen, 60, 50, bufferGrana,0xFFFFFFFF);
    stringColor(screen, /*coordenadaX*/50, 250, bufferVidaInimigo,0xFFFFFFFF);
    //Mostra as moedas
    //stringColor(screen, comprimento/2, 5,  ,0xFFFFFFFF);
    qntCaminho = 0;
     for(int i=0;i<ordemDaMatriz;i++){
        for(int j=0;j<ordemDaMatriz;j++){
            if(matriz[i][j]->caminho == 0)
                SDL_BlitSurface(naoCaminho, 0, screen, &localCenario[i][j]);
            if(matriz[i][j]->caminho == 1){
                SDL_BlitSurface(caminho, 0, screen, &localCenario[i][j]);
                qntCaminho++;
            }
            if(matriz[i][j]->caminho == 8){
                portal_destino.x = localCenario[i][j].x;
                portal_destino.y = localCenario[i][j].y;
                SDL_BlitSurface(portal, 0, screen, &localCenario[i][j]);
                qntCaminho++;
            }
            if(matriz[i][j]->caminho == 9){
                SDL_BlitSurface(castelo, 0, screen, &localCenario[i][j]);
                qntCaminho++;
            }
        }
    }
        //move uma torre existente ou compra uma nova torre
        if(torre_no_mouse == 3 || torre_no_mouse == 1){
            SDL_BlitSurface(torreNoMouse, NULL, screen, &torreMouse_destino);
        }
            /*printf("%d\n", t->id);*/
            imprimeTorres(p);
            imprimeTiros(p);
            //adicionaInimigo(listaInimigo);
            addInimigoTela(listaInimigo);
            moveInimigos(listaInimigo);
            imprimeInimigos(listaInimigo);
            if(contador_tempo >= frequencia_tiro)
                meteBala(p, listaInimigo);

   SDL_Flip(screen); //atualizar a tela
}

/**********************************************************  Tratamento do cenario **********************************************************/
//Verifica a ordem da matriz, que eh o primeiro numero do arquivo .txt
int pegaOrdemDaMatriz(){
   int a;
   ifstream myfile;
   myfile.open ("dados.txt");
   myfile>>a;
   myfile.close();
   return a;
}
//Cria a Matriz nXn, que eh o game
Cenario ***criaMatriz() {
   Cenario ***mat;
   int lixo;
   ifstream myfile;
   myfile.open ("dados.txt");
   // Descarta esse numero
   myfile>>lixo;
   // aloca as linhas
   mat = new Cenario**[ordemDaMatriz];
   // aloca as colunas nas linhas
   for(int i=0;i<ordemDaMatriz;i++)
       mat[i]=new Cenario*[ordemDaMatriz];
   //le a matriz
   for(int i=0;i<ordemDaMatriz;i++)
       for(int j=0;j<ordemDaMatriz;j++){
           mat[j][i] = new Cenario();
           myfile>>mat[j][i]->caminho;
       }
   return mat;
}

SDL_Rect *criaCaminho() {
    int iInicial=0, jInicial=0;
    localCaminho = new SDL_Rect[qntCaminho];
    int aux = 0;

    for(int i = 0; i<ordemDaMatriz;i++){
        for(int j=0;j<ordemDaMatriz;j++){
            if(matriz[i][j]->caminho == 8){
                matriz[i][j]->usada = 1;
                localCaminho[aux].x = localCenario[i][j].x;
                localCaminho[aux].y = localCenario[i][j].y;
                iInicial = i;
                jInicial = j;
                aux++;
            }
        }
    }

    for(int i = 0; i<qntCaminho;i++){
        //Se o caminho estiver a direita do numero
        if(iInicial < ordemDaMatriz-1){
            if((matriz[iInicial+1][jInicial]->caminho == 1 || matriz[iInicial+1][jInicial]->caminho == 9) && matriz[iInicial+1][jInicial]->usada == 0){
                iInicial++;
                localCaminho[aux].x = localCenario[iInicial][jInicial].x;
                localCaminho[aux].y = localCenario[iInicial][jInicial].y;
                matriz[iInicial][jInicial]->usada = 1;
                aux++;
                continue;
            }
        }
        //Se o caminho estiver a esquerda do numero
        if(iInicial > 0){
            if((matriz[iInicial-1][jInicial]->caminho == 1 || matriz[iInicial-1][jInicial]->caminho == 9) && matriz[iInicial-1][jInicial]->usada == 0){
                iInicial--;
                localCaminho[aux].x = localCenario[iInicial][jInicial].x;
                localCaminho[aux].y = localCenario[iInicial][jInicial].y;
                matriz[iInicial][jInicial]->usada = 1;
                aux++;
                continue;
            }
        }
        //Se o caminho estiver acima do numero
        if(jInicial > 0){
            if((matriz[iInicial][jInicial-1]->caminho == 1 || matriz[iInicial][jInicial-1]->caminho == 9) && matriz[iInicial][jInicial-1]->usada == 0){
                jInicial--;
                localCaminho[aux].x = localCenario[iInicial][jInicial].x;
                localCaminho[aux].y = localCenario[iInicial][jInicial].y;
                matriz[iInicial][jInicial]->usada = 1;
                aux++;
                continue;
            }
        }
        //Se o caminho estiver abaixo do numero
        if(jInicial < ordemDaMatriz-1){
            if((matriz[iInicial][jInicial+1]->caminho == 1 || matriz[iInicial][jInicial+1]->caminho == 9) && matriz[iInicial][jInicial+1]->usada == 0){
                jInicial++;
                localCaminho[aux].x = localCenario[iInicial][jInicial].x;
                localCaminho[aux].y = localCenario[iInicial][jInicial].y;
                matriz[iInicial][jInicial]->usada = 1;
                aux++;
                continue;
            }
        }
    }
    return localCaminho;
}

SDL_Rect *pegaNaoCaminho(){
    int aux = 0;
    localNaoCaminho = new SDL_Rect[ordemDaMatriz*ordemDaMatriz/*-qntCaminho*/];
    for(int i=0;i<ordemDaMatriz;i++){
        for(int j=0;j<ordemDaMatriz;j++){
            if(matriz[i][j]->caminho != 1){
                localNaoCaminho[aux].x = localCenario[i][j].x;
                localNaoCaminho[aux].y = localCenario[i][j].y;
                aux++;
                //continue;
            }
        }
    }
    printf("AUX: %d \n", aux);
    caminhoCriado = true;
    return localNaoCaminho;
}

/**********************************************************  Tratamento das torres **********************************************************/
PilhaTorres *criaPilha(){
    PilhaTorres *p = new PilhaTorres;
    p->topo = NULL;
    return p;
}

Torre *retira(PilhaTorres *p){
   if (p == NULL)
       return NULL;
   Torre *aux;
   aux = p->topo;
   SDL_FreeSurface(p->topo->imagem1);
   p->topo = aux->prox;
   return aux;

   //delete aux;
}

void libera(PilhaTorres *p){
   Torre *t, *temp;
   t = p->topo;
   while(t!=NULL){
       temp = t->prox;
       delete t;
       t = temp;
   }
   delete p;
}

void trocaRetiraTorre(PilhaTorres *p, Torre *e){
   Torre *anterior, *t;
   anterior = NULL;
   t = p->topo;
   while(t->id != e->id && t != NULL){
       anterior = t;
       t = t->prox;
   }
   if(anterior != NULL){
       anterior->prox = t->prox;
       torre_no_mouse = 0;
       delete t;
   }
   else{
       p->topo = t->prox;
       torre_no_mouse = 0;
       delete t;
   }
}

void retiraNula(PilhaTorres *p){
    Torre *t;
    t = p->topo;
    while(t->imagem1 != NULL){
        t = t->prox;
    }
    trocaRetiraTorre(p, t);
    teste = false;
}

//Insere uma torre na pilha de torres
void insere(PilhaTorres *p, SDL_Rect rect){
    int auxX, auxY;
    Torre *t = new Torre;
    //Tiro *ti;
    t->tiroImagem = SDL_LoadBMP("bala.bmp");
    t->imagem1 = SDL_LoadBMP("torre2Peq.bmp");
    t->coord = rect;
    t->prox = p->topo;
    p->topo = t;
    t->level = 1;
    t->id = idTorres;
    t->usandoTiro = false;
    t->tiro = criaTiro();
    idTorres++;

    //Ajusta o X e Y da torre para encaixar corretamente
    auxX = (t->coord.x - inicio_do_quadriculadoX)/t->imagem1->w;
    t->coord.x = inicio_do_quadriculadoX + t->imagem1->w * auxX;
    auxY = (t->coord.y - inicio_do_quadriculadoY)/t->imagem1->h;
    t->coord.y = inicio_do_quadriculadoY + t->imagem1->h * auxY;

    t->tiroCoord.x = t->coord.x + t->imagem1->w/2;
    t->tiroCoord.y = t->coord.y + t->imagem1->h/2;

    //Verifica se ja existe alguma torre nessa posicao
    for(Torre *t1 = p->topo->prox; t1!= NULL; t1 = t1->prox){
        if(t1->coord.x == t->coord.x && t1->coord.y == t->coord.y){
            t1->level = 2;
            t1->imagem1 = SDL_LoadBMP("torre4Peq.bmp");
            trocaRetiraTorre(p, t);
            //torre_no_mouse = 0;
        }
    }
}

//Imprime as torres em suas posicoes corretas
void imprimeTorres(PilhaTorres *p){
    Torre *t;
    for(t = p->topo; t!= NULL; t = t->prox)
        SDL_BlitSurface(t->imagem1, NULL, screen, &t->coord);
}

/**********************************************************  Tratamento dos tiros **********************************************************/

void meteBala(PilhaTorres *p, ListaInimigos *l){
    double difX;
    Torre *t;
    Inimigo *i;
    double coeficiente_angular = 0;
    for(t = p->topo; t!= NULL; t = t->prox){ //Percorre todas as torres
        //if(t->tiroImagem != NULL){ //Se o tiro da imagem nao for nulo
            if(t->tiroCoord.x < t->coord.x+80 && t->tiroCoord.x > t->coord.x -80
               && t->tiroCoord.y < t->coord.y+80 && t->tiroCoord.y > t->coord.y -80){  //Se o tiro nao passar de 100 pixel
                for(i = l->inicio; i !=NULL; i = i->prox){ //Percorre todos os inimigos
                   // if(i->prox == NULL){
                        if(i->coord.x < t->coord.x+80 && i->coord.x > t->coord.x-80     //Se algum inimigo estiver a 80 pixel acima ou abaixo da torre
                        && i->coord.y < t->coord.y+80 && i->coord.y > t->coord.y-80  //Se algum inimigo estiver a 80 pixel a esquerda ou direita da torre
                           ){

                            if(t->tiroCoord.x-5 - i->coord.x)
                                coeficiente_angular = (t->tiroCoord.y-5 - i->coord.y)/(t->tiroCoord.x-5 - i->coord.x);

                            if(t->tiroCoord.x + t->imagem1->w > i->coord.x &&
                               t->tiroCoord.x < i->coord.x + i->imagem->w && //Se o tiro colidir com o inimigo
                               t->tiroCoord.y + t->imagem1->h > i->coord.y &&
                               t->tiroCoord.y < i->coord.y + i->imagem->h //Se o tiro colidir com o inimigo
                            ){
                                t->tiroCoord.x = t->coord.x + t->imagem1->w/2; //Retorna o tiro para a torre
                                t->tiroCoord.y = t->coord.y + t->imagem1->h/2;
                                if(t->level == 1)
                                    i->vida -= 50;
                                if(t->level == 2)
                                    i->vida -= 100;
                                if(t->level == 3)
                                    i->vida -= 150;
                                itoa(i->vida, bufferVidaInimigo, 10);
                                contador_tempo = 0;
                                if(i->vida <= 0){
                                    retiraInimigo(listaInimigo, i);
                                    dinheiro += 50;
                                    ganhou++;
                                }
                                continue;
                            }
                            difX = i->coord.x - t->tiroCoord.x ;
                            //difY = i->coord.y - t->tiroCoord.y;


                            coordenadaX = i->coord.x;
                            coordenadaY = i->coord.y + 20;
                            if(difX > 0){
                                t->tiroCoord.x += 2;
                                t->tiroCoord.y += coeficiente_angular*2;
                            }
                            if(difX < 0){
                                t->tiroCoord.x -= 2;
                                t->tiroCoord.y -= coeficiente_angular*2;

                            }
                            //stringColor(screen, i->coord.x, i->coord.y +20, bufferVidaInimigo,0xFFFFFFFF);
                        }
                   // }
                }
            }
            else{ //Se o tiro passar de 100 pixel de distancia
                coeficiente_angular = 0;
                t->tiroCoord.x = t->coord.x + t->imagem1->w/2; //Retorna o tiro para a torre
                t->tiroCoord.y = t->coord.y + t->imagem1->h/2;
                t->tiroImagem = SDL_LoadBMP("bala.bmp");
            }
        //}
    }
}

void imprimeTiros(PilhaTorres *p){
    Torre *t;
    for(t = p->topo; t!= NULL; t = t->prox){ //Percorre todas as torres
        if(t->tiroImagem != NULL){
            SDL_BlitSurface(t->tiroImagem, NULL, screen, &t->tiroCoord); //Imprime o tiro
        }
    }
}

Tiro *criaTiro(){
    Tiro *t = new Tiro();
    t->imagem = SDL_LoadBMP("bala.bmp");
    t->level = 1;
    t->velocidade = 1;
    return t;
}


/**********************************************************  Tratamento dos inimigos **********************************************************/
ListaInimigos *criaLista(){
    ListaInimigos *l = new ListaInimigos;
    l->inicio = NULL;
    return l;
}

void retiraInimigo(ListaInimigos *l, Inimigo *i){
    Inimigo *anterior, *t;
    anterior = NULL;
    t = l->inicio;
    while(t->id != i->id && t != NULL){
       anterior = t;
       t = t->prox;
   }
   if(anterior != NULL){
       anterior->prox = t->prox;
       torre_no_mouse = 0;
       delete t;
   }
   else{
       l->inicio = t->prox;
       torre_no_mouse = 0;
       delete t;
   }
}

void adicionaInimigo(ListaInimigos *l){
   Inimigo *t = new Inimigo;
   //Tiro *ti;
   t->imagem = SDL_LoadBMP("inimigo.bmp");
   t->id = idInimigo;
   t->coord.x = portal_destino.x + torreNoMouse->w/2 - t->imagem->w/2;
   t->coord.y = portal_destino.y + torreNoMouse->h/2 - t->imagem->h/2;
   t->prox = l->inicio;
   t->posicaoLocalCaminho = 1;
   t->vida = 150;
   l->inicio = t;
   idInimigo++;
}

void addInimigoTela(ListaInimigos *l){
    if(contador_inimigo >= 35){
        if(insere_inimigo && qntInimigos < 3){
            adicionaInimigo(l);
            qntInimigos++;
            contador_inimigo = 0;
        }
    }
    if(qntInimigos >= 3){
        insere_inimigo = false;
        qntInimigos = 0;
    }

}

void imprimeInimigos(ListaInimigos *i){
    Inimigo *t;
    for(t = i->inicio; t!= NULL; t = t->prox){
        SDL_BlitSurface(t->imagem, NULL, screen, &t->coord);
        //t->coord.x++;
    }
}

void moveInimigos(ListaInimigos *i){
    Inimigo *t;
    for(t = i->inicio; t!= NULL; t = t->prox){
        if(t->coord.x < localCaminho[t->posicaoLocalCaminho].x + torreNoMouse->w/2 - t->imagem->w/2){
            t->coord.x++;
            continue;
        }
        if(t->coord.x > localCaminho[t->posicaoLocalCaminho].x+ torreNoMouse->w/2 - t->imagem->w/2){
            t->coord.x--;
            continue;
        }
        if(t->coord.y < localCaminho[t->posicaoLocalCaminho].y+ torreNoMouse->w/2 - t->imagem->w/2){
            t->coord.y++;
            continue;
        }
        if(t->coord.y > localCaminho[t->posicaoLocalCaminho].y+ torreNoMouse->w/2 - t->imagem->w/2){
            t->coord.y--;
            continue;
        }
        if(t->posicaoLocalCaminho == qntCaminho-1){
            retiraInimigo(i, t);
            vida -= 50;
        }
        if(t->posicaoLocalCaminho < qntCaminho - 1)
            t->posicaoLocalCaminho++;
            //break;
    }
}


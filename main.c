// Tutorial original retirado do site :
// https://www-ohsat-com.translate.goog/tutorial/megarunner/megarunner-1/?_x_tr_sl=en&_x_tr_tl=pt&_x_tr_hl=pt-BR&_x_tr_pto=wapp
//Módificações adicionais por Vagno Silva.
#include <genesis.h>
#include <resources.h>
#include <string.h>

/*Definicões do Jogador*/
#define ANIM_RUN	0 //Define um indice para a animação de corrida
#define ANIM_JUMP	1 //Define um indice para a animação de pulo
Sprite* player; //Ponteiro para o sprite do jogador
const int player_x = 32; //Posição X inicial do jogador
fix16 player_y = FIX16(112); //Posição Y inicial do jogador
fix16 player_vel_y = FIX16(0); //variável usada para controlar o pulo do jogador
int player_height = 16; //Altura do jogador
const int floor_height = 128; //Altura do piso
fix16 gravity = FIX16(0.2); //Valor usado para definir a ação da gravidade durante o pulo
bool jumping = FALSE; //Define se o jogador está pulando

/*Definições do obstáculo*/
Sprite* obstacle; //Ponteiro para o sprite do obstáculo
int obstacle_x = 320; //Posição X inicial do obstáculo
int obstacle_vel_x = 0; //Velocidade inicial do obstáculo

/*Pontuação*/
int score = 0; //Variável que armazena a pontuação
char label_score[6] = "SCORE\0";  //Exibe a palavra score na tela
char str_score[3] = "0"; //armazena a pontuação em uma string para exibi-la na tela
bool score_added = FALSE; //evita que o score aumente mais que o necessário durante o pulo, se TRUE o score nao aumenta mais

/*Definições Gerais*/
int scrollspeed; // Velocidade de scrolling do plano de fundo
char msg_start[22] = "Press START to Begin!"; // Mensagem de início
char msg_reset[22] = "Press START to Reset!"; // Mensagem de fim do jogo
char msg_pause[22] = "Game Paused"; // Mensagem de pausa
bool game_on = FALSE; // Controla se o jogo está em andamento
bool game_paused = FALSE; // Controla se o jogo está pausado

/* Definição de dificuldade */
int difficulty_step = 10; // Aumenta a dificuldade a cada 10 pontos
int max_scrollspeed = 7; // Limita a velocidade máxima do scroll

// Função para exibir texto na tela
void showText(char s[]) {
    VDP_drawText(s, 20 - strlen(s) / 2, 10); // Centraliza o texto na tela
}

// Função para limpar o texto na tela
void clearText() {
    VDP_clearText(0, 10, 32); // Limpa a área de texto
}

//Função para converter o score em uma string(texto)
void updateScoreDisplay() {
    sprintf(str_score, "%d", score); // Converte o valor do score para string
    VDP_clearText(10, 2, 6); // Garante que qualquer texto residual seja limpo (assumindo que o score nunca terá mais de 6 dígitos)
    VDP_drawText(str_score, 10, 2); // Exibe o score na tela
}

// Função que inicia o jogo
void startGame() {
    if (!game_on) { // Só inicia se o jogo não estiver em andamento
        game_on = TRUE; // Marca o jogo como iniciado
        clearText(); // Limpa o texto anterior
        score = 0;//Reseta o score
        VDP_drawText(label_score,10,1); //Desenha a palavra score na tela
        updateScoreDisplay(); //Atualiza o placa do score antes de iniciar
        obstacle_x = 320; //define a posição inicial do obstáculo
        scrollspeed = 2;
    }
}

// Função que pausa o jogo
void pauseGame() {
    if (game_on && !game_paused) { // Só pausa se o jogo estiver em andamento e não estiver pausado
        game_paused = TRUE; // Marca o jogo como pausado
        clearText(); // Limpa o texto anterior
        showText(msg_pause); // Exibe a mensagem de pausa
    }
}

// Função que retoma o jogo
void resumeGame() {
    if (game_on && game_paused) { // Só retoma se o jogo estiver em andamento e pausado
        game_paused = FALSE; // Marca o jogo como não pausado
        clearText(); // Limpa o texto anterior
    }
}

// Atualiza a função endGame() para exibir a mensagem de reset
void endGame() {
    if (game_on) { // Só finaliza se o jogo estiver em andamento
        showText(msg_reset); // Exibe a mensagem de fim de jogo
        game_on = FALSE; // Marca o jogo como finalizado
        game_paused = TRUE; // Define o estado como pausado para permitir o reset
    }
}

void resetGame() {
    // Restaura as variáveis principais
    player_y = FIX16(112); // Volta o jogador à posição inicial
    player_vel_y = FIX16(0); // Reseta a velocidade do pulo
    jumping = FALSE; // Reseta o estado de pulo do jogador
    SPR_setAnim(player, ANIM_RUN); // Define a animação inicial do jogador

    obstacle_x = 320; // Volta o obstáculo à posição inicial

    score = 0; // Reseta o valor do score
    sprintf(str_score, "%d", score); // Atualiza a string do score para "0"
    updateScoreDisplay(); // Atualiza a exibição do score

    game_on = FALSE; // Reseta o estado do jogo
    game_paused = FALSE; // Reseta o estado de pausa

    // Mostra a mensagem inicial
    clearText();
    showText(msg_start);

    // Atualiza as posições dos sprites
    SPR_setPosition(player, player_x, fix16ToInt(player_y));
    SPR_setPosition(obstacle, obstacle_x, 120);

    // Reinicia o scroll do plano de fundo
    VDP_setHorizontalScroll(BG_B, 0);
}


// Atualiza o manipulador de eventos de controle
void myJoyHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) { // Verifica se o controle é o JOY_1 (primeiro controle)
        /* Start game if START is pressed */
        if (state & BUTTON_START) { // Verifica se o botão START foi pressionado
            if (!game_on) { // Se o jogo não estiver em andamento
                if (!game_paused) {
                    startGame(); // Inicia o jogo
                } else {
                    resetGame(); // Reseta o jogo
                }
            } else if (!game_paused) { // Se o jogo estiver em andamento e não pausado
                pauseGame(); // Pausa o jogo
            } else { // Se o jogo estiver pausado
                resumeGame(); // Retoma o jogo
            }
        }
    }

    if (state & BUTTON_C) { // Verifica se o botão C foi pressionado
        if (jumping == FALSE) { // Verifica se o jogador não está pulando
            jumping = TRUE; // Indica que o jogador está pulando
            player_vel_y = FIX16(-4); // Define a velocidade vertical do jogador, fazendo ele pular
            SPR_setAnim(player, ANIM_JUMP); // Define a animação de pulo do jogador
        }
    }
}

// Função para ajustar a dificuldade
void adjustDifficulty() {
    // Calcula o nível de dificuldade com base na pontuação
    int new_scrollspeed = 2 + (score / difficulty_step);

    // Limita a velocidade de rolagem ao máximo permitido
    if (new_scrollspeed > max_scrollspeed) {
        new_scrollspeed = max_scrollspeed;
    }

    // Atualiza a velocidade de rolagem se ela mudar
    if (new_scrollspeed != scrollspeed) {
        scrollspeed = new_scrollspeed;
    }
}

int main() {
    VDP_init(); // Inicializa o VDP (Video Display Processor)

    VDP_setPlaneSize(32, 32, FALSE); // Define o tamanho do plano de fundo
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE); // Define o tipo de scrolling (horizontal e vertical)

    // Carrega os tilesets para o piso, parede e poste
    VDP_loadTileSet(floor.tileset, 1, DMA);
    VDP_loadTileSet(wall.tileset, 2, DMA);
    VDP_loadTileSet(light.tileset, 3, DMA);

    // Define a paleta de cores 1 para o poste, o piso e a parede
    PAL_setPalette(PAL1, light.palette->data, DMA);
    // Define a paleta de cores 2 para o jogador(runner)
    PAL_setPalette(PAL2, runner.palette->data, DMA);

    // Desenha o piso, parede e poste no plano de fundo
    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), 0, 16, 32, 1);
    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, TRUE, 2), 0, 17, 32, 14);
    VDP_fillTileMapRectInc(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 3), 15, 13, 2, 3);

    // Define uma cor personalizada para o fundo
    u16 customColor = RGB24_TO_VDPCOLOR(0x6dc2ca); // Define a cor azul celeste
    PAL_setColor(0, customColor); // Define a cor no índice 0 da paleta

    // Define o índice 15 como cor de fundo
    VDP_setBackgroundColor(0);

    // Inicializa o controle e define o manipulador de eventos de botão
    JOY_init();
    JOY_setEventHandler(&myJoyHandler);

    showText(msg_start); // Exibe a mensagem inicial para pressionar START

    SPR_init();//Inicializa o controlador dos sprites

    player = SPR_addSprite(&runner,player_x,player_y,TILE_ATTR(PAL2,0,FALSE,FALSE));//Inicializa os sprite do jogador
    SPR_setAnim(player,ANIM_RUN);//define a animação de corrida como animação inicial do sprite

    obstacle = SPR_addSprite(&rock,obstacle_x,128,TILE_ATTR(PAL2,0,FALSE,FALSE));//Inicializa os sprite do obstáculo

    //Variáveis
    int offset = 0; // Armazena o deslocamento do scroll do plano de fundo


    // Loop principal do jogo
    while (1) {
        // Se o jogo estiver em andamento e não estiver pausado, faz o scroll do plano de fundo
        if (game_on && !game_paused) {

            adjustDifficulty(); //Ajusta a dificultade a cada 10 pontos aumentando a velocidade de movimento do obstáculo

            VDP_setHorizontalScroll(BG_B, offset -= scrollspeed); // Faz um scroll da direita para a esquerda

            player_y = player_y + player_vel_y;//Efetua a ação de pulo do jogador
            SPR_setPosition(player,player_x,fix16ToInt(player_y));//Atualiza a posição horizontal e vertical do sprite do jogador
            if(jumping == TRUE){ player_vel_y = player_vel_y + gravity;}//Aplica a  ação da gravidade durante o pulo

            //Verifica se o jogador tocou no solo após o pulo
            if(jumping == TRUE && fix16ToInt(player_y)+player_height >= (floor_height)){//verifica se a posição y do jogador e maior ou igual à altura do solo
            jumping = FALSE; //Define o pulo como inativo
            player_vel_y = FIX16(0);//define novamente a velocidade do pulo para 0 para que nao continue caindo após o pulo
            player_y = intToFix16(floor_height-player_height);//posiciona o jogador no solo caso o sprite nao fique alinhado com o terreno
            score_added = FALSE;//Libera o score para receber novo incremento
            SPR_setAnim(player,ANIM_RUN);//define novamengte a animação do jogador como a de corrida
            }


            //Move o obstáculo
            obstacle_vel_x = -scrollspeed;//Define o scroll do obstáculo da direita para a esquerda subtraindo 1 de sua posição X
            obstacle_x = obstacle_x + obstacle_vel_x;//subtrai 1 de sua posição x a cada ciclo
            if(obstacle_x < -8) obstacle_x = 320;//Reseta sua posição inicial quando sua posição x for inferior a -8
            SPR_setPosition(obstacle,obstacle_x,120);//Atualiza a posição horizontal e verticado do sprite do obstáculo

            //Colisão e Placar
            if(player_x < obstacle_x + 8 && player_x + 8 > obstacle_x){//verifica se o jogador encostou no obstáculo
                if(jumping == FALSE){ //verifica se ele está no solo
                    endGame();//encerra o jogo
                 } else{
                   if(score_added == FALSE){ //verifica se nao foi adicionado mais de 1 ponto durante o pulo
                    score++;//Acrescenta um valor de +1 ao score
                    updateScoreDisplay();//atualiza o score
                    score_added = TRUE;//evita que o score aumente mais que o necessário durante o pulo
                        }
                 }
            }

            SPR_update();
        }

        //Atualiza os sprites na tela
        SYS_doVBlankProcess(); // Processa a VBlank (necessário para o funcionamento do VDP)
    }

    return 0; // Retorna do main (não será atingido devido ao loop infinito)
}

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/bootrom.h"

/* Definição dos pinos dos perifericos */
// LED RGB
const uint8_t LED_R=13, LED_G=11, LED_B= 12;
// BOTÕES
const uint8_t BOTAO_A=5,BOTAO_B=6;

//variavéis de controle do temporizador
static uint contador;       //contador de repetições do alarme
static bool ativar_timer;  // flag de controle da ativação do timer
static bool timer_ativo=false;  // flag de controle da ativação do timer
// Variavél para registro de tempo e controle de bounce da interrupção
static volatile uint32_t tempo_anterior = 0;

// protótipos de funções
void inicializar_leds();
void inicializar_botoes();
void set_rgb(char cor);
static void gpio_irq_handler(uint gpio, uint32_t events);
int64_t turn_off_callback(alarm_id_t id, void *user_data);



int main()
{
    stdio_init_all();   //ativa entrada e saída padrão
    // configura perifericos
    inicializar_botoes();
    inicializar_leds();

    // configura as interupções
    gpio_set_irq_enabled_with_callback(BOTAO_A,GPIO_IRQ_EDGE_FALL,true,&gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B,GPIO_IRQ_EDGE_FALL,true,&gpio_irq_handler);

    ativar_timer= false;
    while (true) {
        // se a flag foi ativa
        printf("Flags\n");
        printf("Botao:%d\nalarme:%d\nContador:%d\n",ativar_timer,timer_ativo,contador);
        printf("------------------------------------------\n");
        // libera ativação de um alrme se o botão foi pressionado e não está com um alarme ativo
        if(ativar_timer && !timer_ativo){
            // atualiza flag do alarme
            timer_ativo=true;
            // limpa flag do botão
            ativar_timer = false;
            // seta contador em 2 para que ocorram 3 execuções
            contador=2;
            // ativa todos os leds
            set_rgb('W');
            // configura alarme para 3s
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
        }
        // delay para exibição das flags
        sleep_ms(1000);
    }
}


/*
|   Função set_rgb
|   Ativa a cor rgb de acordo ao parâmetro cor
|   char cor: 'R'= vermelho 'G' = verde 'B' = azul 'W' = branco 'Y'= amarelo -' apagar  
*/
void set_rgb(char cor){
    switch (cor)
    {
    case 'R':
        gpio_put(LED_R,1);
        gpio_put(LED_G,0);
        gpio_put(LED_B,0);
        break;
    case 'G':
        gpio_put(LED_R,0);
        gpio_put(LED_G,1);
        gpio_put(LED_B,0);
        break;
    case 'B':
        gpio_put(LED_R,0);
        gpio_put(LED_G,0);
        gpio_put(LED_B,1);
        break;
    case 'W':
        gpio_put(LED_R,1);
        gpio_put(LED_G,1);
        gpio_put(LED_B,1);
        break;
    case 'Y':
        gpio_put(LED_R,1);
        gpio_put(LED_G,0);
        gpio_put(LED_B,1);
        break;
    case '-':
        gpio_put(LED_R,0);
        gpio_put(LED_G,0);
        gpio_put(LED_B,0);
        break;
    default:
        printf("Caractere invalido!!\n informe R,G,B,W,Y ou -\n");
        break;
    }
}


/*
|   Função inicializar_leds
|   Configura os pinos da LED RGB
*/
void inicializar_leds(){
    // led vermelha
    gpio_init(LED_R);
    gpio_set_dir(LED_R,GPIO_OUT);
    // led verde
    gpio_init(LED_G);
    gpio_set_dir(LED_G,GPIO_OUT);
    // led azul
    gpio_init(LED_B);
    gpio_set_dir(LED_B,GPIO_OUT);
}


void inicializar_botoes(){
    //botão A para ativar a temporização
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A,GPIO_IN);
    gpio_pull_up(BOTAO_A);
    //botão B para ativar modo bootsel
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B,GPIO_IN);
    gpio_pull_up(BOTAO_B);
}

/*
|   Função de callback do tratamento de interrupções
|   Implementa um deboucer utilizando temporizador de sistema.
|   Faz o tratamento das interrupções dos botões A e B
*/
static void gpio_irq_handler(uint gpio, uint32_t events){
    // obtém tempo absoluto do instante atual
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());
    // passando um intervalo de 200 ms o bounce estabiliza
    if(tempo_atual- tempo_anterior >  200000){
        tempo_anterior= tempo_atual;
        if(gpio == BOTAO_A && !timer_ativo){
            // atualiza flag do temporizador
            ativar_timer = true;
        }
        else{
            printf("Modo bootsel");
            reset_usb_boot(0,0);
        }
    }
}

/* 
|   Função de callback para desligar o LED após o tempo programado.
|   O retorno da função define se o alarme vai rearmado ou não
|   Se o retorno for 0 o alarme não é rearmado
|   Com um retorno em valor maior que zero, o alarme é rearmado com o tempo
|   passado pelo retorno
|*/
int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    // Valor padrão para retorno da função
    // faz com que um novo alarme seja acionado em 3 segundos
    int64_t repetir = 3000000;

    // veririfca qual a proxima condição do contador
    switch (contador)
    {
    case 2:
        // com o contador em 2, apaga uma led e deixa duas acesas
        printf("Primeiro alarme\n");
        contador--;
        //printf("Contador em:%d",contador);
        set_rgb('Y');
        break;
    case 1:
        // com o contador em 1, apaga uma led e deixa uma acesas
        printf("Segundo alarme alarme\n");
        contador--;
        //printf("Contador em:%d\n",contador);
        set_rgb('R');
        break;
    case 0:
        printf("Ultimo alarme\n");
        //printf("Contador em:%d\n",contador);

        // com o contador em 0, apaga todas as leds e encerra a repetição do alarme
        set_rgb('-');
        repetir= 0;
        // atualiza flag do alarme liberando para novas ativações
        timer_ativo= false;
        break;
    default:
        break;
    }
    return repetir;
}
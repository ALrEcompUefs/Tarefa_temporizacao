#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/timer.h"

/* Definição dos pinos dos perifericos */
// LED RGB
const uint8_t LED_R=13, LED_G=11, LED_B= 12;
// BOTÕES
const uint8_t BOTAO_A=5;

// variaveis de controle da led
static char led_ativa;

// protótipos de funções
void inicializar_leds();
void inicializar_botoes();
void set_rgb(char cor);
static void gpio_irq_handler(uint gpio, uint32_t events);
bool repeating_timer_callback(struct repeating_timer *t);

int main()
{
    stdio_init_all();
    inicializar_leds();
    inicializar_botoes();
    // ativa primeiro a led vermelha
    led_ativa = 'R';
    // configura interrupção para o botão A
    gpio_set_irq_enabled_with_callback(BOTAO_A,GPIO_IRQ_EDGE_FALL,true,&gpio_irq_handler);

    // estutura para o temporizador
    struct repeating_timer timer;
    // Configura temporizador de 3s passando o endereço da estrutura timer e a função de callback
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);
    set_rgb(led_ativa);

    while (true) {
        printf("Nova iteração\n");
        sleep_ms(1000);
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
    //botão A
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A,GPIO_IN);
    gpio_pull_up(BOTAO_A);
}

/*
|   Função set_rgb
|   Ativa a cor rgb de acordo ao parâmetro cor
|   char cor: 'R'= vermelho 'G' = verde 'B' = azul 'W' = branco 'Y' = amarelo '-' apagar  
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
        gpio_put(LED_G,1);
        gpio_put(LED_B,0);
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

static void gpio_irq_handler(uint gpio, uint32_t events){
    // Debouncer não é implementado já que o programa vai entrar em modo boot_sel
    printf("Bootsel ativado\n");
    reset_usb_boot(0,0);
}
/*
|   Função repeating_timer_callback
|   Função de callback que é chamada repetidamento pelo contado
|
*/
bool repeating_timer_callback(struct repeating_timer *t) {
    // informa que três segundos se passaram
    printf("3 segundos passaram\n");

    //atualiza o estado da led rgb
    // Dado o valor atual do led ele atualiza para proxima cor do semáforo
    switch (led_ativa)
    {
    case 'R':
        led_ativa='Y';
        //led_ativa='B';
        set_rgb(led_ativa);
        break;
    case 'B':
        led_ativa= 'G';
        set_rgb(led_ativa);
        break;
    case 'Y':
        led_ativa= 'G';
        set_rgb(led_ativa);
        break;
    case 'G':
        led_ativa= 'R';
        set_rgb(led_ativa);
        break;
    default:
        break;
    }
    // Retorna true para manter o temporizador repetindo
    return true;
}
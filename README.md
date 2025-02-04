# TAREFA TEMPORIZAÇÃO

 Repositório destinado para implementação da tarefa sobre temporizadores do curso de formação básica em software embarcado do embarcatech

## ATIVIDADES

Está tarefa consiste no usos dos recursos de temporizadores disponivéis no microcontrolador RP2040. Na atividade 1 presente na pasta temporizador_periodico são utilzados os temporizadores de hardware enquanto na atividade 2 presente na pasta temporizador_por_disparo são usados temporizadores de sistema para configurar alarmes.

## TEMPORIZADOR PERIÓDICO

Na figura abaixo é apresentado o diagrama do circuito proposto para a atividade

![Diagrama do circuito](https://github.com/ALrEcompUefs/Tarefa_temporizacao/blob/main/img/img1.png?raw=true "imagem 1")

Os requisitos da atividade são:

1. Acionamento das leds de modo a simular um semáforo com um intervalo de 3s.
2. Utilizar a função repeating_timer_callback() para configurar o temporizador.

Para configurar o temporizador foram definidas as intruções

```c
// variaveis de controle da led
static char led_ativa;
bool repeating_timer_callback(struct repeating_timer *t);
```

Onde a variável led_ativa é um caractere para controle de qual led vai ser ativada e a função boolrepeating_timer_callback(structrepeating_timer *t); é a função de callback que é chamada quando o temporizador atinge o tempo progamado.

O temporizador é configurado na seguinte instrução:

```c
// estutura para o temporizador
struct repeating_timer timer;
add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);
```

Nesta função é configurado um temporizador com intervalo de 3s com repetição da contagem, a função de callback é passada como parâmetro junto da struct timer que é usada para passagem de informações.

Na estrutura da função main são feitas as seguintes etapas

* Inicialização das leds e botões
* Configuração da interrupção para o botão
* Configuração do temporizador periódico

Na estrutura do laço de repetição apenas é exibido uma mensagem e aplicado um delay, a função de callback fica responsavel por toda configuração das leds.

### Função de callback

A função de callback do temporizador possuí uma estrutura switch case com a variavel led_ativa. Para cada chamada da função de callback é verificado qual a led que está ativa atualmente para atualizar o valor da proxima led ativa e aguardar 3 segundos para proxima atualização, em cada case do switch a variavél led_ativa é atualizada com o valor da proxima led e depois a função set_rgb ativa a led correspondente;

### Modo bootsel

Para facilitar que os usuarios usem a placa bitdoglab foi implementada uma interrupção no botão A da placa que ativa o modo bootsel

## TEMPORIZADOR POR DISPARO

Os requisitos da atividade 2 são:

* Ativar todos leds quando o botão A for clicado e em seguida desativar um led por vez a cada 3 segundos
* Utilizar a função add_alarm_in_ms para temporização
* Impedir que novos cliques no botão reiniciem o ciclo
* Implementar debouncer para o botão

Para configurar o temporizador foi definida a função de callback

```c
int64_t turn_off_callback(alarm_id_t id, void *user_data);
```

O temporizador é configurado pelas seguintes instruções

```c
add_alarm_in_ms(3000, turn_off_callback, NULL, false);
```

Com a função add_alarm_in_ms um alarme é configurado para o tempo de 3 segundos com a função de callback que definimos. Todo controle das leds é feito na função de callback para isso usamos as seguintes variáveis:

```c
//variavéis de controle do temporizador
static uint contador;       //contador de repetições do alarme
static bool ativar_timer;  // flag de controle da ativação do timer
static bool timer_ativo=false;  // flag de controle da ativação do timer
```

Estás variáveis de flag são combinadas para definir quais leds são ativadas na função de callback.

Na estrutura da função main são feitas as seguintes etapas

* inicialização das leds
* inicialização dos botões
* configuração das interrupções dos botões

No laço infinito da função main está um if que verifica se o botão foi pressionado, se o botão foi pressionado e o alarme não foi ativado então as instruções do laço são executadas.

* Limpa flag do botão
* Ativa flag do temporizador ativo
* Ativa alarme para 3 segundos
* Ativa todas as leds

### Função de callback

Uma estrutura switch-case é usada para determinar quais instruções vão ser executas dentro da função. A função de callback tem o retorno do tipo int64_t pois, se o retrono for 0 o alarme não é rearmado, se for um inteiro positivo então o alarme é rearmado para o tempo equivalente a esse retorno.

Definimos que o retorno padrão é a variável repetir com o valor padrão de 300000( 3 segundos), deste modo o alarme vai ser rearmado para três segundos após o retorno da função de callback.

Em cada chamada da função a variável contador é decrementada passando pelos valores 2,1,0 e ao chegar em 0 repetir é definido em 0 para não rearmar o alarme e a flag do temporizador ativo é liberada permitindo que outra execução seja feita.

### Interrupção e debouncer

Como solicitado foi implentado um debouncer para minimizar os efeitos de bounce do botão. O debouncer utiliza as funções de temporização para obter um intervalo de tempo que o botão ficou pressionado por tempo suficiente e assim liberar o tratamento da interrupção, no techo de codigo abaixo esta função é exemplificado.

```c
// Variavél para registro de tempo e controle de bounce da interrupção
static volatile uint32_t tempo_anterior = 0;

// obtém tempo absoluto do instante atual
uint32_t tempo_atual = to_us_since_boot(get_absolute_time());
// passando um intervalo de 200 ms o bounce estabiliza
if(tempo_atual- tempo_anterior >  200000){
        tempo_anterior= tempo_atual;
	.
	.
}
```

O botão A inícia a temporização dos leds e o botão B ativa o modo bootsel


## SIMULAÇÃO E EXECUCAÇÃO

Para execução do projeto ele deve ser compilado utilzando o arquivo makefile disponível.

Após compilação o projeto pode ser carrgado na placa bitdoglab e pode ser etstado via simulação utilzando a extensão do wokwi no Vs code. Para abrir a simulação basta abrir o arquivo diagram.json

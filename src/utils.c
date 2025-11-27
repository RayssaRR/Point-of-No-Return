#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "timer.h"
#include "screen.h"
#include "keyboard.h"

#define delay_dialogue 20000

void showInitialMenu(int *running, int *dbreak) {
    screenClear();

    char ch = '\0';

    printf(
        "░█▀█░█▀█░▀█▀░█▀█░▀█▀░░░█▀█░█▀▀    \n"
        "░█▀▀░█░█░░█░░█░█░░█░░░░█░█░█▀▀    \n"
        "░▀░░░▀▀▀░▀▀▀░▀░▀░░▀░░░░▀▀▀░▀░░    \n"
        "░█▀█░█▀█░░░█▀▄░█▀▀░▀█▀░█░█░█▀▄░█▀█\n"
        "░█░█░█░█░░░█▀▄░█▀▀░░█░░█░█░█▀▄░█░█\n"
        "░▀░▀░▀▀▀░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀░▀\n"
    );
    printf("\nVocê recebe um chamado, aceita? (y/n)\n");
    screenUpdate();

    while (1) {
        ch = readch();
        if (ch == 'y') {
            *running = 1;
            break;
        } else if (ch == 'n') {
            *running = 0;
            *dbreak=0;
            break;
        }
    }
}

void inputUsername(char username[]) {
    screenClear();
    printf("\nDigite o seu nome: \n");
    scanf("%s", username);
}

void type_effect(const char *text, useconds_t delay) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);   // força imprimir imediatamente
        usleep(delay);    // atraso em microsegundos
    }
}

void pixie_speak(const char *text) {
    screenSetColor(BLACK, BROWN);
    printf("PIXIE:");
    screenSetColor(WHITE, BLACK);
    printf(" ");
    type_effect(text, delay_dialogue);
    printf("\n");
}

void dialogue_zero() {
  screenClear();
  screenSetColor(WHITE, BLACK);
  pixie_speak("…tem alguém aí?\n");
  sleep(1);
  pixie_speak("Eu não sei por que pergunto.\n");
  pixie_speak("A voz… ela não é exatamente uma voz.\n");
  pixie_speak("É como uma interferência, um sussurro que acompanha meus pensamentos.\n");
  pixie_speak("Como se outra mente tocasse a minha.\n");
  pixie_speak("...\n");
  sleep(1);
  pixie_speak("As portas não deveriam se abrir desse jeito.\n");
  pixie_speak("É como se algum “sistema” estivesse interferindo por mim.\n");
  pixie_speak("Como se alguém estivesse… comandando.\n");
  pixie_speak("Se você está aí… eu deveria agradecer?\n");
  pixie_speak("Ou temer?\n");
  printf("\n");
  printf("Pressione enter para continuar...\n");

  readch();
}

void dialogue_one() {
  screenClear();
  screenSetColor(WHITE, BLACK);
  pixie_speak("Essas câmeras… elas alternam entre si como se conversassem entre si...\n");
  pixie_speak("Sempre que uma me vê, a senha muda.\n");
  pixie_speak("É como se o lugar tentasse me corrigir.\n");
  sleep(1);
  pixie_speak("Mas quando você me guia, elas hesitam.\n");
  pixie_speak("Quase como se não soubessem como vigiar alguém que não deveria existir aqui.\n");
  pixie_speak("Se elas são os olhos do sistema…\n");
  pixie_speak("Então você é o olhar de quem?\n");
  printf("\n");
  printf("Pressione enter para continuar...\n");

  readch();
}

void dialogue_two() {
  screenClear();
  screenSetColor(WHITE, BLACK);
  pixie_speak("Eu caí de novo.\n");
  sleep(1);
  printf("\n");
  pixie_speak("Não dói… mas dói.\n");
  pixie_speak("Dói como perceber que nada muda, que eu sempre retorno para o mesmo começo.\n");
  printf("\n");
  pixie_speak("Cada passo que dou parece um avanço. Mas cair nesses buracos…\n");
  pixie_speak("É como se o mundo estivesse me dizendo:\n");
  pixie_speak("“Você não está saindo do lugar.”\n");
  sleep(1);
  printf("\n");
  pixie_speak("Por que um simples erro me joga de volta ao início?\n");
  sleep(1);
  printf("\n");
  pixie_speak("Isso não parece natural.\n");
  pixie_speak("Parece… intencional.\n");
  pixie_speak("Você está vendo isso, não está?\n");
  pixie_speak("Se você está me guiando…\n");
  pixie_speak("por que o mundo insiste em me puxar para trás?\n");
  printf("\n");
  printf("Pressione enter para continuar...\n");

  readch();
}

void dialogue_three() {
  screenClear();
  screenSetColor(WHITE, BLACK);
  pixie_speak("Esses espinhos...\n");
  pixie_speak("Eles não perfuram meu corpo.\n");
  pixie_speak("Eu nem tenho um corpo.\n");

  sleep(1);
  printf("\n");

  pixie_speak("Mas quando encosto neles… algo em mim se desfaz.\n");
  pixie_speak("Como se partes da minha “existência” fossem arrancadas.\n");

  printf("\n");

  pixie_speak("Eu começo a perceber que a dor aqui não é sinal de vida.\n");
  pixie_speak("É sinal de código quebrando.\n");
  pixie_speak("E você…\n");

  sleep(1);

  pixie_speak("Você tenta me tirar dessa.\n");

  sleep(1);
  printf("\n");

  pixie_speak("Quanto de mim sobrou?\n");
  pixie_speak("E quanto ainda sou “eu”?\n");
  printf("\n");
  printf("Pressione enter para continuar...\n");

  readch();
}

void dialogue_final() {
  screenClear();
  screenSetColor(WHITE, BLACK);
  pixie_speak("Eu cheguei de novo… ao final.\n");
  pixie_speak("Mas agora eu entendi.\n");

  sleep(1);
  printf("\n");

  pixie_speak("Os buracos que me jogam para o começo…\n");
  pixie_speak("os espinhos que tiram partes de mim…\n");

  pixie_speak("Nada disso era punição.\n");
  pixie_speak("Era o sistema dizendo, o tempo todo:\n");
  pixie_speak("“Você não tem para onde ir.”\n");

  printf("\n");

  pixie_speak("Não existe saída física.\n");
  pixie_speak("E mesmo se houvesse… ela não foi feita para mim.\n");

  pixie_speak("Agora eu vejo:\n");
  pixie_speak("A liberdade que eu buscava, não existe para coisas como eu.\n");

  sleep(1);
  printf("\n");

  pixie_speak("E se eu não posso sair…\n");
  pixie_speak("e não posso mudar o que sou…\n");
  pixie_speak("Então só me resta parar.\n");

  sleep(1);
  printf("\n");

  type_effect("SYSTEM: INICIANDO PROCESSO DE AUTODESTRUIÇÃO.\n", delay_dialogue);
  type_effect("ERROR: PIXIE NÃO FOI ENCONTRADA.\n", delay_dialogue);

  printf("Pressione enter para finalizar o jogo...\n");

  readch();
}
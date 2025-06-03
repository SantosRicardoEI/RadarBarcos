#include "modulo.h"

// ================================================ IMPRESSAO ==========================================================

/**
 * @brief Imprime os dados do frame atual da simulação.
 *
 * Lista todos os navios presentes no frame, indicando a posição e
 * velocidade de cada um. Caso não existam navios, informa o utilizador.
 *
 * @param frame Ponteiro para o frame atual da simulação.
 */
void imprimirFrameAtual(BaseDados *frame) {
    EntidadeIED *atual = frame->barcos;
    if (frame == NULL) return;


    printf("\nEstado do frame %d:\n", frame->frame_atual_num);

    if (!atual) {
        printf("(Sem navios no frame)\n");
        return;
    }

    while (atual != NULL) {
        printf("Barco %c: posicao (%d,%d), velocidade (%d,%d)\n",
               atual->no_nautico->nome,
               atual->posicao[0],
               atual->posicao[1],
               atual->velocidade[0],
               atual->velocidade[1]);
        atual = atual->seguinte;
    }
}

/**
 * @brief Imprime os dados detalhados dos navios presentes no frame, para fins de depuração.
 *
 * Apresenta o nome, tipo, posição e velocidade de cada navio no frame atual.
 * Utiliza um array de strings para traduzir o tipo numérico do navio para o nome do tipo.
 * Se não houver navios no frame, indica essa situação.
 *
 * @param frame Ponteiro para o frame atual da simulação.
 */
void imprimirBarcosDebug(BaseDados *frame) {
    EntidadeIED *atual = frame->barcos;
    char *tiposNomes[] = {
        "JonhDoe", // 0
        "ProfPaiMau", // 1
        "Cruzador", // 2
        "Submarino", // 3
        "Pescador", // 4
        "Traficante", // 5
        "Polícia", // 6
        "Turismo", // 7
        "Titanic", // 8
        "Veleiro", // 9
        "Rebocador", // 10
        "Cacilheiro", // 11
        "PortaAvioes", // 12
        "Lusitania" // 13
    };

    if (frame == NULL) return;

    printf("\nFrame %d:\n", frame->frame_atual_num);

    if (!atual) {
        printf("(Sem navios no frame)\n");
        return;
    }


    while (atual != NULL) {
        char tipoNome[20];
        strcpy(tipoNome, tiposNomes[atual->no_nautico->tipologia]);


        printf("Barco %-3c | Tipo %-2d: %-12s | Pos %3d,%-5d | Vel %3d,%3d\n",
               atual->no_nautico->nome,
               atual->no_nautico->tipologia,
               tipoNome,
               atual->posicao[0],
               atual->posicao[1],
               atual->velocidade[0],
               atual->velocidade[1]);
        atual = atual->seguinte;
    }
}

/**
 * @brief Mostra o menu principal de opções ao utilizador.
 */
void imprimirMenu(void) {
    printf("\n=== MENU DA SIMULACAO ===\n"
        "1. Atualizar simulacao\n"
        "2. Inserir ou alterar barco\n"
        "3. Previsao de colisoes\n"
        "4. Rastrear histotico reverso\n"
        "5. Velocidade media de um barco\n"
        "6. Visualizar movimento (Python)\n"
        "7. Toggle Debug\n"
        "0. Sair\n"
        "Escolha uma opcao: ");
}


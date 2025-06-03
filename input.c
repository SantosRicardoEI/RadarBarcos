#include "modulo.h"

// ================================================ INPUT ==============================================================

/**
 * @brief Processa e valida os argumentos da linha de comandos.
 *
 * Esta função extrai os argumentos fornecidos pelo utilizador ao iniciar o programa,
 * incluindo o ficheiro de entrada, dimensões da grelha, número de frames a simular
 * e o ficheiro de saída. Valida também o número e o formato dos argumentos.
 *
 * @param argc Número de argumentos recebidos na linha de comandos.
 * @param argv Vetor de strings com os argumentos.
 * @param ficheiro_entrada Ponteiro para a string onde será guardado o nome do ficheiro de entrada.
 * @param linhas Ponteiro para inteiro onde será armazenado o número de linhas da grelha.
 * @param colunas Ponteiro para inteiro onde será armazenado o número de colunas da grelha.
 * @param numFrames Ponteiro para inteiro onde será armazenado o número de frames a gerar.
 * @param ficheiro_saida Ponteiro para a string onde será guardado o nome do ficheiro de saída.
 */
void lerArgsMain(int argc, char *argv[],
                 char **ficheiro_entrada, int *linhas, int *colunas,
                 int *numFrames, char **ficheiro_saida) {

    char *dimensoes_str = argv[2];
    char *numFrames_str = argv[3];
    *ficheiro_entrada = argv[1];
    *ficheiro_saida = argv[4];

    if (argc != 5) {
        fprintf(stderr, "Uso: %s <ficheiro_entrada> <dimensoes> <numero_frames> <ficheiro_saida>\n", argv[0]);
        exit(1);
    }


    *numFrames = atoi(numFrames_str);
    if (*numFrames < 0) {
        fprintf(stderr, "Número de frames inválido: deve ser >= 0\n");
        exit(1);
    }

    if (sscanf(dimensoes_str, "%dx%d", linhas, colunas) != 2) {
        fprintf(stderr, "Dimensões inválidas: deve ser no formato <linhas>x<colunas>\n");
        exit(1);
    }

    printf("\n");
}

/**
 * @brief Lê os dados iniciais dos barcos a partir de um ficheiro de texto.
 *
 * Abre o ficheiro fornecido, interpreta cada linha com os dados de um barco
 * (ID, latitude, longitude, ângulo, velocidade, tipo), converte o ângulo e velocidade
 * em componentes de velocidade (vx, vy), e insere os barcos na lista ligada do frame.
 *
 * Em caso de erro na abertura do ficheiro ou alocação de memória, limpa a memória
 * previamente alocada e termina o programa com erro.
 *
 * @param ficheiro Nome do ficheiro de entrada com os dados dos barcos.
 * @param frame Ponteiro para o frame onde os barcos serão inseridos.
 */
void lerFicheiroInicial(const char *ficheiro, BaseDados *frame) {
    // Variáveis para armazenar os dados do ficheiro
    char id;
    int lat, lon, angulo, velocidade, tipo;
    double rad;

    // Abrir o ficheiro em modo de leitura
    FILE *fp = fopen(ficheiro, "r");

    // Se falhar, mostra erro, liberta a memória e termina o programa
    if (fp == NULL) {
        printf("\nErro ao abrir ficheiro \"%s\"\n", ficheiro);
        limparFrameInicial(frame);
        exit(1);
    }

    // Lê cada linha do ficheiro
    while (fscanf(fp, " %c %d %d %d %d %d", &id, &lat, &lon, &angulo, &velocidade, &tipo) == 6) {
        int vx, vy;

        // Alocar memoria para o navio (NoVessel)
        NoVessel *navio = malloc(sizeof(NoVessel));
        // Aloca memoria para a entidade no frame (EntidadeIED)
        EntidadeIED *entidade = malloc(sizeof(EntidadeIED));

        // Se nao conseguiu alocar memoria para o navio mostra erro e limpa
        if (!navio) {
            perror("Erro ao alocar navio");
            fclose(fp);
            limparFrameInicial(frame);
            exit(1);
        }

        // Se nao conseguiu alocar memoria para a entidade mostra erro e limpa
        if (!entidade) {
            perror("Erro ao alocar entidade");
            free(navio);
            fclose(fp);
            limparFrameInicial(frame);
            exit(1);
        }

        // Preenche os dados do navio
        navio->nome = id;
        navio->tipologia = tipo;
        navio->isVisible = 1;

        // Converte o angulo para rad para calculos
        rad = angulo * M_PI / 180.0;

        // Calcular as componentes da velocidade
        vx = (int)(round(cos(rad) * velocidade));
        vy = (int)(round(sin(rad) * velocidade));

        // Preenche os dados da entidade
        entidade->posicao[0] = lon;
        entidade->posicao[1] = lat;
        entidade->velocidade[0] = vx;
        entidade->velocidade[1] = vy;
        entidade->no_nautico = navio;
        entidade->seguinte = NULL;

        // Insere a entidade no fim da lista de frames

        // Caso seja a primeira entidade
        if (frame->barcos == NULL) {
            frame->barcos = entidade;
        }
        // Caso contrario insere no fim
        else {
            EntidadeIED *atual = frame->barcos;
            while (atual->seguinte != NULL) {
                atual = atual->seguinte;
            }
            atual->seguinte = entidade;
        }
    }

    // Fecha o ficheiro após ler tudo
    fclose(fp);
}

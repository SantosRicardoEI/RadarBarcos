#include "modulo.h"

// ================================================ INTERFACE ==========================================================

/**
 * @brief Pede ao utilizador o número de frames a avançar na simulação.
 *
 * Esta função lê do utilizador quantos frames deseja avançar e chama a função
 * `avancarFrame()` para atualizar a simulação. Após isso, limpa a memória
 * associada às colisões detetadas (que não são usadas nesta função).
 *
 * @param frameAtual Ponteiro para o ponteiro do frame atual da simulação.
 * @param listaFrames Ponteiro para a lista de todos os frames da simulação.
 * @param latMax Número máximo de linhas da grelha.
 * @param lonMax Número máximo de colunas da grelha.
 */
void pedeAtualizarSimulacao(BaseDados **frameAtual, ListaFrames *listaFrames, int latMax, int lonMax) {
    int numFrames;
    Colisao *col;

    // Pede ao utilizador quantos frames deseja avançar
    printf("Quantos frames deseja avançar? ");

    // Em caso de input inválido limpa o buffer e atribui valor inválido
    if (scanf("%d", &numFrames) != 1) {
        while (getchar() != '\n');
        numFrames = -1;
    }

    // Atualiza a simulação com base no numero de frames indicado.
    // A função avancarFrame aloca memoria para as colisoes detetadas,
    // mesmo que nao sejam necessárias aqui. Por isso, guardamos o retorno
    // para libertar corretamente essa memoria logo de seguida.
    col = avancarFrame(frameAtual, listaFrames, numFrames, latMax, lonMax, 1);

    // Limpo a memoria das colisos que nao rpeciso aqui
    while (col != NULL) {
        Colisao *next = col->seguinte;

        // Liberta a lista de barcos em colisão
        BarcosEmColisao *b = col->barcos;
        while (b != NULL) {
            BarcosEmColisao *bNext = b->seguinte;
            free(b);
            b = bNext;
        }

        free(col);
        col = next;
    }

    // Informa o utilizador qual o frame atual
    printf("Simulação atualizada para o frame %d\n", (*frameAtual)->frame_atual_num);
}

/**
 * @brief Pede ao utilizador o identificador de um barco e imprime as suas estatisticas.
 *
 * Lê o identificador de um barco e chama a função de cálculo da velocidade média
 * desde o frame inicial até ao frame atual.
 *
 * @param frameAtual Ponteiro para o ponteiro do frame atual.
 */
void pedeVelocidadeBarco(BaseDados **frameAtual) {
    int sucesso;
    char barco;

    printf("\n=== Velocidade Media de um Barco ===");
    printf("\nNome do barco (uma letra): ");
    sucesso = scanf(" %c", &barco);
    if (sucesso != 1) {
        printf("Nome invalido.\n");
        while (getchar() != '\n');
        return;
    }

    imprimirVelocidadeBarco(frameAtual, barco);
}

/**
* @brief Insere um novo barco ou altera um existente no frame atual.
*
* Esta função solicita ao utilizador os dados de uma embarcação (nome, posição,
* ângulo, velocidade e tipo). Se o barco já existir no frame atual (identificado
* pelo nome), os seus dados são atualizados. Caso contrário, um novo barco é
* alocado e inserido na lista ligada de embarcações do frame atual.
*
* @param linhas Número máximo de linhas da grelha (limite da latitude).
* @param colunas Número máximo de colunas da grelha (limite da longitude).
* @param frameAtual Ponteiro para o frame atual onde será inserido ou alterado o barco.
*/
void inserirOuAlterarBarco(int linhas, int colunas, BaseDados *frameAtual) {
    char barco;
    int lat, lon, angulo, velocidade, tipo;
    int vx, vy;
    double rad;
    int sucesso;

    EntidadeIED *atual;
    EntidadeIED *anterior;
    NoVessel *novoNavio;
    EntidadeIED *novaEntidade;

    printf("\n=== Inserir/Alterar Barco ===\n");

    // Ler nome do barco
    printf("Nome do barco (uma letra): ");
    sucesso = scanf(" %c", &barco);
    if (sucesso != 1) {
        printf("Nome invalido.\n");
        while (getchar() != '\n');
        return;
    }

    // Ler posição
    printf("Posição inicial (latitude longitude): ");
    sucesso = scanf("%d %d", &lat, &lon);
    if (sucesso != 2 || lat < 0 || lon < 0 || lat >= linhas || lon >= colunas) {
        printf("Posicao invalida.\n");
        while (getchar() != '\n');
        return;
    }

    // Ler ângulo
    printf("Ângulo (múltiplo de 45): ");
    sucesso = scanf("%d", &angulo);
    if (sucesso != 1 || angulo % 45 != 0 || angulo < 0 || angulo >= 360) {
        printf("Ângulo inválido.\n");
        while (getchar() != '\n');
        return;
    }

    // Ler velocidade
    printf("Velocidade: ");
    sucesso = scanf("%d", &velocidade);
    if (sucesso != 1 || velocidade < 0) {
        printf("Velocidade invalida.\n");
        while (getchar() != '\n');
        return;
    }

    // Ler tipo
    printf("Tipo do barco (1-13): ");
    sucesso = scanf("%d", &tipo);
    if (sucesso != 1 || tipo < 1 || tipo > 13) {
        printf("Tipo inválido.\n");
        while (getchar() != '\n');
        return;
    }

    // Calcular velocidade em x e y
    rad = angulo * M_PI / 180.0;
    vx = (int)(round(cos(rad) * velocidade));
    vy = (int)(round(sin(rad) * velocidade));

    // Procurar barco existente na lista
    atual = frameAtual->barcos;
    anterior = NULL;
    while (atual != NULL) {
        // Se barco existe na lista atualizo os seus dados
        if (atual->no_nautico->nome == barco) {
            atual->posicao[0] = lon;
            atual->posicao[1] = lat;
            atual->velocidade[0] = vx;
            atual->velocidade[1] = vy;
            atual->no_nautico->tipologia = tipo;
            printf("Barco %c alterado com sucesso.\n", barco);
            return;
        }
        anterior = atual;
        atual = atual->seguinte;
    }

    // Caso não exista, crio novo barco
    novoNavio = malloc(sizeof(NoVessel));
    if (!novoNavio) {
        perror("Erro ao alocar navio");
        exit(1);
    }
    novoNavio->nome = barco;
    novoNavio->tipologia = tipo;
    novoNavio->isVisible = 1;

    // Criar nova entidade no frame
    novaEntidade = malloc(sizeof(EntidadeIED));
    if (!novaEntidade) {
        perror("Erro ao alocar entidade");
        exit(1);
    }
    novaEntidade->posicao[0] = lon;
    novaEntidade->posicao[1] = lat;
    novaEntidade->velocidade[0] = vx;
    novaEntidade->velocidade[1] = vy;
    novaEntidade->no_nautico = novoNavio;
    novaEntidade->seguinte = NULL;

    // Inserir no inicio da lista se for o primeiro, se não no fim
    if (anterior == NULL) {
        frameAtual->barcos = novaEntidade;
    } else {
        anterior->seguinte = novaEntidade;
    }

    printf("Barco %c adicionado com sucesso.\n", barco);
}

/**
 * @brief Pede ao utilizador o numero de frames a recuar na simulação.
 *
 * Chama a função `rewindFrames` para recuar no histórico da simulação.
 *
 * @param frameAtual Ponteiro para o ponteiro do frame atual.
 * @param showOutput Se diferente de zero, imprime informações durante o processo.
 */
void rastrearHistoricoReverso(BaseDados **frameAtual, ListaFrames *listaFrames) {
    int steps;
    printf("Quantos frames deseja voltar? ");
    if (scanf("%d", &steps) != 1) {
        while (getchar() != '\n');
        steps = -1;
    }

    rewindFrames(frameAtual, listaFrames, steps);
}

/**
* @brief Calcula e imprime a velocidade média de um barco desde o frame 0 até ao frame atual.
*
* Esta função localiza o barco especificado tanto no frame inicial (frame 0) como no frame atual,
* calcula a distância percorrida com base na variação de posição, e divide pelo número de frames
* decorridos para determinar a velocidade média em casas por frame.
*
* @param frameAtual Ponteiro duplo para o frame atual da simulação.
* @param barco Carácter identificador do barco a ser analisado.
*/
void imprimirVelocidadeBarco(BaseDados **frameAtual, char barco) {
    BaseDados *frameZero;
    int posInicialX, posInicialY;
    int posAtualX, posAtualY;
    int primeiroFrame, ultimoFrame;
    BaseDados *ptr;
    EntidadeIED *b;
    float dx, dy, distancia;
    int framesPercorridos;
    float velocidadeMedia;

    if (frameAtual == NULL || *frameAtual == NULL) {
        printf("Frame atual inválido.\n");
        return;
    }

    frameZero = *frameAtual;
    while (frameZero->prev != NULL)
        frameZero = frameZero->prev;

    posInicialX = -1;
    posInicialY = -1;
    posAtualX = -1;
    posAtualY = -1;
    primeiroFrame = -1;
    ultimoFrame = -1;

    ptr = frameZero;
    while (ptr != NULL && ptr->frame_atual_num <= (*frameAtual)->frame_atual_num) {
        b = ptr->barcos;
        while (b != NULL) {
            if (b->no_nautico->nome == barco) {
                if (primeiroFrame == -1) {
                    posInicialX = b->posicao[0];
                    posInicialY = b->posicao[1];
                    primeiroFrame = ptr->frame_atual_num;
                }
                posAtualX = b->posicao[0];
                posAtualY = b->posicao[1];
                ultimoFrame = ptr->frame_atual_num;
            }
            b = b->seguinte;
        }
        ptr = ptr->next;
    }

    if (primeiroFrame == -1) {
        printf("Barco %c não encontrado em nenhum frame.\n", barco);
        return;
    }

    dx = posAtualX - posInicialX;
    dy = posAtualY - posInicialY;
    distancia = sqrtf(dx * dx + dy * dy);
    framesPercorridos = ultimoFrame - primeiroFrame;
    velocidadeMedia = (framesPercorridos > 0) ? distancia / framesPercorridos : 0.0;

    printf("\nEstatísticas do barco %c:\n", barco);
    printf("Frame inicial: %d\n", primeiroFrame);
    printf("Frame final: %d\n", ultimoFrame);
    printf("Posição inicial: (%d, %d)\n", posInicialX, posInicialY);
    printf("Posição atual: (%d, %d)\n", posAtualX, posAtualY);
    printf("Distância percorrida: %.2f casas\n", distancia);
    printf("Velocidade média: %.2f casas/frame\n", velocidadeMedia);
}

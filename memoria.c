#include "modulo.h"

// ================================================ MEMORIA ============================================================

/**
* @brief Remove todos os frames futuros da simulação a partir do frame atual.
*
* Esta função percorre os frames seguintes ao frame atual, libertando a memória associada
* às entidades (EntidadeIED) e, quando apropriado, aos navios (NoVessel) não reutilizados
* em frames anteriores. Garante que os dados dos frames futuros são corretamente eliminados
* e atualiza os ponteiros da estrutura ListaFrames.
*
* @param frameAtual Ponteiro duplo para o frame atual da simulação.
* @param listaFrames Ponteiro para a lista de todos os frames da simulação.
*/
void apagarFramesFuturos(BaseDados **frameAtual, ListaFrames *listaFrames) {
    BaseDados *atual = (*frameAtual)->next;  // Começo no frame seguinte ao atual
    NoVessel *jaLibertados[100];             // Lista para controlar os NoVessel já libertados
    int jaLibertadosCount = 0;               // Contador de NoVessel libertados

    while (atual != NULL) {
        BaseDados *aRemover = atual;
        EntidadeIED *barco = aRemover->barcos;
        EntidadeIED *entidades[100];         // Lista temporária de entidades a libertar
        NoVessel *vessels[100];              // Lista temporária dos navios associados
        int count = 0;

        // Avanço antes de remover
        atual = atual->next;

        // Guardo as entidades e os respetivos navios
        while (barco != NULL && count < 100) {
            entidades[count] = barco;
            vessels[count] = barco->no_nautico;
            barco = barco->seguinte;
            count++;
        }

        // Libertar a memoria das entidades
        for (int i = 0; i < count; i++) {
            free(entidades[i]);
        }

        // Verificar e libertar os NoVessel se não forem usados nos frames anteriores
        for (int i = 0; i < count; i++) {
            int jaFoiLibertado = 0;
            int encontrado = 0;
            BaseDados *frameCheck;

            if (vessels[i] == NULL) continue;

            // Aqui verifico se ja foi libertado (para os casos de inserção em frames que não 0)
            for (int j = 0; j < jaLibertadosCount; j++) {
                if (jaLibertados[j] == vessels[i]) {
                    jaFoiLibertado = 1;
                    break;
                }
            }
            if (jaFoiLibertado) continue;

            // Verifico se o navio ainda é usado em frames anteriores
            frameCheck = (*frameAtual);
            while (frameCheck != NULL && !encontrado) {
                EntidadeIED *e = frameCheck->barcos;
                while (e != NULL) {
                    if (e->no_nautico == vessels[i]) {
                        encontrado = 1;
                        break;
                    }
                    e = e->seguinte;
                }
                frameCheck = frameCheck->prev;
            }

            // Se nao foi encontrado já posso libertar
            if (!encontrado) {
                free(vessels[i]);
                jaLibertados[jaLibertadosCount++] = vessels[i];
            }
        }

        // Libertar a memoria do frame atual
        free(aRemover);
        listaFrames->total_frames--;
    }

    // Atualizra os ponteiros da lista dos frames
    (*frameAtual)->next = NULL;
    listaFrames->tail = *frameAtual;
}

/**
 * @brief Guarda os dados do frame atual no ficheiro "depois.txt".
 *
 * Esta função percorre todos os barcos presentes no frame atual e escreve no ficheiro
 * "depois.txt" os seus dados: identificador, posição (latitude, longitude),
 * ângulo de deslocação, velocidade escalar e tipo.
 * Submarinos invisíveis (tipo 3 e isVisible == 0) são ignorados.
 *
 * O ficheiro é sobrescrito em cada chamada. Se ativado, mostra mensagem de confirmação.
 *
 * @param frameAtual Ponteiro para o frame atual cujos dados serão guardados.
 * @param showOutput Valor booleano (0 ou 1) que indica se deve mostrar mensagem de sucesso.
 */
void guardarFrameNoFicheiro(BaseDados *frameAtual, int showOutput) {
    EntidadeIED *atual = frameAtual->barcos;  // Aponta para o início da lista de barcos
    char id;

    // Abrir o ficheiro para escrita
    FILE *fp = fopen("depois.txt", "w");
    if (!fp) {
        perror("Erro ao abrir depois.txt");
        return;
    }

    // Percorrer todos os barcos do frame
    while (atual != NULL) {
        int tipo = atual->no_nautico->tipologia;
        int visivel = atual->no_nautico->isVisible;
        int lat;
        int lon;
        int vx;
        int vy;
        double angulo_rad;
        int angulo_deg;
        int velocidade;

        // Ignoro os submarinos invisíveis
        if (tipo == 3 && visivel == 0) {
            atual = atual->seguinte;
            continue;
        }

        // Guardar os dados do barco
        id = atual->no_nautico->nome;
        lat = atual->posicao[1];
        lon = atual->posicao[0];
        vx = atual->velocidade[0];
        vy = atual->velocidade[1];

        // Calculo dos dados de cada barco
        angulo_rad = atan2(vy, vx);  // radianos
        angulo_deg = (int) round(angulo_rad * 180.0 / M_PI);
        if (angulo_deg < 0)
            angulo_deg += 360;

        velocidade = (int) round(sqrt(vx * vx + vy * vy));

        // Escreve no ficheiro
        fprintf(fp, "%c %d %d %d %d %d\n", id, lat, lon, angulo_deg, velocidade, tipo);

        // Avança para o próximo barco
        atual = atual->seguinte;
    }
    fclose(fp);

    // Mostra mensagem de confirmação se pedido
    if (showOutput) {
        printf("Frame %d guardado com sucesso em depois.txt\n", frameAtual->frame_atual_num);
    }
}

/**
 * @brief Liberta toda a memória associada aos barcos do frame inicial.
 *
 * Esta função percorre a lista de entidades (barcos) do frame zero e
 * liberta, de forma segura, tanto os nós da lista (`EntidadeIED`) como
 * as estruturas associadas (`NoVessel`), assumindo que já não existem
 * outros frames a referenciar os mesmos ponteiros.
 *
 * Deve ser chamada após a função `rewindFrames`, que garante que o
 * frame inicial é o único restante e que não há duplicação de
 * ponteiros `no_nautico` em memória.
 *
 * @param frameZero Ponteiro para o frame inicial da simulação.
 */
void limparFrameInicial(BaseDados *frameZero) {
    EntidadeIED *barco = frameZero->barcos;
    while (barco != NULL) {
        EntidadeIED *seguinte = barco->seguinte;

        if (barco->no_nautico != NULL)
            free(barco->no_nautico);

        free(barco);
        barco = seguinte;
    }

    frameZero->barcos = NULL;
}


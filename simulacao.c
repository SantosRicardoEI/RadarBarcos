#include "modulo.h"

// ================================================ SIMULACAO ==========================================================

/**
 * @brief Avança a simulação um número específico de frames.
 *
 * Esta função gera novos frames a partir do frame atual, atualizando a posição
 * de cada barco com base na sua tipologia e comportamento definido. Durante
 * este processo, barcos que saem dos limites do radar são ignorados e removem-se
 * automaticamente barcos em colisão. Os frames gerados são ligados à lista
 * de frames existente, e a memória é corretamente alocada.
 *
 * Ao final, o último frame gerado é guardado em ficheiro, e é devolvida a lista
 * de colisões registadas ao longo dos frames criados.
 *
 * @param frameAtual Ponteiro para o ponteiro do frame atual da simulação.
 * @param listaFrames Ponteiro para a estrutura que contém o início e fim da lista de frames.
 * @param numFrames Número de frames que se deseja gerar/avançar.
 * @param latitudeMax Número máximo de linhas (altura da grelha).
 * @param longitudeMax Número máximo de colunas (largura da grelha).
 * @param showOutput Se for diferente de zero, imprime mensagens de debug durante a execução.
 * @return Ponteiro para a lista de colisões detetadas durante o avanço dos frames.
 */
Colisao *avancarFrame(BaseDados **frameAtual, ListaFrames *listaFrames, int numFrames, int latitudeMax,
                      int longitudeMax, int showOutput) {
    Colisao *colisoes = NULL;           // Lista global de colisões acumuladas
    Colisao *ultimaColisao = NULL;      // Ponteiro para a última colisão da lista

    // Gera numFrames frames novos
    for (int i = 0; i < numFrames; i++) {
        // Cria novo frame
        BaseDados *novoFrame = malloc(sizeof(BaseDados));
        EntidadeIED *anterior = (*frameAtual)->barcos;  // Lista de barcos do frame anterior
        EntidadeIED *novaLista = NULL;                  // Nova lista para o novo frame
        EntidadeIED *ultima = NULL;                     // Último elemento da nova lista
        Colisao *colisoesFrame;                         // Colisões
        if (!novoFrame) {
            perror("Erro ao alocar novo frame");
            exit(1);
        }

        novoFrame->barcos = NULL;
        novoFrame->prev = *frameAtual;
        novoFrame->next = NULL;
        novoFrame->frame_atual_num = (*frameAtual)->frame_atual_num + 1;

        // Processar cada barco do frame anterior
        while (anterior != NULL) {
            int tipo = anterior->no_nautico->tipologia;
            int novaX, novaY;
            EntidadeIED *novo;

            // Comportamento específico por tipo de barco
            switch (tipo) {
                case 1:  // ProfPaiMau - Movimento padrão
                    novaX = anterior->posicao[0] + anterior->velocidade[0];
                    novaY = anterior->posicao[1] + anterior->velocidade[1];
                    break;
                case 2:  // Cruzador - Duplica velocidade se ninguém perto
                    if (!temBarcosADistancia(*frameAtual, anterior, 4)) {
                        novaX = anterior->posicao[0] + anterior->velocidade[0] * 2;
                        novaY = anterior->posicao[1] + anterior->velocidade[1] * 2;
                    } else {
                        novaX = anterior->posicao[0] + anterior->velocidade[0];
                        novaY = anterior->posicao[1] + anterior->velocidade[1];
                    }
                    break;
                case 3:  // Submarino - Alterna visibilidade a cada 5 frames
                    if ((novoFrame->frame_atual_num % 5) == 0)
                        anterior->no_nautico->isVisible = !anterior->no_nautico->isVisible;
                    novaX = anterior->posicao[0] + anterior->velocidade[0];
                    novaY = anterior->posicao[1] + anterior->velocidade[1];
                    break;
                case 10: // Rebocador - Move 1 casa se estiver próximo de outro barco
                    if (temBarcosADistancia(*frameAtual, anterior, 5)) {
                        int dirX = anterior->velocidade[0];
                        int dirY = anterior->velocidade[1];
                        int vx = (dirX == 0) ? 0 : (dirX > 0 ? 1 : -1);
                        int vy = (dirY == 0) ? 0 : (dirY > 0 ? 1 : -1);
                        novaX = anterior->posicao[0] + vx;
                        novaY = anterior->posicao[1] + vy;
                        anterior->velocidade[0] = vx;
                        anterior->velocidade[1] = vy;
                    } else {
                        novaX = anterior->posicao[0] + anterior->velocidade[0];
                        novaY = anterior->posicao[1] + anterior->velocidade[1];
                    }
                    break;
                default: // Comportamento genérico
                    novaX = anterior->posicao[0] + anterior->velocidade[0];
                    novaY = anterior->posicao[1] + anterior->velocidade[1];
                    break;
            }

            // Se barco saiu fora do radar é ignorado
            if (novaX < 0 || novaX >= longitudeMax || novaY < 0 || novaY >= latitudeMax) {
                if (showOutput)
                    printf("\033[1;31mBarco %c saiu do radar\033[0m\n", anterior->no_nautico->nome);
                anterior = anterior->seguinte;
                continue;
            }

            // Aloca nova entidade para este frame
            novo = malloc(sizeof(EntidadeIED));
            if (!novo) {
                perror("Erro ao alocar barco");
                exit(1);
            }

            // Copia os dados
            novo->posicao[0] = novaX;
            novo->posicao[1] = novaY;
            novo->velocidade[0] = anterior->velocidade[0];
            novo->velocidade[1] = anterior->velocidade[1];
            novo->no_nautico = anterior->no_nautico;
            novo->seguinte = NULL;

            // Adiciono à lista ligada
            if (novaLista == NULL)
                novaLista = novo;
            else
                ultima->seguinte = novo;

            ultima = novo;
            anterior = anterior->seguinte;
        }

        // Remove barcos que colidiram neste frame e obtém a lista de colisões
        colisoesFrame = removerBarcosEmColisao(&novaLista, showOutput);

        // Junta colisões detetadas à lista global
        if (colisoes == NULL) {
            colisoes = colisoesFrame;
        } else {
            ultimaColisao->seguinte = colisoesFrame;
        }

        // Atualiza ponteiro para o fim da lista de colisões
        if (colisoesFrame != NULL) {
            Colisao *tmp = colisoesFrame;
            while (tmp->seguinte != NULL)
                tmp = tmp->seguinte;
            ultimaColisao = tmp;
        }

        // Liga o novo frame à lista de frames
        novoFrame->barcos = novaLista;
        (*frameAtual)->next = novoFrame;
        novoFrame->prev = *frameAtual;
        *frameAtual = novoFrame;
        listaFrames->tail = novoFrame;
        listaFrames->total_frames++;
    }

    // Retorna lista completa de colisões detetadas
    return colisoes;
}

/**
 * @brief Verifica se existem barcos próximos de um barco dado num frame.
 *
 * Percorre a lista de embarcações no frame e verifica se existe algum barco
 * a uma distância máxima de 'n' unidades (em ambos os eixos) do barco fornecido.
 * Submarinos invisíveis são ignorados durante esta verificação.
 *
 * @param frame Ponteiro para o frame onde se encontram os barcos.
 * @param barco Ponteiro para a entidade a partir da qual se calcula a distância.
 * @param n Distância máxima (em células) a considerar para proximidade.
 * @return 1 se houver outro barco visível dentro da distância 'n'; 0 caso contrário.
 */
int temBarcosADistancia(BaseDados *frame, EntidadeIED *barco, int n) {
    int x;
    int y;
    EntidadeIED *atual;

    // Verificação dos argumentos
    if (frame == NULL || barco == NULL || n < 1)
        return 0;

    // Coordenadas do barco de referência
    x = barco->posicao[0];
    y = barco->posicao[1];

    atual = frame->barcos;

    // Percorre todos os barcos do frame
    while (atual != NULL) {
        // Ignora o próprio barco
        if (atual != barco) {
            int dx = atual->posicao[0] - x;
            int dy = atual->posicao[1] - y;

            // Verifica se está dentro da area definida
            if (dx >= -n && dx <= n && dy >= -n && dy <= n) {
                int tipo = atual->no_nautico->tipologia;

                // Se for submarino invisível é ignorado
                if (tipo == 3) {
                    if (!atual->no_nautico->isVisible) {
                        atual = atual->seguinte;
                        continue;
                    }
                }

                // Barco encontrado dentro da distância
                return 1;
            }
        }
        // Próximo
        atual = atual->seguinte;
    }

    // Nenhum barco encontrado na area escolhida
    return 0;
}

/**
 * @brief Simula a evolução da simulação para prever colisões futuras.
 *
 * Esta função avança a simulação iterativamente frame a frame até que:
 * - Não existam mais barcos visíveis, ou
 * - Todos os barcos estejam parados.
 *
 * Em cada frame simulado, são analisadas colisões entre embarcações. Estas são apresentadas
 * ao utilizador com os barcos envolvidos e a posição da colisão. Após a previsão, todos os
 * frames futuros são eliminados e a simulação é revertida ao frame inicial.
 *
 * @param frameAtual Ponteiro para o ponteiro do frame atual da simulação.
 * @param listaFrames Estrutura que contém referências ao início e fim da lista de frames.
 * @param latitudeMax Número máximo de linhas da grelha.
 * @param longitudeMax Número máximo de colunas da grelha.
 */
void previsaoDeColisoes(BaseDados **frameAtual, ListaFrames *listaFrames, int latitudeMax, int longitudeMax) {
    // Guardar o frame inicial para voltar atrás no final
    BaseDados *frameInicial = *frameAtual;
    int frameCount = 0;
    BaseDados *tempFrame;

    printf("\n=== Previsão de Colisões ===\n");

    while (1) {
        EntidadeIED *temp = (*frameAtual)->barcos;
        int barcosVisiveis = 0;
        int algumComVelocidade = 0;
        Colisao *colisoes;

        // Verifica se há barcos visíveis e se algum se está a mover
        while (temp != NULL) {
            int tipo = temp->no_nautico->tipologia;
            int visivel = temp->no_nautico->isVisible;

            // Só considera visíveis os não-submarinos ou submarinos visíveis
            if (tipo != 3 || visivel) {
                barcosVisiveis++;
                if (temp->velocidade[0] != 0 || temp->velocidade[1] != 0)
                    algumComVelocidade = 1;
            }

            temp = temp->seguinte;
        }

        // Se não há barcos ou todos estão parados, parar a previsão
        if (barcosVisiveis == 0 || !algumComVelocidade)
            break;

        // Avança 1 frame sem mostrar output
        colisoes = avancarFrame(frameAtual, listaFrames, 1, latitudeMax, longitudeMax, 0);

        // Imprimir colisões detetadas
        while (colisoes != NULL) {
            BarcosEmColisao *b = colisoes->barcos;
            Colisao *colisaoTmp = colisoes;
            BarcosEmColisao *btmp = colisaoTmp->barcos;

            printf("Frame %d\n    Colisão prevista entre barcos: ", (*frameAtual)->frame_atual_num);
            while (b != NULL) {
                printf("%c", b->id);
                if (b->seguinte != NULL) printf(", ");
                b = b->seguinte;
            }
            printf("\n    Posicao prevista da colisao: (%d,%d) \n", colisoes->x, colisoes->y);

            // Libertar memória desta colisão
            colisoes = colisoes->seguinte;
            while (btmp != NULL) {
                BarcosEmColisao *aux = btmp;
                btmp = btmp->seguinte;
                free(aux);
            }
            free(colisaoTmp);
        }

        frameCount++;
    }

    // Recuar ao frame onde começou a previsão
    while (*frameAtual != frameInicial)
        *frameAtual = (*frameAtual)->prev;

    // Libertar todos os frames criados após o frame inicial
    tempFrame = (*frameAtual)->next;
    while (tempFrame != NULL) {
        BaseDados *aRemover = tempFrame;
        EntidadeIED *barco = aRemover->barcos;
        tempFrame = tempFrame->next;

        // Libertar lista de barcos
        while (barco != NULL) {
            EntidadeIED *tmp = barco;
            barco = barco->seguinte;
            free(tmp);
        }

        free(aRemover);
        listaFrames->total_frames--;
    }

    // Atualizar apontadores da lista
    (*frameAtual)->next = NULL;
    listaFrames->tail = *frameAtual;

    // Caso nenhuma colisão tenha ocorrido
    if (frameCount == 0)
        printf("Nenhuma colisão prevista.\n");
}

/**
 * @brief Deteta e remove barcos que colidiram no mesmo frame.
 *
 * Percorre a lista de barcos num frame e identifica posições partilhadas por mais de um barco
 * (excluindo certos tipos como tipo 1 e submarinos invisíveis). Para cada colisão:
 * - Cria um registo de colisão com os IDs dos barcos envolvidos.
 * - Remove os barcos da lista original.
 * - Devolve uma lista ligada com os dados das colisões.
 *
 * @param lista Ponteiro para a lista ligada de entidades (barcos) no frame.
 * @param showOutput Se diferente de zero, imprime as colisões encontradas.
 * @return Lista ligada com as colisões detetadas (ou NULL se não houver colisões).
 */
Colisao *removerBarcosEmColisao(EntidadeIED **lista, int showOutput) {
    EntidadeIED *a = *lista;
    Colisao *colisoes = NULL;
    Colisao *ultimaColisao = NULL;

    // Percorre todos os barcos da lista
    while (a != NULL) {
        int x = a->posicao[0];
        int y = a->posicao[1];

        // Contadores e lista temporária para barcos em colisão
        int count = 0;
        EntidadeIED *aux = *lista;
        BarcosEmColisao *listaBarcos = NULL;
        BarcosEmColisao *ultimoBarco = NULL;

        // Percorre todos os barcos para encontrar outros na mesma posição
        while (aux != NULL) {
            int tipo = aux->no_nautico->tipologia;
            int visivel = aux->no_nautico->isVisible;

            // Só considera barcos não invisíveis e exclui tipo 1
            if (aux->posicao[0] == x && aux->posicao[1] == y &&
                tipo != 1 && !(tipo == 3 && visivel == 0)) {

                // Criar novo nó para a lista de barcos colididos
                BarcosEmColisao *b = malloc(sizeof(BarcosEmColisao));
                if (!b) {
                    perror("Erro ao alocar BarcosEmColisao");
                    exit(1);
                }
                b->id = aux->no_nautico->nome;
                b->seguinte = NULL;

                if (listaBarcos == NULL)
                    listaBarcos = b;
                else
                    ultimoBarco->seguinte = b;

                ultimoBarco = b;
                count++;
            }
            aux = aux->seguinte;
        }

        // Se mais de um barco está na mesma posição, há colisão
        if (count > 1) {
            EntidadeIED *curr;
            EntidadeIED *prev;

            // Criar estrutura Colisao e adicionar à lista
            Colisao *nova = malloc(sizeof(Colisao));
            if (!nova) {
                perror("Erro ao alocar Colisao");
                exit(1);
            }

            nova->x = x;
            nova->y = y;
            nova->barcos = listaBarcos;
            nova->seguinte = NULL;

            if (colisoes == NULL)
                colisoes = nova;
            else
                ultimaColisao->seguinte = nova;

            ultimaColisao = nova;

            // Remover barcos colididos da lista original
            curr = *lista;
            prev = NULL;
            while (curr != NULL) {
                EntidadeIED *seguinte = curr->seguinte;
                int tipo = curr->no_nautico->tipologia;
                int visivel = curr->no_nautico->isVisible;

                if (curr->posicao[0] == x && curr->posicao[1] == y &&
                    tipo != 1 && !(tipo == 3 && visivel == 0)) {

                    if (showOutput) {
                        printf("\033[1;31mBarco %c colidiu em (%d,%d)\033[0m\n",
                               curr->no_nautico->nome, x, y);
                    }

                    // Remoção do nó da lista ligada
                    if (prev == NULL)
                        *lista = seguinte;
                    else
                        prev->seguinte = seguinte;

                    free(curr);
                } else {
                    prev = curr;
                }

                curr = seguinte;
            }

            // Reinicia o scan desde o início (a lista foi modificada)
            a = *lista;
        } else {
            // Nenhuma colisão: libertar listaBarcos temporária
            while (listaBarcos != NULL) {
                BarcosEmColisao *tmp = listaBarcos;
                listaBarcos = listaBarcos->seguinte;
                free(tmp);
            }
            a = a->seguinte;
        }
    }

    return colisoes;
}

/**
 * @brief Recuar a simulação um número definido de frames.
 *
 * Esta função recua o ponteiro do frame atual um número especificado de passos (frames anteriores),
 * desde que existam frames anteriores disponíveis. Após recuar, todos os frames futuros são eliminados
 * da memória e o estado do novo frame atual é guardado no ficheiro "depois.txt".
 *
 * @param frameAtual Ponteiro para o ponteiro do frame atual da simulação.
 * @param listaFrames Ponteiro para a estrutura que contém os limites da lista de frames.
 * @param steps Número de frames a recuar.
 * @param showOutput Flag que indica se deve ser impresso feedback ao utilizador.
 */
void rewindFrames(BaseDados **frameAtual, ListaFrames *listaFrames, int steps) {
    // Verifica se o ponteiro para o frame atual é válido
    if (frameAtual == NULL || *frameAtual == NULL) {
        printf("Frame atual inválido.\n");
        return;
    }

    // Tenta recuar 'steps' vezes para frames anteriores
    for (int i = 0; i < steps; i++) {
        // Se não houver frame anterior, interrompe
        if ((*frameAtual)->prev == NULL) {
            printf("Não existem frames anteriores ao Frame 0.\n");
            break;
        }

        // Atualiza o frame atual para o anterior
        *frameAtual = (*frameAtual)->prev;
    }

    // Remove da memória todos os frames que vinham depois do novo frame atual
    apagarFramesFuturos(frameAtual, listaFrames);
}

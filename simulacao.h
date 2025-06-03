#ifndef SIMULACAO_H
#define SIMULACAO_H

// ================================================ SIMULACAO ==========================================================

/**
 * @brief Atualiza a simulação avançando um número de frames.
 */
Colisao *avancarFrame(BaseDados **frameAtual, ListaFrames *listaFrames,
                      int numFrames, int latitudeMax, int longitudeMax, int showOutput);

/**
 * @brief Reverte o estado da simulação para frames anteriores.
 */
void rewindFrames(BaseDados **frameAtual, ListaFrames *listaFrames, int steps);

/**
 * @brief Deteta e remove barcos em colisão. Retorna lista de colisões.
 */
Colisao *removerBarcosEmColisao(EntidadeIED **lista, int showOutput);

/**
 * @brief Corre previsão automática de colisões até ao fim da simulação.
 */
void previsaoDeColisoes(BaseDados **frameAtual, ListaFrames *listaFrames,
                        int latitudeMax, int longitudeMax);

/**
 * @brief Verifica se há barcos a uma determinada distância.
 */
int temBarcosADistancia(BaseDados *frame, EntidadeIED *barco, int n);

#endif

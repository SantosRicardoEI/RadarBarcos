#ifndef MEMORIA_H
#define MEMORIA_H

// ================================================ MEMORIA ============================================================

/**
 * @brief Guarda o estado atual num ficheiro.
 */
void guardarFrameNoFicheiro(BaseDados *frameAtual, int showOutput);

/**
 * @brief Liberta toda a memória alocada dinamicamente a partir do frame 0.
 */
void limparFrameInicial(BaseDados *frameZero);

/**
 * @brief Apaga os frames seguintes e liberta a sua memória.
 */
void apagarFramesFuturos(BaseDados **frameAtual, ListaFrames *listaFrames);

#endif //MEMORIA_H

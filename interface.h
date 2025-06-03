#ifndef INTERFACE_H
#define INTERFACE_H

// ================================================ INTERFACE ==========================================================

/**
 * @brief Pergunta por um barco e imprime a velocidade média.
 */
void pedeVelocidadeBarco(BaseDados **frameAtual);

/**
 * @brief Calcula e imprime a velocidade média de um barco.
 */
void imprimirVelocidadeBarco(BaseDados **frameAtual, char barco);

/**
 * @brief Função para pedir os dados ao utilizador e inserir/alterar barco.
 */
void inserirOuAlterarBarco(int lat, int lon, BaseDados *frameAtual);

/**
 * @brief Pergunta ao utilizador quantos frames deve avançar.
 */
void pedeAtualizarSimulacao(BaseDados **frameAtual, ListaFrames *listaFrames,
                             int latMax, int lonMax);

/**
 * @brief Pergunta ao utilizador quantos frames quer recuar.
 */
void rastrearHistoricoReverso(BaseDados **frameAtual, ListaFrames *listaFrames);

#endif //INTERFACE_H

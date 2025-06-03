#ifndef INPUT_H
#define INPUT_H

// ================================================ INPUT ==============================================================

/**
 * @brief Lê e valida argumentos da linha de comandos.
 */
void lerArgsMain(int argc, char *argv[],
                 char **ficheiro_entrada, int *linhas, int *colunas,
                 int *numFrames, char **ficheiro_saida);

/**
 * @brief Lê os dados iniciais do ficheiro e preenche o frame 0.
 */
void lerFicheiroInicial(const char *ficheiro, BaseDados *frame);

#endif //INPUT_H

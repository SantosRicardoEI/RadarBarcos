#include "modulo.h"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define RESET   "\033[0m"

/*
 * ===============================================================
 *
 *  === USO DE LLMS ===
 *
 * Parte da documentação deste código (anotações em estilo Javadoc)
 * e da redação de comentários explicativos foi auxiliada por LLMs,
 * nomeadamente o ChatGPT.
 *
 * Esta assistência teve como principal objetivo facilitar e agilizar
 * a compreensão do código por parte dos docentes responsáveis pela
 * avaliação, recorrendo a explicações mais claras, diretas e coerentes.
 *
 * Serviu também como apoio para estruturar melhor o raciocínio e
 * manter o código mais organizado e legível.
 *
 * Em momentos pontuais, o ChatGPT foi ainda utilizado para identificar
 * e resolver erros de execução relacionados com gestão de memória,
 * como problemas de libertação de ponteiros não alocados. Esse apoio foi sempre
 * utilizado de forma crítica e orientada à resolução de situações específicas.
 *
 * Adicionalmente, o ChatGPT auxiliou na organização estrutural do projeto,
 * nomeadamente na separação do código em múltiplos ficheiros e na correta
 * gestão de cabeçalhos e includes.
 *
 *
 * === Debug e script python
 *
 * Foram ainda adicionadas duas funcionalidades ao menu:
 * - Uma opção de ativação do modo de debug, que imprime os barcos presentes a cada frame;
 * - Uma opção que executa um script Python fornecido pelos professores (colocado na pasta
 *   do projeto), o qual gera um gráfico
 *   com a trajetória dos barcos entre o ficheiro de entrada e o ficheiro de saída.
 *
 * ================================================================
 */


// ================================================ MAIN ===============================================================

// Compilar:
// gcc main.c impressao.c input.c interface.c memoria.c simulacao.c -Wall -Wextra -g -Wvla -Wpedantic -Wdeclaration-after-statement -lm -o radar

// Run e Detetar leaks
// leaks --atExit -- ./radar antes.txt 120x120 0 depois.txt

/**
 * @brief Função principal da aplicação de simulação de radar marítimo.
 *
 * Esta função coordena a leitura de argumentos, a criação dos frames iniciais,
 * a simulação dos movimentos das embarcações, e apresenta um menu interativo
 * para manipulação da simulação.
 *
 * @param argc Número de argumentos recebidos na linha de comandos.
 * @param argv Vetor de strings com os argumentos da linha de comandos.
 * @return int Código de saída da aplicação (0 em caso de sucesso).
 */
int main(int argc, char *argv[]) {
    // Variáveis
    char *ficheiro_entrada, *ficheiro_saida;
    int latitudeMax, longitudeMax, numFrames;
    int debugEnable = 0;
    int opcao;

    // Estruturas de dados
    BaseDados frameInicial = {.frame_atual_num = 0};
    ListaFrames listaFrames = {0};
    BaseDados *frameAtual = &frameInicial;

    // Ler arumentos e ficheiro de input
    lerArgsMain(argc, argv, &ficheiro_entrada, &latitudeMax, &longitudeMax,
                &numFrames, &ficheiro_saida);
    lerFicheiroInicial(ficheiro_entrada, &frameInicial);

    // Imprimir info debug (leitura dos argumentos)
    if (debugEnable) {
        printf(YELLOW);
        printf("--- DEBUG: Main args ---\n");
        printf("%-20s %s\n", "Ficheiro entrada:", ficheiro_entrada);
        printf("%-20s %d\n", "Linhas:", latitudeMax);
        printf("%-20s %d\n", "Colunas:", longitudeMax);
        printf("%-20s %d\n", "Numero de frames:", numFrames);
        printf("%-21s %s\n\n", "Ficheiro saída:", ficheiro_saida);
        printf(RESET);
    }

    // Inicio e fim da lista de frames
    listaFrames.head = &frameInicial;
    listaFrames.tail = &frameInicial;

    // Avanaçar simulação de acordo com os argumentos da main
    avancarFrame(&frameAtual, &listaFrames, numFrames, latitudeMax, longitudeMax, 1);
    guardarFrameNoFicheiro(frameAtual, 1);

    if (numFrames > 0)
        printf("Simulação atualizada para o frame %d\n", frameAtual->frame_atual_num);

    // ========================================= LOOP MENU =============================================================

    do {
        // Imprimo infos de debug
        char debug[20];
        strcpy(debug, debugEnable ? "Ativo" : "Desativado");
        printf(YELLOW);
        printf("\nDebug %s", debug);
        if (debugEnable) {
            printf(" | Frame %d", frameAtual->frame_atual_num);
        }
        printf(RESET);


        imprimirMenu();

        // Ler a opcao introduzida
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = -1;
        }


        switch (opcao) {
            case 1:
                // Pergunta quantos frames avançar na simulação, gera-os, avança e guarda o frame no ficheiro output
                pedeAtualizarSimulacao(&frameAtual, &listaFrames, latitudeMax, longitudeMax);
                guardarFrameNoFicheiro(frameAtual, 1);
                break;

            case 2:
                // Pergunta qual barco inserir ou alterar e insere/altera
                inserirOuAlterarBarco(latitudeMax, longitudeMax, frameAtual);
                break;

            case 3:
                // Mostra em quais frames haverão colisões e qual o ponto e barcos envolvidos
                previsaoDeColisoes(&frameAtual, &listaFrames, latitudeMax, longitudeMax);
                break;

            case 4:
                // Pergunta quantos frames recuar, recua esse número de frames, "apaga" os frames à frente desse
                // e guarda o frame no ficheiro output
                rastrearHistoricoReverso(&frameAtual, &listaFrames);
                imprimirFrameAtual(frameAtual);
                guardarFrameNoFicheiro(frameAtual, 1);
                break;

            case 5:
                // Pergunta qual barco e dá as suas estatísticas
                pedeVelocidadeBarco(&frameAtual);
                break;

            case 6: {
                system("python3 radarbarcos.py");
                break;
            }

            case 7: {
                // Alterna o debugMode
                debugEnable = !debugEnable;
                break;
            }

            case 0:
                // Guarda o frame atual no ficheiro de output
                guardarFrameNoFicheiro(frameAtual, 1);
                printf("A sair do programa...\n");
                break;

            default:
                printf("Opção inválida.\n");
        }

        // Mostra info. dos barcos a cada iteração do loop (se debug está ativo)
        if (debugEnable) {
            printf(YELLOW);
            printf("\n--- DEBUG ---");
            imprimirBarcosDebug(frameAtual);
            printf(RESET);
        }
    } while (opcao != 0);

    // Volta para o frame 0 (liberta todos os proximos)
    rewindFrames(&frameAtual, &listaFrames, frameAtual->frame_atual_num);
    // Liberto o frame 0
    limparFrameInicial(&frameInicial);

    return 0;
}

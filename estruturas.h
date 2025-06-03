#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// ================================================ ESTRUTURAS =========================================================

/**
 * @brief Estrutura que representa um navio.
 *
 * Cada navio é identificado por um nome (caractere), tem uma tipologia (número identificador)
 * e um indicador de visibilidade no radar (utilizado para o submarino).
 */
typedef struct NoVessel {
    char nome;         /**< Identificador único do navio (ex: 'A', 'B') */
    int tipologia;     /**< Tipo do navio (ex: cruzador, pescador, etc.) */
    int isVisible;     /**< Indicador de visibilidade no radar (1 = visível, 0 = invisível) */
} NoVessel;

/**
 * @brief Estrutura que representa a instância de um navio num determinado frame.
 *
 * Esta estrutura liga-se em lista para representar todos os navios num frame específico.
 */
typedef struct EntidadeIED {
    int posicao[2];               /**< Posição do navio no radar (x, y) */
    int velocidade[2];           /**< Velocidade do navio em cada eixo (vx, vy) */
    NoVessel *no_nautico;        /**< Ponteiro para a estrutura estática do navio */
    struct EntidadeIED *seguinte;/**< Ponteiro para a próxima entidade no frame */
} EntidadeIED;

/**
 * @brief Estrutura que representa um frame da simulação.
 *
 * Contém todas as entidades ativas no radar nesse instante.
 */
typedef struct BaseDados {
    int frame_atual_num;         /**< Número identificador do frame */
    EntidadeIED *barcos;         /**< Lista de entidades (navios) presentes no frame */
    struct BaseDados *prev;      /**< Ponteiro para o frame anterior */
    struct BaseDados *next;      /**< Ponteiro para o frame seguinte */
} BaseDados;

/**
 * @brief Estrutura auxiliar para aceder rapidamente ao início e fim da lista de frames.
 */
typedef struct ListaFrames {
    BaseDados *head;             /**< Ponteiro para o primeiro frame (frame inicial) */
    BaseDados *tail;             /**< Ponteiro para o último frame gerado */
    int total_frames;            /**< Total de frames existentes na simulação */
} ListaFrames;

/**
 * @brief Estrutura que representa um navio envolvido numa colisão.
 *
 * Ligada em lista para representar todos os navios envolvidos numa colisão específica.
 */
typedef struct BarcosEmColisao {
    char id;                     /**< Identificador do navio envolvido na colisão */
    struct BarcosEmColisao *seguinte; /**< Ponteiro para o próximo navio em colisão */
} BarcosEmColisao;

/**
 * @brief Estrutura que representa uma colisão entre navios.
 *
 * Contém a posição da colisão e a lista de navios envolvidos.
 */
typedef struct Colisao {
    int x, y;                    /**< Coordenadas da colisão */
    BarcosEmColisao *barcos;    /**< Lista de navios envolvidos na colisão */
    struct Colisao *seguinte;   /**< Ponteiro para a próxima colisão na lista */
} Colisao;

#endif

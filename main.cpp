#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>

#include "cabecalho.h"

#define MAX(X,Y) ((X > Y) ? X : Y)
#define MIN(X,Y) ((X < Y) ? X : Y)

int INST      = 1; // 1, 2, 3 ou 4 (usada quando nenhum arquivo e passado na linha de comando)
double beta   = 1;
double alfa   = 0.75;
double lambda = 1;

static void executar(const char* nome, Solucao& s, void (*bl)(Solucao&))
{
    clock_t ini = clock();
    bl(s);
    double tempo = (double)(clock() - ini) / CLOCKS_PER_SEC;
    printf("\n\n%s (%.3f s):", nome, tempo);
    escrever_solucao(s);
}

static void bl_rand_padrao(Solucao& s) { heu_BL_rand(s, 1000); }

int main(int argc, char* argv[])
{
    Solucao s, s_ini;
    char arq[256];
    if (argc > 1)
        snprintf(arq, sizeof(arq), "%s", argv[1]);
    else
        snprintf(arq, sizeof(arq), "inst%d.txt", INST);

    srand(time(NULL));
    ler_dados(arq);
    //testar_dados("");

    // ---------- Construtivas ----------
    heu_cons_ale(s);
    calcular_fo(s);
    printf("\nConstrutiva aleatoria:");
    escrever_solucao(s);
    memcpy(&s_ini, &s, sizeof(s));

    heu_cons_gul(s);
    calcular_fo(s);
    printf("\n\nConstrutiva gulosa:");
    escrever_solucao(s);

    heu_cons_ale_gul(s, 0.1);
    calcular_fo(s);
    printf("\n\nConstrutiva aleatoria gulosa (10%%):");
    escrever_solucao(s);

    // ---------- Buscas locais (todas partem da mesma solucao aleatoria) ----------
    memcpy(&s, &s_ini, sizeof(s));
    executar("BL randomica (1000 iter)", s, bl_rand_padrao);

    memcpy(&s, &s_ini, sizeof(s));
    executar("BL melhor melhora", s, heu_BL_MM);

    memcpy(&s, &s_ini, sizeof(s));
    executar("BL primeira melhora", s, heu_BL_PM);

    printf("\n");
    return 0;
}

// =====================================================================
// Avaliacao
// =====================================================================

// Calcula a FO considerando apenas os qtd_hubs primeiros nos de vet_sol como hubs.
// Custo de i para j (multipla alocacao): min_{k,l hubs} beta*d(i,k) + alfa*d(k,l) + lambda*d(l,j)
// FO = maior custo entre todos os pares i != j.
double avaliar(const Solucao& s, const int& qtd_hubs)
{
    static double custo_ate_hub[MAX_NOS][MAX_NOS]; // [no i][indice do hub l] = min_k beta*d(i,k) + alfa*d(k,l)
    const int* h = s.vet_sol;

    for (int i = 0; i < num_nos; i++)
        for (int l = 0; l < qtd_hubs; l++)
        {
            double melhor = DBL_MAX;
            for (int k = 0; k < qtd_hubs; k++)
                melhor = MIN(melhor, beta * mat_distancias[i][h[k]] + alfa * mat_distancias[h[k]][h[l]]);
            custo_ate_hub[i][l] = melhor;
        }

    double fo = 0;
    for (int i = 0; i < num_nos; i++)
        for (int j = 0; j < num_nos; j++)
        {
            if (i == j)
                continue;
            double melhor = DBL_MAX;
            for (int l = 0; l < qtd_hubs; l++)
                melhor = MIN(melhor, custo_ate_hub[i][l] + lambda * mat_distancias[h[l]][j]);
            fo = MAX(fo, melhor);
        }
    return fo;
}

void calcular_fo(Solucao& s)
{
    s.fo = avaliar(s, num_hub);
}

// =====================================================================
// Heuristicas construtivas
// =====================================================================

// Preenche o vetor de solucao aleatoriamente (as num_hub primeiras posicoes sao os hubs)
void heu_cons_ale(Solucao &s)
{
    for (int i = 0; i < num_nos; i++)
        s.vet_sol[i] = i;
    for (int i = 0; i < num_nos; i++)
    {
        int pos = i + rand() % (num_nos - i);
        int aux = s.vet_sol[i];
        s.vet_sol[i] = s.vet_sol[pos];
        s.vet_sol[pos] = aux;
    }
}

// Avalia cada candidato (posicoes pos..num_nos-1) como o (pos+1)-esimo hub
static void avaliar_candidatos(Solucao& s, const int& pos, double* custos)
{
    for (int c = pos; c < num_nos; c++)
    {
        int aux = s.vet_sol[pos]; s.vet_sol[pos] = s.vet_sol[c]; s.vet_sol[c] = aux;
        custos[c] = avaliar(s, pos + 1);
        aux = s.vet_sol[pos]; s.vet_sol[pos] = s.vet_sol[c]; s.vet_sol[c] = aux;
    }
}

// A cada passo, adiciona como hub o no que resulta na menor FO parcial
void heu_cons_gul(Solucao &s)
{
    double custos[MAX_NOS];
    for (int i = 0; i < num_nos; i++)
        s.vet_sol[i] = i;
    for (int pos = 0; pos < num_hub; pos++)
    {
        avaliar_candidatos(s, pos, custos);
        int melhor = pos;
        for (int c = pos + 1; c < num_nos; c++)
            if (custos[c] < custos[melhor])
                melhor = c;
        int aux = s.vet_sol[pos]; s.vet_sol[pos] = s.vet_sol[melhor]; s.vet_sol[melhor] = aux;
    }
}

// A cada passo, ordena os candidatos pela FO parcial e sorteia um entre os
// "percentual" (0 a 1) melhores (lista restrita de candidatos)
void heu_cons_ale_gul(Solucao &s, const double& percentual)
{
    double custos[MAX_NOS];
    int ordem[MAX_NOS];
    for (int i = 0; i < num_nos; i++)
        s.vet_sol[i] = i;
    for (int pos = 0; pos < num_hub; pos++)
    {
        avaliar_candidatos(s, pos, custos);
        int qtd = num_nos - pos;
        for (int c = 0; c < qtd; c++)
            ordem[c] = pos + c;
        // ordenacao por insercao (crescente pela FO parcial)
        for (int a = 1; a < qtd; a++)
        {
            int x = ordem[a], b = a - 1;
            while (b >= 0 && custos[ordem[b]] > custos[x])
            {
                ordem[b + 1] = ordem[b];
                b--;
            }
            ordem[b + 1] = x;
        }
        int tam_lrc = MAX(1, (int)ceil(percentual * qtd));
        tam_lrc = MIN(tam_lrc, qtd);
        int escolhido = ordem[rand() % tam_lrc];
        int aux = s.vet_sol[pos]; s.vet_sol[pos] = s.vet_sol[escolhido]; s.vet_sol[escolhido] = aux;
    }
}

// =====================================================================
// Heuristicas de refinamento
// Vizinhanca: troca de um hub (posicao < num_hub) por um nao-hub (posicao >= num_hub)
// =====================================================================

static inline void trocar(Solucao& s, const int& a, const int& b)
{
    int aux = s.vet_sol[a];
    s.vet_sol[a] = s.vet_sol[b];
    s.vet_sol[b] = aux;
}

// Gera "iter" vizinhos aleatorios; aceita apenas os que melhoram a FO
void heu_BL_rand(Solucao& s, const int& iter)
{
    if (num_hub == 0 || num_hub == num_nos)
        return;
    calcular_fo(s);
    for (int t = 0; t < iter; t++)
    {
        int h = rand() % num_hub;
        int n = num_hub + rand() % (num_nos - num_hub);
        trocar(s, h, n);
        double fo = avaliar(s, num_hub);
        if (fo < s.fo)
            s.fo = fo;
        else
            trocar(s, h, n); // desfaz
    }
}

// Melhor melhora: avalia toda a vizinhanca e aplica o melhor movimento,
// repetindo ate nao haver melhora
void heu_BL_MM(Solucao& s)
{
    calcular_fo(s);
    while (true)
    {
        int melhor_h = -1, melhor_n = -1;
        double melhor_fo = s.fo;
        for (int h = 0; h < num_hub; h++)
            for (int n = num_hub; n < num_nos; n++)
            {
                trocar(s, h, n);
                double fo = avaliar(s, num_hub);
                if (fo < melhor_fo)
                {
                    melhor_fo = fo;
                    melhor_h = h;
                    melhor_n = n;
                }
                trocar(s, h, n);
            }
        if (melhor_h == -1)
            break;
        trocar(s, melhor_h, melhor_n);
        s.fo = melhor_fo;
    }
}

// Primeira melhora: aplica o primeiro movimento que melhora a FO e reinicia,
// repetindo ate nao haver melhora
void heu_BL_PM(Solucao& s)
{
    calcular_fo(s);
    bool melhorou = true;
    while (melhorou)
    {
        melhorou = false;
        for (int h = 0; h < num_hub && !melhorou; h++)
            for (int n = num_hub; n < num_nos && !melhorou; n++)
            {
                trocar(s, h, n);
                double fo = avaliar(s, num_hub);
                if (fo < s.fo)
                {
                    s.fo = fo;
                    melhorou = true;
                }
                else
                    trocar(s, h, n);
            }
    }
}

// =====================================================================
// Entrada / saida
// =====================================================================

void escrever_solucao(Solucao& s)
{
    printf("\nFO: %.2f\nHUBS: ", s.fo);
    for (int i = 0; i < num_hub; i++)
        printf("%d  ", s.vet_sol[i]);
}

// Passe "" para escrever na tela
void testar_dados(const char* arq)
{
    FILE* f;
    if (strcmp(arq, "") == 0)
        f = stdout;
    else
        f = fopen(arq, "w");
    if (!f)
        return;
    fprintf(f, "%d\t%d\n", num_nos, num_hub);
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j < num_nos; j++)
            fprintf(f, "%-10.2f\t", mat_distancias[i][j]);
        fprintf(f, "\n");
    }
    if (f != stdout)
        fclose(f);
}

void ler_dados(const char* arq)
{
    double vet_x[MAX_NOS], vet_y[MAX_NOS];
    FILE* f = fopen(arq, "r");
    if (!f)
    {
        printf("Erro ao abrir o arquivo %s\n", arq);
        exit(1);
    }
    if (fscanf(f, "%d %d", &num_nos, &num_hub) != 2 || num_nos > MAX_NOS || num_hub > num_nos)
    {
        printf("Cabecalho invalido em %s\n", arq);
        exit(1);
    }
    for (int i = 0; i < num_nos; i++)
        if (fscanf(f, "%lf %lf", &vet_x[i], &vet_y[i]) != 2)
        {
            printf("Coordenadas invalidas em %s\n", arq);
            exit(1);
        }
    fclose(f);
    memset(&mat_distancias, 0, sizeof(mat_distancias));
    for (int i = 0; i < num_nos; i++)
        for (int j = 0; j < num_nos; j++)
            mat_distancias[i][j] = sqrt((vet_x[i] - vet_x[j]) * (vet_x[i] - vet_x[j]) + (vet_y[i] - vet_y[j]) * (vet_y[i] - vet_y[j]));
}

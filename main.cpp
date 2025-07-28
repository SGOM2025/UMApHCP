#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#include "cabecalho.h"

#define MAX(X,Y) ((X > Y) ? X : Y)
#define MIN(X,Y) ((X < Y) ? X : Y)

int INST      = 1; // 1, 2, 3 ou 4
double beta   = 1;
double alfa   = 0.75;
double lambda = 1;

int main()
{
    Solucao s;
    char aux[50];
    sprintf(aux, "inst%d.txt", INST);
    ler_dados(aux);
    //testar_dados("");
    criar_solucao(s);
    calcular_fo(s);
    printf("\n\nSolucao inicial:");
    escrever_solucao(s);
    //---

	// UTILIZE ESSA �REA PARA TESTE

    //---
    return 0;
}

//----------------------------------------------------------------------------------------------------------


// INSIRA SEU C�DIGO AQUI


//----------------------------------------------------------------------------------------------------------

void criar_solucao(Solucao& s)
{
    int vet_aux[MAX_NOS];
    for (int i = 0; i < num_nos; i++)
        vet_aux[i] = i;
    // Preenche o vetor de solu��o aleatoriamente, sendo as num_hub primeiras posi��es indicando os n�s hubs
    for (int i = 0; i < num_nos; i++)
    {
        int pos = i + rand() % (num_nos - i);
        s.vet_sol[i] = vet_aux[pos];
        int aux = vet_aux[i];
        vet_aux[i] = vet_aux[pos];
        vet_aux[pos] = aux;
    }
}

void calcular_fo(Solucao& s)
{
    s.fo = 0;
    // Inicializar os custos entre n�s n�o-hubs (n�o-hubs x n�o-hubs)
    for (int i = num_hub; i < num_nos; i++)
        for (int j = i; j < num_nos; j++)
        {
            mat_custo[s.vet_sol[i]][s.vet_sol[j]] = INT_MAX;
            mat_custo[s.vet_sol[j]][s.vet_sol[i]] = INT_MAX;
        }
    // Inicializar os custos (hubs x hubs) e (hubs x n�o-hubs)
    for (int k = 0; k < num_hub; k++)
    {
        // Hubs
        for (int l = k + 1; l < num_hub; l++)
        {
             mat_custo[s.vet_sol[k]][s.vet_sol[l]] = alfa * mat_distancias[s.vet_sol[k]][s.vet_sol[l]];
             mat_custo[s.vet_sol[l]][s.vet_sol[k]] = alfa * mat_distancias[s.vet_sol[l]][s.vet_sol[k]];
        }
        // N�o-hubs
        for (int i = num_hub; i < num_nos; i++)
        {
             mat_custo[s.vet_sol[k]][s.vet_sol[i]] = lambda * mat_distancias[s.vet_sol[k]][s.vet_sol[i]];
             mat_custo[s.vet_sol[i]][s.vet_sol[k]] = beta * mat_distancias[s.vet_sol[i]][s.vet_sol[k]];
        }
    }
    // Atualizar os custos (hubs x n�o-hubs) passando por 1 hub intermedi�rio
    for (int k = 0; k < num_hub; k++)
        for (int i = num_hub; i < num_nos; i++)
            for (int l = 0; l < num_hub; l++)
            {
                mat_custo[s.vet_sol[k]][s.vet_sol[i]] = MIN(mat_custo[s.vet_sol[k]][s.vet_sol[i]],
                                                            mat_custo[s.vet_sol[k]][s.vet_sol[l]] + mat_custo[s.vet_sol[l]][s.vet_sol[i]]);
                mat_custo[s.vet_sol[i]][s.vet_sol[k]] = MIN(mat_custo[s.vet_sol[i]][s.vet_sol[k]],
                                                            mat_custo[s.vet_sol[i]][s.vet_sol[l]] + mat_custo[s.vet_sol[l]][s.vet_sol[k]]);
            }
    // Atualizar os custos (n�o-hubs x n�o-hubs) passando por 1 ou 2 hubs intermedi�rio
    for (int i = num_hub; i < num_nos; i++)
    {
        for (int j = i; j < num_nos; j++)
        {
            for (int k = 0; k < num_hub; k++)
            {
               mat_custo[s.vet_sol[i]][s.vet_sol[j]] = MIN(mat_custo[s.vet_sol[i]][s.vet_sol[j]],
                                                           mat_custo[s.vet_sol[i]][s.vet_sol[k]] + mat_custo[s.vet_sol[k]][s.vet_sol[j]]);
               mat_custo[s.vet_sol[j]][s.vet_sol[i]] = MIN(mat_custo[s.vet_sol[j]][s.vet_sol[i]],
                                                           mat_custo[s.vet_sol[j]][s.vet_sol[k]] + mat_custo[s.vet_sol[k]][s.vet_sol[i]]);
               for (int l = 0; l < num_hub; l++)
               {
                   mat_custo[s.vet_sol[i]][s.vet_sol[j]] = MIN(mat_custo[s.vet_sol[i]][s.vet_sol[j]],
                                                               mat_custo[s.vet_sol[i]][s.vet_sol[k]] + mat_custo[s.vet_sol[k]][s.vet_sol[l]] +
                                                               mat_custo[s.vet_sol[l]][s.vet_sol[j]]);
                   mat_custo[s.vet_sol[j]][s.vet_sol[i]] = MIN(mat_custo[s.vet_sol[j]][s.vet_sol[i]],
                                                               mat_custo[s.vet_sol[j]][s.vet_sol[l]] + mat_custo[s.vet_sol[l]][s.vet_sol[k]] +
                                                               mat_custo[s.vet_sol[k]][s.vet_sol[i]]);
               }
            }
        }
    }
    // Calcular a FO
    for (int i = 0; i < num_nos; i++)
        for (int j = i; j < num_nos; j++)
            s.fo = MAX(s.fo, mat_custo[s.vet_sol[i]][s.vet_sol[j]]);
}

void escrever_solucao(Solucao& s)
{
    printf("\n\nFO: %.2f\nHUBS: ", s.fo);
    for (int i = 0; i < num_hub; i++)
        printf("%d  ", s.vet_sol[i]);
}

void testar_dados(char* arq)
{
    FILE* f;
    if (arq == "")
        f = stdout;
    else
        f = fopen(arq, "w");
    fprintf(f, "%d\t%d\n", num_nos, num_hub);
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j < num_nos; j++)
            fprintf(f, "%-10.2f\t", mat_distancias[i][j]);
        fprintf(f, "\n");
    }
    if (arq != "")
        fclose(f);
}

void ler_dados(char* arq)
{
    double vet_x[MAX_NOS], vet_y[MAX_NOS];
    FILE* f = fopen(arq, "r");
    fscanf(f, "%d %d", &num_nos, &num_hub);
    for (int i = 0; i < num_nos; i++)
        fscanf(f, "%lf %lf", &vet_x[i], &vet_y[i]);
    fclose(f);
    memset(&mat_distancias, 0, sizeof(mat_distancias));
    for (int i = 0; i < num_nos; i++)
        for (int j = 0; j < num_nos; j++)
            mat_distancias[i][j] = sqrt((vet_x[i] - vet_x[j]) * (vet_x[i] - vet_x[j]) + (vet_y[i] - vet_y[j]) * (vet_y[i] - vet_y[j]));
}

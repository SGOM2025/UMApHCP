#ifndef CABECALHO_H_INCLUDED
#define CABECALHO_H_INCLUDED

#define MAX_NOS 250

typedef struct tSolucao
{
    int vet_sol[MAX_NOS]; // [num_hub | num_nos - num_hub]
    double fo;
}Solucao;

int num_nos;
int num_hub;
double mat_distancias[MAX_NOS][MAX_NOS];
double mat_custo[MAX_NOS][MAX_NOS];

void calcular_fo(Solucao& s);
void escrever_solucao(Solucao& s);
void testar_dados(char* arq);
void ler_dados(char* arq);

// Heuristicas Construtivas

void heu_cons_ale(Solucao &s);
void heu_cons_gul(Solucao &s);
void heu_cons_ale_gul(Solucao &s, const double& percentual);

#endif

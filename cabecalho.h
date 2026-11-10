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

double avaliar(const Solucao& s, const int& qtd_hubs);
void calcular_fo(Solucao& s);
void escrever_solucao(Solucao& s);
void testar_dados(const char* arq);
void ler_dados(const char* arq);

// Heuristicas Construtivas

void heu_cons_ale(Solucao &s);
void heu_cons_gul(Solucao &s);
void heu_cons_ale_gul(Solucao &s, const double& percentual);

// Heuristicas de refinamento

void heu_BL_rand(Solucao& s, const int& iter);
void heu_BL_MM(Solucao& s);
void heu_BL_PM(Solucao& s);

#endif

# 🌐 Problema p-Hub Centro Não Capacitado de Múltipla Alocação (UMApHCP)

Este repositório contém uma implementação de heurísticas e meta-heurísticas do problema **UMApHCP – Uncapacitated Multiple Allocation p-Hub Center Problem**, desenvolvido para a disciplina de Métodos de Otimização (COM10393) da UFES.

---

## 📌 Descrição do Problema

Dado um conjunto de `n` nós, o objetivo é escolher exatamente `p` hubs, de modo que **o maior custo de transporte entre todos os pares de nós seja minimizado**. A conexão entre dois nós não-hubs **deve passar por hubs**, e o custo de transporte leva em conta:

- Distância entre os nós (distância euclidiana)
- Fatores de ponderação:
  - β = 1 (origem → hub)
  - α = 0.75 (hub → hub)
  - λ = 1 (hub → destino)

Cálculo do custo de transporte entre i e j:

```
tᵢⱼ = β * cᵢₖ + α * cₖₗ + λ * cₗⱼ
```

A função objetivo é:

```
f(s) = max(tᵢⱼ), ∀ i, j ∈ N
```

---

## 💾 Formato das Instâncias

Cada instância tem o seguinte formato:

```
<n> <p>             # número de nós e número de hubs
<x[1]> <y[1]>       # coordenadas do nó 1
...
<x[n]> <y[n]>       # coordenadas do nó n
```

---

## 🧠 Heurísticas Implementadas

A solução é representada por um vetor `vet_sol` em que as `p` primeiras posições são os hubs e as demais são os nós não-hubs.

### Construtivas

- `heu_cons_ale(Solucao &s)` – Aleatória: embaralha os nós e os `p` primeiros viram hubs
- `heu_cons_gul(Solucao &s)` – Gulosa: adiciona um hub por vez, sempre o que resulta na menor FO parcial
- `heu_cons_ale_gul(Solucao &s, const double& percentual)` – Aleatória gulosa: a cada passo, sorteia o hub entre os `percentual` (0 a 1) melhores candidatos

### Refinamento (buscas locais)

Vizinhança: troca de um hub por um nó não-hub.

- `heu_BL_rand(Solucao& s, const int& iter)` – Busca local randômica: gera `iter` vizinhos aleatórios e aceita apenas os que melhoram a FO
- `heu_BL_MM(Solucao& s)` – Melhor melhora: avalia toda a vizinhança e aplica o melhor movimento, até não haver melhora
- `heu_BL_PM(Solucao& s)` – Primeira melhora: aplica o primeiro movimento que melhora a FO, até não haver melhora

Todas recebem a solução por referência e têm retorno `void`.

---

## 🎯 Melhor FO Conhecida (por instância)

| Instância | Melhor FO   |
|-----------|-------------|
| 1         | 24.574,90   |
| 2         | 38.320,25   |
| 3         | 50.707,87   |
| 4         | 51.860,03   |

---

## 🛠️ Execução

Para compilar o código (em C++):

```bash
g++ -O2 main.cpp -o umah
./umah inst1.txt
```

Se nenhum arquivo for passado, é usada a instância definida pela constante `INST` em `main.cpp`. O programa executa as três construtivas e, a partir da mesma solução aleatória, as três buscas locais, exibindo a FO, os hubs e o tempo de cada uma.

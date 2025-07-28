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

- `busca_local_rand(Solucao &s)` – Busca local randômica
- `busca_local_primeira_melhora(Solucao &s)` – Primeira melhora
- `busca_local_melhor_melhora(Solucao &s)` – Melhor melhora

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
g++ nome_sobrenome.cpp -o umah
./umah instancia.txt
```

// =====================================================
// PESSOA 2 - ALGORITMOS: Dijkstra, Kruskal, conexidade
// e programa principal (main)
//
// Compilar: g++ -o eurotrip pessoa2_algoritmos.cpp
// (precisa do arquivo pessoa1_grafo.h na mesma pasta)
// =====================================================

#include <queue>
#include <algorithm>
#include "pessoa1_grafo.h"

// =====================================================
// DIJKSTRA (com vetor "pai" para reconstruir o caminho)
// =====================================================

class Dijkstra {
public:
    static vector<int> dijkstra(const GrafoListaPonderado& grafo, int origem,
                                vector<int>& pai) {
        int numVertices = grafo.numVertices;
        vector<int> distancias(numVertices, INT_MAX);
        vector<bool> visitado(numVertices, false);
        pai.assign(numVertices, -1);

        // Min-heap: (distancia, vertice)
        priority_queue<pair<int, int>, vector<pair<int, int>>,
                       greater<pair<int, int>>> pq;

        distancias[origem] = 0;
        pq.push({0, origem});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (visitado[u]) continue;
            visitado[u] = true;

            for (const auto& vizinho : grafo.obterVizinhos(u)) {
                int v = vizinho.first;
                int peso = vizinho.second;

                if (distancias[u] != INT_MAX &&
                    distancias[u] + peso < distancias[v]) {
                    distancias[v] = distancias[u] + peso;
                    pai[v] = u;
                    pq.push({distancias[v], v});
                }
            }
        }
        return distancias;
    }
};

// =====================================================
// KRUSKAL (usa Union-Find) - devolve as arestas da AGM
// =====================================================

struct ArestaSimples {
    int u;
    int v;
    int peso;
};

class Kruskal {
private:
    const GrafoListaPonderado& grafo;
    vector<int> pai;

    int encontrar(int x) {
        if (pai[x] == x) return x;
        return pai[x] = encontrar(pai[x]);
    }

    bool unir(int a, int b) {
        a = encontrar(a);
        b = encontrar(b);
        if (a == b) return false;
        pai[b] = a;
        return true;
    }

public:
    Kruskal(const GrafoListaPonderado& g) : grafo(g) {}

    vector<ArestaSimples> executar() {
        int n = grafo.numVertices;

        // Montar a lista de arestas (u < v evita repetir)
        vector<ArestaSimples> arestasGrafo;
        for (int u = 0; u < n; u++) {
            for (const auto& vizinho : grafo.obterVizinhos(u)) {
                if (u < vizinho.first) {
                    arestasGrafo.push_back({u, vizinho.first, vizinho.second});
                }
            }
        }

        // Ordenar por peso crescente
        sort(arestasGrafo.begin(), arestasGrafo.end(),
             [](const ArestaSimples& a, const ArestaSimples& b) {
                 return a.peso < b.peso;
             });

        pai.resize(n);
        for (int i = 0; i < n; i++) pai[i] = i;

        vector<ArestaSimples> agm;
        for (const ArestaSimples& e : arestasGrafo) {
            if (unir(e.u, e.v)) {
                agm.push_back(e);
                if ((int)agm.size() == n - 1) break;
            }
        }
        return agm;
    }
};

// =====================================================
// FUNCOES QUE USAM OS ALGORITMOS
// =====================================================

// Verifica conexo: Dijkstra a partir do 0 precisa alcancar todos
bool ehConexo(const GrafoListaPonderado& g) {
    vector<int> pai;
    vector<int> d = Dijkstra::dijkstra(g, 0, pai);
    for (int i = 0; i < N; i++) {
        if (d[i] == INT_MAX) return false;
    }
    return true;
}

void caminhosMinimos(const vector<GrafoListaPonderado>& lista,
                     const vector<GrafoMatrizPonderado>& matriz,
                     int origem, int destino) {
    cout << "=== CAMINHOS MINIMOS: " << nome[origem] << " ate "
         << nome[destino] << " ===" << endl;

    for (int c = 0; c < 3; c++) {
        vector<int> pai;
        Dijkstra::dijkstra(lista[c], origem, pai);

        // Reconstruir o caminho do destino ate a origem
        vector<int> caminho;
        for (int v = destino; v != -1; v = pai[v]) caminho.push_back(v);
        reverse(caminho.begin(), caminho.end());

        // Somar os 3 criterios ao longo do caminho
        int totCusto = 0, totDist = 0, totTempo = 0;
        for (size_t i = 0; i + 1 < caminho.size(); i++) {
            totCusto += matriz[0].obterPeso(caminho[i], caminho[i + 1]);
            totDist  += matriz[1].obterPeso(caminho[i], caminho[i + 1]);
            totTempo += matriz[2].obterPeso(caminho[i], caminho[i + 1]);
        }

        cout << endl << "Menor " << nomeCriterio[c] << ":" << endl << "  ";
        for (size_t i = 0; i < caminho.size(); i++) {
            if (i > 0) cout << " -> ";
            cout << nome[caminho[i]];
        }
        cout << endl << "  Custo: R$ " << totCusto << " | Distancia: " << totDist
             << " km | Tempo: " << formatarTempo(totTempo) << endl;
    }
}

void arvoreGeradoraMinima(const vector<GrafoListaPonderado>& lista,
                          const vector<GrafoMatrizPonderado>& matriz) {
    cout << endl << "=== ARVORE GERADORA MINIMA (Kruskal, pelo custo) ===" << endl;
    Kruskal kruskal(lista[0]);
    vector<ArestaSimples> agm = kruskal.executar();

    int totCusto = 0, totDist = 0, totTempo = 0;
    for (const ArestaSimples& e : agm) {
        cout << "  " << nome[e.u] << " --- " << nome[e.v]
             << " | R$ " << e.peso << endl;
        totCusto += matriz[0].obterPeso(e.u, e.v);
        totDist  += matriz[1].obterPeso(e.u, e.v);
        totTempo += matriz[2].obterPeso(e.u, e.v);
    }
    cout << "  TOTAL -> Custo: R$ " << totCusto << " | Distancia: " << totDist
         << " km | Tempo: " << formatarTempo(totTempo) << endl;
}

// =====================================================
// PROGRAMA PRINCIPAL
// =====================================================

int main() {
    // Um grafo (lista e matriz) para cada criterio
    vector<GrafoListaPonderado> lista(3, GrafoListaPonderado(N));
    vector<GrafoMatrizPonderado> matriz(3, GrafoMatrizPonderado(N));
    montarGrafos(lista, matriz);              // funcao da Pessoa 1

    // Mude para true para ver tambem o grafo original e as 3 matrizes
    const bool MOSTRAR_DETALHES = false;

    if (MOSTRAR_DETALHES) {
        imprimirGrafo();
        cout << "Grafo conexo? " << (ehConexo(lista[0]) ? "SIM" : "NAO") << endl << endl;
        cout << "=== MATRIZES ===" << endl;
        for (int c = 0; c < 3; c++) imprimirMatriz(matriz[c], nomeCriterio[c]);
    }

    // Caminhos minimos: Sao Paulo (0) -> Istambul (6)
    caminhosMinimos(lista, matriz, 0, 6);

    // Arvore Geradora Minima
    arvoreGeradoraMinima(lista, matriz);

    return 0;
}

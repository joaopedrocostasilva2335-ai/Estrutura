#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
#include <climits>
#include <string>

using namespace std;

// =====================================================
// GRAFO COM LISTA DE ADJACENCIA
// =====================================================

class GrafoListaPonderado {
public:
    int numVertices;
    vector<list<pair<int, int>>> listaAdj;

    GrafoListaPonderado(int vertices) : numVertices(vertices) {
        listaAdj.resize(vertices);
    }

    void adicionarAresta(int origem, int destino, int peso) {
        if (origem >= 0 && origem < numVertices &&
            destino >= 0 && destino < numVertices) {
            listaAdj[origem].push_back({destino, peso});
        } else {
            cout << "Erro: vertices invalidos" << endl;
        }
    }

    const list<pair<int, int>>& obterVizinhos(int vertice) const {
        static const list<pair<int, int>> vazio;
        if (vertice >= 0 && vertice < numVertices) {
            return listaAdj[vertice];
        }
        return vazio;
    }
};

// =====================================================
// GRAFO COM MATRIZ DE ADJACENCIA
// =====================================================

class GrafoMatrizPonderado {
public:
    int numVertices;
    vector<vector<int>> matrizAdj;

    GrafoMatrizPonderado(int vertices)
        : numVertices(vertices),
          matrizAdj(vertices, vector<int>(vertices, INT_MAX)) {
    }

    void adicionarAresta(int origem, int destino, int peso) {
        if (origem >= 0 && origem < numVertices &&
            destino >= 0 && destino < numVertices) {
            matrizAdj[origem][destino] = peso;
        } else {
            cout << "Erro: vertices invalidos" << endl;
        }
    }

    int obterPeso(int origem, int destino) const {
        if (origem >= 0 && origem < numVertices &&
            destino >= 0 && destino < numVertices) {
            return matrizAdj[origem][destino];
        }
        return INT_MAX;
    }
};

// =====================================================
// DADOS DA VIAGEM
// =====================================================

const int N = 7;

// Nome de cada vertice (o indice e o numero do vertice)
string nome[N] = {
    "Sao Paulo (Brasil)",
    "Estadio da Luz (Portugal)",
    "Santiago Bernabeu (Espanha)",
    "Wembley (Inglaterra)",
    "San Siro (Italia)",
    "Allianz Arena (Alemanha)",
    "Ataturk Olimpico (Turquia)"
};

// Criterios: 0 = custo, 1 = distancia, 2 = tempo
string nomeCriterio[3] = {"CUSTO (R$)", "DISTANCIA (km)", "TEMPO (min)"};

struct Aresta {
    int u;
    int v;
    int custo;   // R$
    int dist;    // km
    int tempo;   // minutos
};

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
        vector<ArestaSimples> arestas;
        for (int u = 0; u < n; u++) {
            for (const auto& vizinho : grafo.obterVizinhos(u)) {
                if (u < vizinho.first) {
                    arestas.push_back({u, vizinho.first, vizinho.second});
                }
            }
        }

        // Ordenar por peso crescente
        sort(arestas.begin(), arestas.end(),
             [](const ArestaSimples& a, const ArestaSimples& b) {
                 return a.peso < b.peso;
             });

        pai.resize(n);
        for (int i = 0; i < n; i++) pai[i] = i;

        vector<ArestaSimples> agm;
        for (const ArestaSimples& e : arestas) {
            if (unir(e.u, e.v)) {
                agm.push_back(e);
                if ((int)agm.size() == n - 1) break;
            }
        }
        return agm;
    }
};

// =====================================================
// FUNCOES DE IMPRESSAO
// =====================================================

// Mostra o tempo em minutos como "Xh YYmin"
string formatarTempo(int minutos) {
    return to_string(minutos / 60) + "h" +
           (minutos % 60 < 10 ? "0" : "") + to_string(minutos % 60) + "min";
}

void imprimirGrafo(const vector<Aresta>& arestas) {
    cout << "=== GRAFO ORIGINAL ===" << endl;
    for (int i = 0; i < N; i++) {
        cout << "[" << i << "] " << nome[i] << endl;
        int grau = 0;
        for (const Aresta& e : arestas) {
            if (e.u == i || e.v == i) {
                int outro = (e.u == i) ? e.v : e.u;
                cout << "     -> [" << outro << "] " << nome[outro]
                     << " | R$ " << e.custo << " | " << e.dist << " km | "
                     << formatarTempo(e.tempo) << endl;
                grau++;
            }
        }
        cout << "     (grau = " << grau << ")" << endl;
    }
    cout << endl;
}

void imprimirMatriz(const GrafoMatrizPonderado& m, string titulo) {
    cout << "--- Matriz de " << titulo << " (- = sem ligacao direta) ---" << endl;
    cout << setw(5) << " ";
    for (int j = 0; j < N; j++) cout << setw(7) << j;
    cout << endl;
    for (int i = 0; i < N; i++) {
        cout << setw(5) << i;
        for (int j = 0; j < N; j++) {
            if (i == j) cout << setw(7) << 0;
            else if (m.obterPeso(i, j) == INT_MAX) cout << setw(7) << "-";
            else cout << setw(7) << m.obterPeso(i, j);
        }
        cout << endl;
    }
    cout << endl;
}

// Verifica conexo: Dijkstra a partir do 0 precisa alcancar todos
bool ehConexo(const GrafoListaPonderado& g) {
    vector<int> pai;
    vector<int> d = Dijkstra::dijkstra(g, 0, pai);
    for (int i = 0; i < N; i++) {
        if (d[i] == INT_MAX) return false;
    }
    return true;
}

// =====================================================
// PROGRAMA PRINCIPAL
// =====================================================

int main() {
    // Tempo em minutos (ex.: 10,5 h = 630 min)
    vector<Aresta> arestas = {
        //u  v  custo  dist  tempo
        {0, 1, 3500,  7900, 630},   // Sao Paulo - Lisboa
        {0, 2, 3800,  8400, 648},   // Sao Paulo - Madri
        {0, 3, 4200,  9500, 690},   // Sao Paulo - Londres
        {0, 6, 4800, 10400, 840},   // Sao Paulo - Istambul
        {1, 2,  250,   503,  78},   // Lisboa - Madri
        {1, 3,  450,  1590, 168},   // Lisboa - Londres
        {2, 3,  400,  1260, 150},   // Madri - Londres
        {2, 4,  380,  1190, 144},   // Madri - Milao
        {3, 5,  420,   920, 120},   // Londres - Munique
        {3, 4,  400,   960, 126},   // Londres - Milao
        {4, 5,  300,   350,  72},   // Milao - Munique
        {4, 6,  600,  2100, 198},   // Milao - Istambul
        {5, 6,  550,  1850, 180}    // Munique - Istambul
    };

    // Um grafo (lista e matriz) para cada criterio
    vector<GrafoListaPonderado> lista(3, GrafoListaPonderado(N));
    vector<GrafoMatrizPonderado> matriz(3, GrafoMatrizPonderado(N));

    for (const Aresta& e : arestas) {
        int pesos[3] = {e.custo, e.dist, e.tempo};
        for (int c = 0; c < 3; c++) {
            // Grafo nao direcionado: ida e volta
            lista[c].adicionarAresta(e.u, e.v, pesos[c]);
            lista[c].adicionarAresta(e.v, e.u, pesos[c]);
            matriz[c].adicionarAresta(e.u, e.v, pesos[c]);
            matriz[c].adicionarAresta(e.v, e.u, pesos[c]);
        }
    }

    // 1. Grafo original
    imprimirGrafo(arestas);
    cout << "Grafo conexo? " << (ehConexo(lista[0]) ? "SIM" : "NAO") << endl << endl;

    // 2. As 3 matrizes
    cout << "=== MATRIZES ===" << endl;
    for (int c = 0; c < 3; c++) imprimirMatriz(matriz[c], nomeCriterio[c]);

    // 3. Caminhos minimos: Sao Paulo (0) -> Istambul (6)
    int origem = 0, destino = 6;
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

    // 4. Arvore Geradora Minima (Kruskal pelo custo)
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

    return 0;
}

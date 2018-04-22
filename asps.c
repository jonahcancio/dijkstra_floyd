/*
Name: Jonathan Jandel R. Cancio
Student # 2015-08058
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define max_Vs 12
#define max_str 24

typedef struct Graph {
    int adjMatrix[max_Vs][max_Vs];
    int n;
    int dPred[max_Vs];
    int dist[max_Vs];
	int fPred[max_Vs][max_Vs];
	int fMatrix[max_Vs][max_Vs];
} Graph;

typedef struct PriorityQueue{
    int heap[max_Vs];
    int index[max_Vs];
    int key[max_Vs];
    int sizePQ;
} PriorityQueue;

Graph* graph;
PriorityQueue* pq;

void InitGraph(Graph* g){
    int i, j;
    for (i = 0; i < max_Vs; i++) {
        for(j = 0; j < max_Vs; j++){
            g->adjMatrix[i][j] = INT_MAX;
        }
    }
    g->n = 0;
}

void ReadGraph(char* fileName, Graph* g){
    FILE* f = fopen(fileName, "r");
    if (f != NULL){
        char lineString[max_str];
        char *cellToken;
        int r = 1;
        while(fgets(lineString, max_str, f) != NULL){
            cellToken = strtok(lineString, " \n");
            int c = 1;
            while(cellToken != NULL){
                if(strcmp(cellToken, "x")==0){
                    g->adjMatrix[r][c] = INT_MAX;
                }else{
                    g->adjMatrix[r][c] = atoi(cellToken);
                }
                c++;
                cellToken = strtok(NULL, " \n");
            }
            r++;
        }
        g->n = r-1;
    }else{
        printf("File not found man!\n");
    }
}

void PrintGraph(Graph* g) {
    int i, j;
    for (i = 1; i <= g->n; i++) {
        for(j = 1; j <= g->n; j++){
            int x = g->adjMatrix[i][j];
            if(x == INT_MAX){
                printf("x\t");
            }else{
                printf("%d\t", x);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void PrintFloydGraph(Graph* g) {
    int i, j;
    for (i = 1; i <= g->n; i++) {
        for(j = 1; j <= g->n; j++){
            int x = g->fMatrix[i][j];
            if(x == INT_MAX){
                printf("x\t");
            }else{
                printf("%d\t", x);
            }
        }
        printf("\n");
    }
    printf("\n");
}


void InitPQ(Graph* g, PriorityQueue* pq, int s){
    int i = 1;
    int v;
    for (v = 1; v <= g->n; v++) {
        if(v == s){
            pq->heap[1] = s;
            pq->index[s] = 1;
            pq->key[s] = 0;
        }else{
            i++;
            pq->heap[i] = v;
            pq->index[v] = i;
            pq->key[v] = INT_MAX;
        }
    }
    pq->sizePQ = g->n;
}

void PrintPQ(PriorityQueue* pq){
    int i;
    printf("heap:\t");
    for (i = 1; i <= pq->sizePQ ; i++) {
        printf("%4d\t", pq->heap[i]);
    }
    printf("\n");
    printf("index:\t");
    for (i = 1; i <= pq->sizePQ ; i++) {
        printf("%4d\t", pq->index[i]);
    }
    printf("\n");
    printf("key:\t");
    for (i = 1; i <= pq->sizePQ ; i++) {
        printf("%4d\t", pq->key[i]);
    }
    printf("\n");
}

int IsEmptyPQ(PriorityQueue* pq){
    return pq->sizePQ == 0;
}

void Heapify(PriorityQueue* pq, int r){
    int l = pq->heap[r];
    int k = pq->key[l];
    int i = r;
    int j = 2 * i;
    while(j <= pq->sizePQ){
        if(j<pq->sizePQ && pq->key[pq->heap[j+1]] < pq->key[pq->heap[j]]){
            j = j + 1;
        }
        if(pq->key[pq->heap[j]] < k){
            pq->heap[i] = pq->heap[j];
            pq->index[pq->heap[j]] = i;
            i = j;
            j = 2 * i;
        }
        else{
            break;
        }
    }
    pq->heap[i] = l;
    pq->index[l] = i;
}

int ExtractMin(PriorityQueue* pq){
    if(pq->sizePQ == 0){
        printf("PQ Underflow\n");
        return -1;
    }
    int j = pq->heap[1];
    pq->heap[1] = pq->heap[pq->sizePQ];
    pq->index[pq->heap[1]] = 1;
    pq->sizePQ -= 1;
    Heapify(pq, 1);
    return j;
}

int DecreaseKey(PriorityQueue* pq, int l, int newKey) {
    if(pq->key[l] < newKey){
        printf("newKey %d for %d isn't lower than %d\n", newKey, l, pq->key[l]);
        return -1;
    }
    pq->key[l] = newKey;
    int i = pq->index[l];
    int j = i/2;
    while(i > 1 && pq->key[pq->heap[j]] > newKey){
        pq->heap[i] = pq->heap[j];
        pq->index[pq->heap[j]] = i;
        i = j;
        j = i/2;
    }
    pq->heap[i] = l;
    pq->index[l] = i;
    return 0;
}

void Dijkstra(Graph* g, PriorityQueue* pq, int s){
    InitPQ(g, pq, s);
    memset(g->dPred, 0, max_Vs*sizeof(int));
    int u;
    int v;
    int newVal;
    while(IsEmptyPQ(pq) == 0){
        u = ExtractMin(pq);
        if(pq->key[u] == INT_MAX){
            printf("INFINITY: NO CONNECTIONS si %d\n", s);
            break;
        }
        for(v = 1; v <= g->n; v++){
            if(g->adjMatrix[u][v] < INT_MAX){
                newVal = pq->key[u] + g->adjMatrix[u][v];
                if(pq->key[v] > newVal){
                    g->dPred[v] = u;
                    DecreaseKey(pq, v, newVal);
                }
            }
        }
    }
    memcpy(g->dist, pq->key, (g->n+1)*sizeof(int));
}

int DisplayDijkstraPath(Graph* g, int s, int v){
    int path[(g->n)+1];
    int len = 1;
    path[len] = v;
    int i = v;
    while(i != s){
        if(g->dPred[i] == 0){
            printf("No dijkstra path found\n");
            return -1;
        }else{
            i = g->dPred[i];
            len += 1;
            path[len] = i;
        }
    }
    printf("Shortest path found:\t");
    for(i = len; i >= 1; i--){
        printf("%d\t", path[i]);
    }
    printf("\n");
    printf("Cost of shortest found: %d\n", g->dist[v]);
    return 0;
}

void Floyd(Graph* g){
    int i, j;
    for (i = 1; i <= g->n; i++) {
        for (j = 1; j <= g->n; j++) {
            g->fMatrix[i][j] = g->adjMatrix[i][j];
        }
    }
    for (i = 1; i <= g->n; i++) {
        for (j = 1; j <= g->n; j++) {
            if(i == j || g->adjMatrix[i][j] == INT_MAX){
                g->fPred[i][j] = 0;
            }else{
                g->fPred[i][j] = i;
            }
        }
    }

    int k, dijk;
    for (k = 1; k < g->n; k++) {
        for (i = 1; i <= g->n; i++) {
            for (j = 1; j <= g->n; j++) {
                if(g->fMatrix[i][k] == INT_MAX || g->fMatrix[k][j] == INT_MAX){
                    dijk = INT_MAX;
                }else{
                    dijk = g->fMatrix[i][k] + g->fMatrix[k][j];
                }
                if(dijk < g->fMatrix[i][j]){
                    g->fMatrix[i][j] = dijk;
                    g->fPred[i][j] = g->fPred[k][j];
                }
            }
        }
    }
}

 int DisplayFloydPath(Graph* g, int i, int j){
    int path[(g->n)+1];
    int len = 1;
    path[len] = j;
    int k = j;
    while(k != i){
        if(g->fPred[i][k] == 0){
            printf("No floyd path found\n");
            return -1;
        }else{
            k = g->fPred[i][k];
            len++;
            path[len] = k;
        }
    }
    printf("Shortest path found:\t");
    int x;
    for(x = len; x >= 1; x--){
        printf("%d\t", path[x]);
    }
    printf("\n");
    printf("Cost of shortest found: %d\n", g->fMatrix[i][j]);
}

int main(){
    graph = malloc(sizeof(Graph));
    pq = malloc(sizeof(PriorityQueue));
    ReadGraph("graph3.txt", graph);
    printf("HEY: %d\n", max_Vs);
    PrintGraph(graph);
    printf("Dijkstra:\n");
    Dijkstra(graph, pq, 2);
    DisplayDijkstraPath(graph, 2, 8);
    printf("Floyd:\n");
    Floyd(graph);
    DisplayFloydPath(graph, 2, 8);
}

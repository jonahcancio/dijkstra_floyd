#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define max_Vs 11

typedef struct Graph {
    int matrix[max_Vs][max_Vs];
    int n;
    int pred[max_Vs];
    int dist[max_Vs];
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
            g->matrix[i][j] = INT_MAX;
        }
    }
    g->n = 0;
}


void PrintGraph(Graph* g) {
    int i, j;
    for (i = 1; i <= g->n; i++) {
        for(j = 1; j <= g->n; j++){
            printf("%d\t", g->matrix[i][j]);
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

void Dijkstra(Graph* g, int s){
    InitPQ(g, pq, s);
    memset(g->pred, 0, max_Vs*sizeof(int));
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
            if(g->matrix[u][v] < INT_MAX){
                newVal = pq->key[u] + g->matrix[u][v];
                if(pq->key[v] > newVal){
                    g->pred[v] = u;
                    DecreaseKey(pq, v, newVal);
                }
            }
        }
    }
    memcpy(g->dist, pq->key, (g->n+1)*sizeof(int));
}

int DisplayPath(Graph* g, int s, int v){
    int path[(g->n)+1];
    int len = 1;
    path[len] = v;
    int i = v;
    while(i != s){
        if(g->pred[i] == 0){
            printf("No path found");
            return -1;
        }else{
            i = g->pred[i];
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

int main(){
    graph = malloc(sizeof(Graph));
    pq = malloc(sizeof(PriorityQueue));
    InitGraph(graph);
    graph->n = 4;
    graph->matrix[1][1] = 0;
    graph->matrix[2][2] = 0;
    graph->matrix[3][3] = 0;
    graph->matrix[4][4] = 0;
    graph->matrix[1][3] = 6;
    graph->matrix[3][1] = 6;
    graph->matrix[1][2] = 4;
    graph->matrix[2][1] = 4;
    graph->matrix[3][4] = 1;
    graph->matrix[4][3] = 1;
    graph->matrix[2][4] = 2;
    graph->matrix[4][2] = 2;
    PrintGraph(graph);
    Dijkstra(graph, 1);
    DisplayPath(graph, 1, 4);


//    PrintPQ(pq);
//    int x = ExtractMin(pq);
//    PrintPQ(pq);
}

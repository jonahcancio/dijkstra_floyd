#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define max_Vs 11

typedef struct Graph {
    int matrix[max_Vs][max_Vs];
    int n;
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
    int l;
    for (l = 1; l <= g->n; l++) {
        if(l == s){
            pq->heap[1] = s;
            pq->index[s] = 1;
            pq->key[s] = INT_MIN;
        }else{
            i++;
            pq->heap[i] = l;
            pq->index[l] = i;
            pq->key[l] = INT_MAX;
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

int main(){
    graph = malloc(sizeof(Graph));
    pq = malloc(sizeof(PriorityQueue));
    InitGraph(graph);
    graph->n = 3;
    graph->matrix[1][1] = 0;
    graph->matrix[2][2] = 0;
    graph->matrix[3][3] = 0;
    graph->matrix[1][3] = 6;
    graph->matrix[3][1] = 6;
    graph->matrix[1][2] = 4;
    graph->matrix[2][1] = 4;
    PrintGraph(graph);
    InitPQ(graph, pq, 3);
    PrintPQ(pq);
}

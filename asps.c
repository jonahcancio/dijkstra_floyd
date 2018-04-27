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
    int adjMatrix[max_Vs][max_Vs];//adjacency matrix
    int n;//graph size
    int dPred[max_Vs][max_Vs];//pred matrix for iterated Dijkstra
    int dist[max_Vs][max_Vs];//dist matrix for iteratedDijkstra
	int fPred[max_Vs][max_Vs];//pred matrix for Floyd
	int fMatrix[max_Vs][max_Vs];//Floyd D matrix
} Graph;

typedef struct PriorityQueue{
    int heap[max_Vs];
    int index[max_Vs];
    int key[max_Vs];
    int sizePQ;
} PriorityQueue;

Graph* graph;
PriorityQueue* pq;

//initialize graph with no vertices and edges
void InitGraph(Graph* g){
    int i, j;
    for (i = 0; i < max_Vs; i++) {
        for(j = 0; j < max_Vs; j++){
            g->adjMatrix[i][j] = INT_MAX;
        }
    }
    g->n = 0;
}

//parses the graph at fileName, and initializes the graph g's adjacency matrix accordingly
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
        exit(1);
    }
}

//prints adjacency list of graph; for debugging purposes
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

//prints fMatrix of Floyd algorithm; used for debugging
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

//initializes priority queue for Dijkstra's algorithm
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

//prints out the heap, index and key arrays of the priority queue; for debugging purposes
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

//returns 1 if priority queue is empty, else returns 0
int IsEmptyPQ(PriorityQueue* pq){
    return pq->sizePQ == 0;
}

//Converts the sub-heap at root r back into a min-heap
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

//returns the root(minimum) of the priority tree, and retains min heap property, used by Dijkstras algorithm
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

//decrease key[l] into newKey
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

//Dijkstra's algorithm from the book
void Dijkstra(Graph* g, PriorityQueue* pq, int s){
    InitPQ(g, pq, s);
    int i;
    //initialize pred array at row s to 0
    for (i = 1; i <= g->n; i++) {
        g->dPred[s][i] = 0;
    }
    int u;
    int v;
    int newVal;
    while(IsEmptyPQ(pq) == 0){
        u = ExtractMin(pq);
        if(pq->key[u] == INT_MAX){
            //printf("INFINITY: Vertex %d has no friends\n", s);
            break;
        }
        for(v = 1; v <= g->n; v++){
            if(g->adjMatrix[u][v] < INT_MAX){
                newVal = pq->key[u] + g->adjMatrix[u][v];
                if(pq->key[v] > newVal){
                    g->dPred[s][v] = u;
                    DecreaseKey(pq, v, newVal);
                }
            }
        }
    }
    //copy key array of pq into dist array at row s
    for (i = 1; i <= g->n; i++) {
        g->dist[s][i] = pq->key[i];
    }
}

//iterates dijkstras algorithm witha ll possible vertices as start nodes
void IteratedDijkstra(Graph* g, PriorityQueue* pq){
    int s;
    for(s = 1; s <= g->n; s++){
        Dijkstra(graph, pq, s);
    }
}

//Displays shortest path from vertex s to vertex v derived using Dijkstras, remember to run IteratedDijkstra() first
int DisplayDijkstraPath(Graph* g, int s, int v){
    printf("vertex %d to vertex %d: ", s, v);
    int path[(g->n)+1];
    int len = 1;
    path[len] = v;
    int i = v;
    while(i != s){
        if(g->dPred[s][i] == 0){
            printf("No dijkstra path found\n");
            return -1;
        }else{
            i = g->dPred[s][i];
            len += 1;
            path[len] = i;
        }
    }
//    printf("Shortest path found:\t");
    printf("%d ", path[len]);
    for(i = len-1; i >= 1; i--){
        printf("-> %d ", path[i]);
    }
    printf("(cost = %d)\n", g->dist[s][v]);
    return 0;
}

//displays Iterated Dijkstra derived paths on all possible start and end vertex pairs
void DisplayAllDijkstraPaths(Graph* g){
    int s, v;
    for(s = 1; s <= g->n; s++){
        for(v = 1; v <= g->n; v++){
            if(s==v){
                continue;
            }
            DisplayDijkstraPath(g, s, v);
        }
    }
}
//Floyd procedure from book calculates shortest paths into fMatrix
void Floyd(Graph* g){
    int i, j;
    //copy adjacency matrix to floyd D matrix
    for (i = 1; i <= g->n; i++) {
        for (j = 1; j <= g->n; j++) {
            g->fMatrix[i][j] = g->adjMatrix[i][j];
        }
    }

    //initialize Floyd pred matrix
    for (i = 1; i <= g->n; i++) {
        for (j = 1; j <= g->n; j++) {
            if(i == j || g->adjMatrix[i][j] == INT_MAX){
                g->fPred[i][j] = 0;
            }else{
                g->fPred[i][j] = i;
            }
        }
    }
    //Main Floyd procedure
    int k, dijk;
    for (k = 1; k <= g->n; k++) {
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

//Displays shortest path from vertex i to vertex j derived using Floyd, remember to run Floyd() first
 int DisplayFloydPath(Graph* g, int i, int j){
    printf("vertex %d to vertex %d: ", i, j);
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
//    printf("Shortest path found:\t");
    int x;
    printf("%d ", path[len]);
    for(x = len-1; x >= 1; x--){
        printf("-> %d ", path[x]);
    }
    printf("(cost = %d)\n", g->fMatrix[i][j]);
    return 0;
}

//displays Floyd derived paths on all possible start and end vertex pairs
void DisplayAllFloydPaths(Graph* g){
    int i, j;
    for(i = 1; i <= g->n; i++){
        for(j = 1; j <= g->n; j++){
            if(i==j){
                continue;
            }
            DisplayFloydPath(g, i, j);
        }
    }
}

int main(){
    //Initialization and input phase
    graph = malloc(sizeof(Graph));
    pq = malloc(sizeof(PriorityQueue));
    char fileName[30];
    printf("Program started\n\n");
    printf("Enter filename> ");
    scanf("%s", fileName);
    ReadGraph(fileName, graph);
//    printf("Graph Adjacency Matrix read:\n");
//    PrintGraph(graph);
    printf("\n");

    //Dijkstra phase
    printf("Iterated Dijkstra:\n");
    IteratedDijkstra(graph, pq);
    DisplayAllDijkstraPaths(graph);
    printf("\n");

    //Floyd phase
    printf("Floyd:\n");
    Floyd(graph);
//    PrintFloydGraph(graph);
    DisplayAllFloydPaths(graph);

    //Freedom phase
    free(graph);
    free(pq);
}

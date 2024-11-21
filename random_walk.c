#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_NODES 100  // Define max nodes for the graph
#define MAX_EDGES 1000 // Define max edges for the graph
#define WALK_LENGTH 10 // Length of each walk


typedef struct
{
	int start;
	int end;
} Edge;


typedef struct
{
	int node;
	int num_edges;
	int edges[MAX_NODES];
} GraphNode;


typedef struct {
    int num_nodes;
    GraphNode nodes[MAX_NODES];
} Graph;


// Initialize the graph with nodes and edges
void initialize_graph(Graph *graph, int num_nodes) 
{
	graph->num_nodes = num_nodes;
	for (int i = 0; i < num_nodes; i++) 
	{
		graph->nodes[i].node = i;
		graph->nodes[i].num_edges = 0;
	}
}
// Add an edge between two nodes in an undirected graph

void add_edge(Graph *graph, int node1, int node2)
{
	graph->nodes[node1].edges[graph->nodes[node1].num_edges++] = node2;
	graph->nodes[node2].edges[graph->nodes[node2].num_edges++] = node1;
}

// Choose a neighbor based on Node2Vec biases (simple approach)
int choose_next_node(Graph *graph, int curr_node, int prev_node, double p, double q)
{
	int next_node = -1;
	int num_neighbors = graph->nodes[curr_node].num_edges;
	int *neighbors = graph->nodes[curr_node].edges;
	// Choose the next node based on some random probability
	int rand_index = rand() % num_neighbors;
	next_node = neighbors[rand_index];

	return next_node;
}

// Generate a random walk
void generate_random_walk(Graph *graph, int start_node, int walk[], int walk_length, double p, double q) 
{
	walk[0] = start_node;
	int curr_node = start_node;
	int prev_node = -1;

	for (int i = 1; i < walk_length; i++) 
	{
		int next_node = choose_next_node(graph, curr_node, prev_node, p, q);
		walk[i] = next_node;
		prev_node = curr_node;
		curr_node = next_node;
	}
}

// Print the random walk
void print_walk(int walk[], int walk_length) 
{
	for (int i = 0; i < walk_length; i++) 
		printf("%d ", walk[i]);
	printf("\n");
}

// Main function
int main() 
{
	srand(time(NULL));  // Initialize random seed

	Graph graph;
	initialize_graph(&graph, 5);

	 // Adding edges to the graph
	add_edge(&graph, 0, 1);
	add_edge(&graph, 0, 2);
	add_edge(&graph, 1, 3);
	add_edge(&graph, 2, 3);
	add_edge(&graph, 3, 4);


	int walk[WALK_LENGTH];

	double p = 1.0, q = 1.0;


	// Generate multiple random walks
	for (int i = 0; i < graph.num_nodes; i++) 
	{
	     generate_random_walk(&graph, i, walk, WALK_LENGTH, p, q);
	     printf("Random Walk from Node %d: ", i);
	     print_walk(walk, WALK_LENGTH);
	}

	return 0;
}

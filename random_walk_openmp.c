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

int is_nb_of(Graph *graph, int a, int b)
{
	int a_num_neighbors = graph->nodes[a].num_edges;
	int *a_neighbors = graph->nodes[a].edges;
	for (int i = 0; i < a_num_neighbors; i++) 
	{
		if ( a_neighbors[i] == b ) 
		{
			return 1;
		}
	}
	return 0;
}

// Read the edges from a file and populate the graph
int read_edges_from_file(Graph *graph, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int node1, node2;
    int max_node_id = -1;
    while (fscanf(file, "%d %d", &node1, &node2) == 2) {
        add_edge(graph, node1, node2);
        if (node1 > max_node_id) max_node_id = node1;
        if (node2 > max_node_id) max_node_id = node2;
    }

    fclose(file);
    return max_node_id + 1; // Return the number of nodes (max ID + 1)
}

// Print the graph's adjacency list (for debugging)
void print_graph(Graph *graph) {
    for (int i = 0; i < graph->num_nodes; i++) {
        printf("Node %d: ", graph->nodes[i].node);
        for (int j = 0; j < graph->nodes[i].num_edges; j++) {
            printf("%d ", graph->nodes[i].edges[j]);
        }
        printf("\n");
    }
}

// Choose a neighbor based on Node2Vec biases (simple approach)
int choose_next_node(Graph *graph, int curr_node, int prev_node, double p, double q)
{
	printf("From %d ", curr_node);
	int next_node = -1;
	int num_neighbors = graph->nodes[curr_node].num_edges;
	int *neighbors = graph->nodes[curr_node].edges;
	// Normalizing constant
	double Z = 0.0;
	double weights[num_neighbors];
	// Weights
	double dtx0 = 1.0/p;
	double dtx1 = 1.0;
	double dtx2 = 1.0/q;
	
	// Weighted choice array
	for (int i = 0; i < num_neighbors; i++) 
	{
		if ( neighbors[i] == prev_node )
		{
			weights[i] = dtx0;
			Z += dtx0;
		} 
		else if ( is_nb_of( graph, prev_node, neighbors[i] ) )
		{
			weights[i] = dtx1;
			Z += dtx1;
		}
		else
		{
			weights[i] = dtx2;
			Z += dtx2;
		}
	}
	// TODO: Pre-compute weights as graph structure is constant
	// Select neighbor from weighted array
	weights[0]=weights[0]/(double)Z;
	// printf("%f, ", weights[0]);
	for (int i = 1; i < num_neighbors; i++)
	{
		weights[i]=(weights[i]/(double)Z)+weights[i-1];
		// printf("%f, ", weights[i]);
	}
	// printf("\n");
	double r = (double)rand()/(double)RAND_MAX;
	// printf("Random number chosen is %f\n", r);
	double low = 0.0;
	// Choose the next node based on some random probability
	for (int i = 0; i < num_neighbors; i++) 
	{
		if ( r >= low && r <= weights[i] )
		{
			printf("to %d\n", neighbors[i]);
			return neighbors[i];
		}
	}
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

	double p = 2.0, q = 1.0;
	printf("p:%f\nq:%f\n", p, q);

	// Generate multiple random walks
	#pragma omp parallel for
	for (int i = 0; i < graph.num_nodes; i++) 
	{
	     generate_random_walk(&graph, i, walk, WALK_LENGTH, p, q);
	     printf("Random Walk from Node %d: ", i);
	     print_walk(walk, WALK_LENGTH);
	}

	return 0;
}

#include <stdio.h>
#include <stdlib.h>

/* DFS vertex colors:*/
#define WHITE 0
#define GREY  1
#define BLACK 2

/* Edge types:*/
#define ROAD    1
#define AIRPORT 2

/* Graph types:*/
#define ROADS_ONLY        1
#define AIRPORTS_INCLUDED 2

typedef struct list_node {
	int vertex;
	int weight;
	struct list_node* next;
} list_node;

typedef struct set_node {
	int id; /* Unique for each node. Corresponds to the vertex it represents.*/
	int rank;
	struct set_node* parent;
} set_node;

typedef struct {
	int weight;
	int source;
	int destination;
	int type; /* Road or airport*/
} edge;


set_node* make_set() {
	set_node* x = (set_node*)malloc(sizeof(set_node));
	x->parent = x;
	x->rank = 0;
	return x;
}

void link(set_node* x, set_node* y) {
	if(x->rank > y->rank)
		y->parent = x;
	else {
		x->parent = y;
		if(x->rank == y->rank)
			++(y->rank);
	}
}

set_node* find_set(set_node* x) {
	if(x != x->parent) {
		x->parent = find_set(x->parent);
	}
	return x->parent;
}

void union_set(set_node* x, set_node* y) {
	link(find_set(x), find_set(y));
}

int compare_edges(const void* e1, const void* e2) {
	return ((edge*)e1)->weight - ((edge*)e2)->weight;
}

void DFS_visit(list_node** adj, int u, int* colour) {
	list_node* list_it;
	colour[u] = GREY;

	for(list_it = adj[u]; list_it != NULL; list_it = list_it->next) {
		if(colour[list_it->vertex - 1] == WHITE)
			DFS_visit(adj, list_it->vertex - 1 , colour);

	}
	colour[u] = BLACK;
}

int is_connected(list_node** adj, int size) {
	int i;
	int colour[size];
	for(i = 0; i < size; ++i)
		colour[i] = WHITE;

	/* Makes a single DFS visit from the a source. If, in the end,*/
	/*  one or more vertices weren't visited, then the graph isn't connected.*/
	DFS_visit(adj, 0, colour);

	for(i = 0; i < size; ++i) {
		if(colour[i] == WHITE)
			return 0;
	}

	return 1;
}

void MST_Kruskal(list_node** adj, int n_vertices, int n_edges, int graph_type,
	int* total_cost, int* n_roads_to_build, int* n_airports_to_build) {

	int i, j = 0;
	list_node* list_it;
	edge edges[n_edges];

	set_node** set_nodes = (set_node**)malloc(n_vertices*sizeof(set_node*));

	*total_cost = 0;
	*n_airports_to_build = 0;
	*n_roads_to_build = 0;

	for(i = 0; i < n_vertices; ++i) {
		set_nodes[i] = make_set();
	}

	for(i = 0; i < n_vertices; ++i) {
		for(list_it = adj[i]; list_it != NULL; list_it = list_it->next) {
			edges[j].weight = list_it->weight;
			edges[j].source = i;
			edges[j].destination = list_it->vertex - 1;

			if(graph_type == AIRPORTS_INCLUDED &&
				(list_it->vertex - 1 == n_vertices - 1 || i == n_vertices - 1)) {
				
			/*	printf("AIRPORT: %d - %d", i, list_it->vertex - 1);*/
				edges[j].type = AIRPORT;
			}
			else {
			/*	printf("ROAD: %d - %d", i, list_it->vertex - 1);*/

				edges[j].type = ROAD;
			}
			j++;
		}
	}	

	qsort((void*)edges, n_edges, sizeof(edge), compare_edges);

	for(i = 0; i < n_edges; ++i) {
		int u = edges[i].source;
		int v = edges[i].destination;

		if(find_set(set_nodes[u]) != find_set(set_nodes[v])) {
			/* We want to include this edge in our MST. In our case, that means:*/
			(*total_cost) += edges[i].weight;
			if(edges[i].type == ROAD)
				++*n_roads_to_build;
			
			else /* edges[i].type == AIRPORT */
				++*n_airports_to_build;
			union_set(set_nodes[u], set_nodes[v]);
		}
	}

	for(i = 0; i < n_vertices; ++i)
		free(set_nodes[i]);
	free(set_nodes);
}

void delete_list(list_node* list) {
/* Frees up the space taken by the nodes of the list passed as argument */
	list_node* list_iterator = list;
	list_node* next_node;
	while(list_iterator != NULL) {
		next_node = list_iterator->next;
		free(list_iterator);
		list_iterator = next_node;
	}
}

int main() {
	int n_cities;
	int n_airports;
	int n_roads;

	list_node** adj_a; /* adj_a: graph with airports and roads;*/
	list_node** adj_r; /* adj_r: graph with roads only;*/

	int i;
	int city, airport_cost;          /* Variables used in a cycle, later on;*/
	int origin, destination, road_cost; /* Variables used in a cycle, later on;*/

	int total_cost_a;
	int total_cost_r;

	int n_roads_to_build_a;
	int n_roads_to_build_r;
	int n_airports_to_build_a;
	int n_airports_to_build_r;

	scanf("%d", &n_cities);
	scanf("%d", &n_airports);

	adj_r = (list_node**)malloc(n_cities * sizeof(list_node*));
	adj_a = (list_node**)malloc((n_cities + 1) * sizeof(list_node*));
	/* The last list_node in adj_a is the airport list_node;*/

	for(i = 0; i < n_cities; ++i) {
		adj_r[i] = NULL;
		adj_a[i] = NULL;
	}

	adj_a[n_cities] = NULL;

	for(i = 0; i < n_airports; ++i) {
		list_node* a_node1 = (list_node*)malloc(sizeof(list_node));
		list_node* a_node2 = (list_node*)malloc(sizeof(list_node));

		scanf("%d %d", &city, &airport_cost);
		
		/* In the airport vertex, an edge is added to the city, with weight = airport_cost*/
		a_node1->vertex = city;
		a_node1->weight = airport_cost;
		a_node1->next   = adj_a[n_cities];
		adj_a[n_cities] = a_node1;

		/* In the city vertex, an edge is added to the airport, with weight = airport_cost*/
		a_node2->vertex = n_cities + 1;
		a_node2->weight = airport_cost;
		a_node2->next   = adj_a[city-1];
		adj_a[city-1]   = a_node2; 
	}

	scanf("%d", &n_roads);

	for(i = 0; i < n_roads; ++i) {

		list_node* r_node1 = (list_node*)malloc(sizeof(list_node));
		list_node* r_node2 = (list_node*)malloc(sizeof(list_node));
		list_node* r_node3 = (list_node*)malloc(sizeof(list_node));
		list_node* r_node4 = (list_node*)malloc(sizeof(list_node));

		scanf("%d %d %d", &origin, &destination, &road_cost);

		/* In the origin city vertex, an edge is added to the destination city,*/
		/*  with weight = road_cost, in both the adj_a and the adj_r graphs;*/
		r_node1->vertex = destination;
		r_node1->weight = road_cost;
		r_node1->next   = adj_a[origin-1];
		adj_a[origin-1] = r_node1;

		r_node2->vertex = destination;
		r_node2->weight = road_cost;
		r_node2->next   = adj_r[origin-1];
		adj_r[origin-1] = r_node2;
	

		/* In the destination city vertex, an edge is added to the origin city,*/
		/*  with weight = road_cost, in both the adj_a and the adj_r graphs;*/
		r_node3->vertex = origin;
		r_node3->weight = road_cost;
		r_node3->next   = adj_a[destination-1];
		adj_a[destination-1] = r_node3;

		r_node4->vertex = origin;
		r_node4->weight = road_cost;
		r_node4->next   = adj_r[destination-1];
		adj_r[destination-1] = r_node4;
	}

	if(!is_connected(adj_a, n_cities+1)) {
		puts("Insuficiente");
	}
	else if(!is_connected(adj_r, n_cities)) { /* if adj_r is not connected, we will definitely want the adj_a solution;*/
		MST_Kruskal(adj_a, n_cities + 1, 2 * (n_roads + n_airports), AIRPORTS_INCLUDED,
			&total_cost_a, &n_roads_to_build_a, &n_airports_to_build_a);
		
		printf("%d\n", total_cost_a);
		printf("%d %d\n", n_airports_to_build_a, n_roads_to_build_a);

	} else {
		MST_Kruskal(adj_a, n_cities + 1, 2 * (n_roads + n_airports), AIRPORTS_INCLUDED,
			&total_cost_a, &n_roads_to_build_a, &n_airports_to_build_a);

		MST_Kruskal(adj_r, n_cities, 2 * n_roads, ROADS_ONLY,
			&total_cost_r, &n_roads_to_build_r, &n_airports_to_build_r);

		if(total_cost_r <= total_cost_a) {
			/* Use adj_r solution;*/

			printf("%d\n", total_cost_r);
			printf("%d %d\n", n_airports_to_build_r, n_roads_to_build_r);
		}
		else {
			/* Use adj_a solution;*/

			printf("%d\n", total_cost_a);
			printf("%d %d\n", n_airports_to_build_a, n_roads_to_build_a);
		}
	}	

	for(i = 0; i < n_cities; ++i) {
		delete_list(adj_a[i]);
		delete_list(adj_r[i]);
	}
	delete_list(adj_a[n_cities]);

	free(adj_a);
	free(adj_r);

	return 0;
}

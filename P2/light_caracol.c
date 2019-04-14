#include <stdio.h>
#include <stdlib.h>

/* Edge types:*/
#define ROAD    1
#define AIRPORT 2

/* Graph types:*/
#define ROADS_ONLY        1
#define AIRPORTS_INCLUDED 2

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

void MST_Kruskal(edge* g, int n_vertices, int n_edges,
	int* total_cost, int* n_roads_to_build, int* n_airports_to_build) {

	int safe_edges = 0;
	int i = 0;

	set_node** set_nodes = (set_node**)malloc(n_vertices*sizeof(set_node*));

	*total_cost = 0;
	*n_airports_to_build = 0;
	*n_roads_to_build = 0;

	for(i = 0; i < n_vertices; ++i) {
		set_nodes[i] = make_set();
	}

	qsort((void*)g, n_edges, sizeof(edge), compare_edges);

	for(i = 0; i < n_edges; ++i) {
		int u = g[i].source-1;
		int v = g[i].destination-1;

		if(find_set(set_nodes[u]) != find_set(set_nodes[v])) {
			safe_edges++;
			/* We want to include this edge in our MST. In our case, that means:*/
			(*total_cost) += g[i].weight;
			if(g[i].type == ROAD)
				++*n_roads_to_build;
			
			else /* g[i].type == AIRPORT */
				++*n_airports_to_build;
			union_set(set_nodes[u], set_nodes[v]);
		}
	}

	if(safe_edges < n_vertices - 1) /* The graph isn't connected */
		*total_cost = -1;

	for(i = 0; i < n_vertices; ++i)
		free(set_nodes[i]);
	free(set_nodes);
}

int main() {
	int n_cities;
	int n_airports;
	int n_roads;

	edge* roads_g;
	edge* airports_g;

	int i;
	int city, airport_cost;          /* Variables used in a cycle, later on;*/
	int origin, destination, road_cost; /* Variables used in a cycle, later on;*/

	int total_cost_a = 0;
	int total_cost_r = 0;
	int n_roads_to_build_a = 0;
	int n_roads_to_build_r = 0;
	int n_airports_to_build_a = 0;
	int n_airports_to_build_r = 0;

	scanf("%d", &n_cities);
	scanf("%d", &n_airports);

	airports_g = (edge*)malloc(n_airports*sizeof(edge));

	for(i = 0; i < n_airports; ++i) {
		scanf("%d %d", &city, &airport_cost);
		
		airports_g[i].source      = city;
		airports_g[i].destination = n_cities + 1;
		airports_g[i].weight      = airport_cost;
		airports_g[i].type        = AIRPORT;
	}

	scanf("%d", &n_roads);

	roads_g = (edge*)malloc(n_roads*sizeof(edge));
	airports_g = (edge*)realloc(airports_g, (n_roads + n_airports)*sizeof(edge));

	for(i = 0; i < n_roads; ++i) {
		scanf("%d %d %d", &origin, &destination, &road_cost);

		airports_g[n_airports+i].source      = roads_g[i].source      = origin;
		airports_g[n_airports+i].destination = roads_g[i].destination = destination;
		airports_g[n_airports+i].weight      = roads_g[i].weight      = road_cost;
		airports_g[n_airports+i].type        = roads_g[i].type        = ROAD;
	}

	MST_Kruskal(airports_g, n_cities + 1, n_roads, &total_cost_a,
		&n_roads_to_build_a, &n_airports_to_build_a);

	if(total_cost_a < 0)
		puts("Insuficiente");

	else {
		MST_Kruskal(roads_g, n_cities, n_roads, &total_cost_r,
			&n_roads_to_build_r, &n_airports_to_build_r);

		if(total_cost_r > 0 && total_cost_r <= total_cost_a) {
			printf("%d\n", total_cost_r);
			printf("%d %d\n", n_airports_to_build_r, n_roads_to_build_r);
		}

		else {
			printf("%d\n", total_cost_a);
			printf("%d %d\n", n_airports_to_build_a, n_roads_to_build_a);
		}
	}

	free(airports_g);
	free(roads_g);

	return 0;
}

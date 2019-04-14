#include <stdio.h>
#include <stdlib.h>

/* Edge types:*/
#define ROAD    1
#define AIRPORT 2

typedef struct set_node {
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
	if(x != x->parent)
		x->parent = find_set(x->parent);
	return x->parent;
}

void union_set(set_node* x, set_node* y) {
	link(find_set(x), find_set(y));
}

int compare_edges(const void* e1, const void* e2) {
	return ((edge*)e1)->weight - ((edge*)e2)->weight;
}

edge* merge(edge* roads, int roads_size, edge* airports, int airports_size){
	int i = 0, j = 0;
	edge* merged = (edge*)malloc((roads_size+airports_size)*sizeof(edge));
	
	while(i < roads_size || j < airports_size){
		if((i < roads_size && compare_edges(&roads[i], &airports[j]) <= 0)  || (j >= airports_size)){
			merged[i+j] = roads[i];
			i++;
		}
		else /* compare_edges(&roads[i], &airports[j]) > 0 */{
			merged[i+j] = airports[j];
			j++;
		}
	}
	
	free(roads);
	free(airports);

	return merged;
}

void MST_Kruskal(edge* graph, int n_vertices, int n_edges,
	int* total_cost, int* n_roads_to_build, int* n_airports_to_build) {

	int safe_edges = 0; /* The number of safe_edges the algorithm encounters. */
	int i;

	set_node** set_nodes = (set_node**)malloc(n_vertices*sizeof(set_node*));

	*total_cost = 0;
	*n_airports_to_build = 0;
	*n_roads_to_build = 0;

	for(i = 0; i < n_vertices; ++i) {
		set_nodes[i] = make_set();
	}
	
	
	for(i = 0; i < n_edges; ++i) {
		int u = graph[i].source - 1;
		int v = graph[i].destination - 1;
		if(find_set(set_nodes[u]) != find_set(set_nodes[v])) /* (u, v) is a safe edge */ {
			safe_edges++;
			/* We want to include this edge in our MST. In our case, that means: */
			(*total_cost) += graph[i].weight;
			if(graph[i].type == ROAD)
				++*n_roads_to_build;
			
			else /* graph[i].type == AIRPORT */
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
	int begin = clock();
	int end;
	int n_cities;
	int n_airports;
	int n_roads;

	/* Two graphs, represented by sortable arrays of edges. */
	edge* roads_g;	  /* Represents the solution builiding only roads */
	edge* airports_g; /* Represents the airports */
	edge* merged;     /* Represents the solution builiding airports as well as roads */

	/* Variables used in cycles, later on. */
	int i;
	int city, airport_cost;
	int origin, destination, road_cost;

	/* Variables passed to the MST_Kruskal function, *
	 *  where the results will be stored.            */
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

	for(i = 0; i < n_roads; ++i) {
		scanf("%d %d %d", &origin, &destination, &road_cost);
		
		roads_g[i].source      = origin;
		roads_g[i].destination = destination;
		roads_g[i].weight      = road_cost;
		roads_g[i].type        = ROAD;
	}
		
	
	qsort((void*)roads_g, n_roads, sizeof(edge), compare_edges);
	
	MST_Kruskal(roads_g, n_cities, n_roads, &total_cost_r,
		&n_roads_to_build_r, &n_airports_to_build_r);
	
	
	qsort((void*)airports_g, n_airports, sizeof(edge), compare_edges);
	merged = merge(roads_g, n_roads, airports_g, n_airports);
	
	MST_Kruskal(merged, n_cities + 1, n_airports + n_roads,
		&total_cost_a, &n_roads_to_build_a, &n_airports_to_build_a);
	
	
			
	/* If none of the graphs is connected, the input is deemed insuficient. */
	if(total_cost_a < 0 && total_cost_r < 0){}
	//	printf("Insuficiente\n");

	/* If only one of them is connected, we use that one's solution. */
	else if(total_cost_r < 0) {
	//	printf("%d\n", total_cost_a);
	//	printf("%d %d\n", n_airports_to_build_a, n_roads_to_build_a);
	}

	else if(total_cost_a < 0) {
	//	printf("%d\n", total_cost_r);
	//	printf("%d %d\n", n_airports_to_build_r, n_roads_to_build_r);
	}
	
	/* In case they're both connected, we use the one with the lowest total cost. *
	 *  If they're the same, we use the solution without airports.                */
	else if(total_cost_r <= total_cost_a) {
	//	printf("%d\n", total_cost_r);
	//	printf("%d %d\n", n_airports_to_build_r, n_roads_to_build_r);
	}

	else /* total_cost_a < total_cost_r */ {
	//	printf("%d\n", total_cost_a);
	//	printf("%d %d\n", n_airports_to_build_a, n_roads_to_build_a);
	}

	free(merged);
	end = clock();
	printf("%d %d %d %d\n", end-begin, n_cities, n_airports, n_roads);
	return 0;
}

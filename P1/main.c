#include <stdio.h>
#include <stdlib.h>

#define INSUFICIENTE 	-2
#define INCOERENTE 		-1
#define VALIDO 			 0

#define	WHITE	0
#define GREY 	1
#define BLACK	2

typedef struct node {
	int value;
	struct node* next;
} node;

void delete_list(node* list) {
	node* list_iterator = list;
	node* next_node;
	while(list_iterator != NULL) {
		next_node = list_iterator->next;
		free(list_iterator);
		list_iterator = next_node;
	}
}

int DFS_visit(node** adj, node** t_ordered, int u, int* colour) {
	int ret_val;
	node* list_iterator;
	node* closed_node;

	colour[u] = GREY;

	for(list_iterator = adj[u]; list_iterator != NULL; list_iterator = list_iterator->next) {
		if(colour[list_iterator->value] == WHITE) {
			ret_val = DFS_visit(adj, t_ordered, list_iterator->value, colour);
			if(ret_val == INCOERENTE) {
				return INCOERENTE;
			}
		}
		else if(colour[list_iterator->value] == GREY) {
			return INCOERENTE;
		}
	}
	colour[u] = BLACK;

	closed_node = (node*)malloc(sizeof(node));
	closed_node->value = u; 
	closed_node->next = *t_ordered;
	*t_ordered = closed_node;
	
	return VALIDO;
}

int DFS(node** adj, node** t_ordered, int n_photos) {
	int i, ret_val;
	int* colour;
	node* t_ordered_iterator;
	node* adj_iterator;

	colour = (int*)malloc(n_photos*sizeof(int));
	
	for(i = 0; i < n_photos; ++i) {
		colour[i] = WHITE;
	}

	for(i = 0; i < n_photos; ++i) {
		if (colour[i] == WHITE) {
			ret_val = DFS_visit(adj, t_ordered, i, colour);
			if(ret_val == INCOERENTE){
				free(colour);
				return INCOERENTE;
			}
		}
	}
	
	free(colour);
	
	t_ordered_iterator = (*t_ordered);

	while(t_ordered_iterator->next != NULL) {
		
		adj_iterator = adj[t_ordered_iterator->value];

		while(adj_iterator != NULL) {
			if(adj_iterator->value == t_ordered_iterator->next->value) break;
			adj_iterator = adj_iterator->next;
		}

		if(adj_iterator == NULL) {
			return INSUFICIENTE;
		}

		t_ordered_iterator = t_ordered_iterator->next;
	}
	
	return VALIDO;
}

int main() {
	int n_photos, n_edges;
	int u, v;
	int ret_val;
	int i;

	node** adj;
	node*  t_ordered;

	scanf("%d %d", &n_photos, &n_edges);
	
	adj = (node**)malloc(n_photos*sizeof(node*));

	t_ordered = NULL;

	for(i = 0; i < n_photos; ++i)
		adj[i] = NULL;

	for(i = 0; i < n_edges; i++) {
		node* v_node = (node*)malloc(sizeof(node));
		
		scanf("%d %d", &u, &v);
		
		v_node->value = v-1;
		v_node->next = adj[u-1];
		adj[u-1] = v_node;
	}

	ret_val = DFS(adj, &t_ordered, n_photos);

	if(ret_val == INCOERENTE){
		puts("Incoerente");
	}
	else if(ret_val == INSUFICIENTE){
		puts("Insuficiente");
	}
	else {
		node* list_iterator = t_ordered;
		while(list_iterator->next != NULL) {
			printf("%d ", (list_iterator->value) + 1);
			list_iterator = list_iterator->next;
		}

		printf("%d\n", (list_iterator->value) + 1);	
	}

	for(i = 0; i < n_photos; i++) {
		if(adj[i] != NULL) {
			delete_list(adj[i]);
		}
	}
	delete_list(t_ordered);
	free(adj);
	
	return 0;
}

#include <stdio.h>
#include <time.h>

int main(){
	char generateCommand[100];
	int i;
	int num_cidades,num_arestas, num_aero, num_estradas, custo_max, begin, end;
	system("rm tests.txt");
	for(i = 1; i<101; i++){
		printf("%d\n", i);
		num_cidades =(int) i*40;
		num_arestas = num_cidades*num_cidades/2;
		num_aero = rand()%(num_cidades);
		num_estradas = num_arestas-num_aero;
		
		custo_max = num_estradas;
		sprintf(generateCommand, "./gen_p2 %d %d %d %d 0 > inputNovo.txt",
			num_cidades, num_aero, num_estradas, custo_max);
		system(generateCommand);
		printf("n_cities: %d, n_est: %d, n_aero: %d, custo_max: %d\n",
			num_cidades, num_estradas, num_aero, custo_max);
		system("./a.out<inputNovo.txt>>tests.txt");



	}
}

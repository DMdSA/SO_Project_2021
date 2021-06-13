#include <stdio.h>
#include <string.h>
#include <stdlib.h>



void swap(int* x, int* y){

	int aux = *x;
	*x = *y;
	*y = aux;
}




void selectionSort(int* array, int size){


	for(int i = 0; i < size-1; i++){

		int min_index = i;

		for(int a = i+1; a < size; a++){

			min_index = array[min_index] < array[a] ? min_index : a;
		}

		swap(&(array[min_index]), &(array[i]));

	}


}



int sumhtpo (int n) {

	int r = 0; int contador = 0;
	int size = 150;
	int* array = malloc(sizeof(int) * size);


	while (n != 1) {

		if(contador >= 149){
			size *= 2;
			array = realloc(array, sizeof(int) * size);
		}



		array[contador] = n;
		contador++;
		r += n;
		if (n%2 == 0) n = n/2; else n = 1+(3*n);
	}

	if(contador < 100) return -1;

	selectionSort(array, contador);
	int answer = array[99];
	return answer;
}
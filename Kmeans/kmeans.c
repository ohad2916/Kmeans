#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DEFAULT_ITER 200
#define Convergence_VALUE 0.001
#define INT_MAX 2147483647

double euc_d(double* p, double* q, size_t dim) {
	double d_sqrd_sum = 0;
	size_t i = 0;
	while (i < dim) {
		d_sqrd_sum += pow(p[i] - q[i], 2);
		i++;
	}
	return sqrt(d_sqrd_sum);
}

int free_memory(double* a, double* b, double* c, double** data, double** mean, double** curr) {
	free(a);
	free(b);
	free(c);
	free(data);
	free(mean);
	free(curr);
	return 0;
}


int main(int argc, char** argv) {
	size_t j, m, i, N, K, iter, d, size,capacity;
	char ch;
	char* input_data,*temp_input_data;
	double** data;
	double* p;
	char* end_ptr;
	char* begin_ptr;
	double* b,*c;
	double** cluster_mean;
	double** curr_X;
	size_t min_cluster_index;
	double min_value, curr_euc_d;
	double** new_cluster;
	double* min_cluster;
	double curr_Muk;
	double max_Duk;
	int converged;
	N = 1;
	d = 1;
	data = NULL; p = NULL; b = NULL; c = NULL; cluster_mean = NULL; new_cluster = NULL;

	/*input validation*/
	if (argc < 2 || argc > 3) {
		/*printf("Invalid number of cmd_line_Arguments: %d\nPlease run the program in the following format:\n$./kmeans {N} {ITER} <{input_data.txt}, ITER is optional!,Default value set to 200.\n", argc);*/
		printf("An Error Has Occurred\n");
		return 1;
	}
	K = atoi(*(argv + 1)); /*validate later*/
	if (argc == 3)
		iter = atoi(*(argv + 2));
	else
		iter = DEFAULT_ITER;
	if (iter >= 1000) {
		printf("Invalid maximum iteration!\n");
		return 1;
	}
	/*load file, calculate N, d. can be Optimized.*/
	input_data = NULL;
	size = 0;
	capacity = 100;
	input_data = calloc(capacity + 1,sizeof(char));
	if (!input_data){
		printf("An Error Has Occurred\n");
		free_memory(b, c, p, new_cluster, cluster_mean, data);
		return 1;
	}
	while ((ch = getchar()) != EOF) {
		input_data[size] = (char)ch;
		size++;
		if(size == capacity){
			capacity += 100;
			temp_input_data = realloc(input_data, sizeof(char) * capacity + 1);
			if (!temp_input_data) {
				printf("An Error Has Occurred\n");
				free_memory(b, c, p, new_cluster, cluster_mean, data);
				return 1;
			}
			else
				input_data = temp_input_data;
		}
		N += (ch == '\n');
	}
	if (input_data[size-1] == '\n') {/*size is actually size()(*/
		N--;
	}
	/*inputted K validation*/
	if (K >= N) {
		printf("Invalid number of clusters!\n");
		return 1;
	}
	input_data[size] = '\0';
	i = 0;
	while (i < size && input_data[i] != '\n') {
		d += input_data[i] == ',';
		i++;
	}
	/*convert loaded string to a double array*/
	data = NULL;
	data = calloc( N,sizeof(double*));
	if (!data) {
		printf("An Error Has Occurred\n");
		free_memory(b, c, p, new_cluster, cluster_mean, data);
		return 1;
	}
	p = NULL;
	p = (double*)calloc(N * d, sizeof(double));
	if (!p){
		printf("An Error Has Occurred\n");
		free_memory(b, c, p, new_cluster, cluster_mean, data);
		return 1;
	}
	for (i = 0; i < N; i++) {
		data[i] = p + i * d;
	}
	begin_ptr = input_data;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < d; j++)
		{
			data[i][j] = strtod(begin_ptr, &end_ptr);
			begin_ptr = end_ptr + 1;

		}
	}
	free(input_data);
	/*printing for debugging*/
	/*for (i = 0; i < N; i++) {
		for (j = 0; j < d; j++) {
			printf("%f,", data[i][j]);
		}
		printf("\n");
	}*/
	/*end convert
	initialize clusters*/
	b = calloc(K * d, sizeof(double));
	cluster_mean = calloc(K , sizeof(double*));
	if (!b || !cluster_mean){
		printf("An Error Has Occurred\n");
		free_memory(b, c, p, new_cluster, cluster_mean, data);
		return 1;
	}
	for (i = 0; i < K; i++) {
		cluster_mean[i] = b + i * (d);
	}

	for (i = 0; i < K; i++)
	{
		for (j = 0; j < d; j++)
		{
			cluster_mean[i][j] = data[i][j];
		}
	}
	/*main algorithem*/
	curr_X = data;
	i = 0;
	/*allocate temporary clusters to decide convergence*/
	c = calloc(K * (d + 1), sizeof(double));
	new_cluster = calloc(K , sizeof(double*));
	if (!c || !new_cluster) {
		printf("An Error Has Occurred\n");
		free_memory(b, c, p, new_cluster, cluster_mean, data);
		return 1;
	}
	for (i = 0; i < K; i++) {
		new_cluster[i] = c + i * (d + 1);
	}

	/*printf("#datapoints recieved: %lu of dimension:%lu\n", (unsigned long)N, (unsigned long)d);
	printf("Iterating %lu times over %lu clusters\n", (unsigned long)iter, (unsigned long)K);*/
	
	i = 0;
	converged = 0;
	while (i < iter && !converged)
	{	
		/*zero out new cluster array*/
		memset(c, 0.0, K* (d + 1) * sizeof(double));
		/*decide closest cluster against the original and update it with new Xi*/
		for (m = 0; m < N ; m++) {
			min_cluster_index = 0;
			min_value = INT_MAX;
			for (j = 0; j < K; j++)
			{
				curr_euc_d = euc_d(cluster_mean[j], *(curr_X + m), d);
				if (curr_euc_d < min_value)
				{
					min_value = curr_euc_d;
					min_cluster_index = j;
				}
			}
			/*updating new cluster, just adding for now. divide later.*/
			min_cluster = new_cluster[min_cluster_index];
			for (j = 0; j < d; j++)
			{
				min_cluster[j] += curr_X[m][j];
			}
			min_cluster[d]++;
		}
		/*calculate the actual means*/
		for (j = 0; j < K; j++){
			for (m = 0; m < d; m++){
				new_cluster[j][m] /= new_cluster[j][d];
			}
		}

		/*decide convegerence*/
		max_Duk = 0;
		for (j = 0; j < K; j++) {
			curr_Muk = euc_d(cluster_mean[j], new_cluster[j], d);
			if (curr_Muk > max_Duk)
				max_Duk = curr_Muk;
		}
		if (max_Duk <= Convergence_VALUE) {
			/*print statement for debugging*/
			/*printf("Converged after: %d iterations\n", (int)i + 1);*/
			converged = 1;
		}
		i++;
		/*copy new cluster to old ones*/
		for (j = 0; j < K; j++)
		{
			for (m = 0; m < d; m++)
			{
				cluster_mean[j][m] = new_cluster[j][m];
			}
		}
	}
	for (m = 0; m < K; m++)
	{
		for (j = 0; j < d; j++)
		{
			printf("%.4f", cluster_mean[m][j]);
			if (j < d - 1)
				printf(",");
		}
		printf("\n");
	}
	free_memory(b, c, p, new_cluster, cluster_mean, data);
	/*free(b);
	free(c);
	free(p);
	free(new_cluster);
	free(cluster_mean);
	free(data);*/

	return 0;
}

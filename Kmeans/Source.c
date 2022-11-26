#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define ITER 600
#define Convergence_VALUE 0.001

double euc_d(double* p, double* q,size_t dim) { 
	double d_sqrd_sum = 0;
	int i = 0;
	while (i < dim) {
		d_sqrd_sum += pow(p[i] - q[i], 2);
		i++;
	}
	return sqrt(d_sqrd_sum);
}

int main(int argc, char **argv) {
	//input validation
	if (argc != 3){
		printf("Invalid number of CL-Arguments: %d\nPlease run the program in the following format:\n$./kmeans {N} {ITER} <{input_data.txt}", argc);
		return 1;
	}
	int	K = atoi(*(argv+1)); // validate later
	int iter = atoi(*(argv + 2)); //validate later
	int N = 1;
	int d = 1;
	
	//load file, calculate N, d
	char ch;
	int size = 0;
	char* input_data = malloc(sizeof(char)+1);
	while ((ch = getchar()) != EOF) {
		input_data[size] = (char)ch;
		size++;
		if (!(input_data = realloc(input_data, sizeof(char) * size + 1))) return 1;
		N += (ch == '\n');
	}
	input_data[size] = '\0';
	int i = 0;
	while (input_data[i] != '\n') {
		d += input_data[i] == ',';
		i++;
	}
	// convert loaded string to a double array
	double** data = malloc(sizeof(double*)*N);
	double* p;
	p = (double*)calloc(N * d, sizeof(double));
	for (size_t i = 0; i < N; i++){
		data[i] = p + i * d;
	}
	char* end_ptr = input_data;
	char* begin_ptr = input_data;
	for (size_t i = 0; i < N; i++) 
	{
		for (size_t j = 0; j < d; j++)
		{
			while (*end_ptr != ',' && *end_ptr != '\n') {
				end_ptr++;
			}

			data[i][j] = strtod(begin_ptr,&end_ptr);
			begin_ptr = ++end_ptr ;
		}
	}
	free(input_data);
	//end convert
	//initialize clusters
	double* b = calloc(K*(d + 2), sizeof(double));
	double** cluster_mean = malloc(K*sizeof(double*));
	for (size_t i = 0; i < K; i++){
		cluster_mean[i] = b + i * (d+2);
	}

	for (size_t i = 0; i < K; i++)
	{
		for (size_t j = 0; j < d; j++)
		{
			cluster_mean[i][j] = data[i][j];
		}
	} 
	for (size_t i = 0; i < K; i++)
	{
		cluster_mean[i][d] = 1;
		cluster_mean[i][d+1] = 0;
	}
	//main algorithem
	double** curr_X = data;
	curr_X += K;
	i = 0;
	while (i < ITER && i < N)
	{	

		int min_cluster_index = 0;
		//decide closest cluster
		double min_value = INT_MAX;
		for (size_t j = 0; j < K; j++)
		{
			double curr_euc_d = euc_d(cluster_mean[j], *(curr_X+i), d);
			if (curr_euc_d < min_value)
			{
				min_value = curr_euc_d;
				min_cluster_index = j;
			}
		}
		//update centeroid
		double* curr_cluster = malloc(sizeof(double) * d);
		double* min_cluster = cluster_mean[min_cluster_index];
		memcpy(curr_cluster, min_cluster, sizeof(double) * d);
		
		for (int j = 0; j < d; j++)
		{		
			double cluster_size = min_cluster[d];
			min_cluster[j] *= (cluster_size);
			min_cluster[j] += (curr_X[i][j]);
			min_cluster[j] /= (cluster_size + 1);
		}
		
		double curr_Muk = euc_d(curr_cluster, min_cluster, d);
		min_cluster[d + 1] = curr_Muk;

		double max_Duk = cluster_mean[1][d+1];
		for (size_t m = 0; m < K; m++)
		{
			max_Duk = max(max_Duk, cluster_mean[m][d + 1]);
		}
		cluster_mean[min_cluster_index][d]++;
		
		if (max_Duk < Convergence_VALUE)
			printf("Converged!\n");

		/*
		if (i % 100 == 0)
		{
			printf("min ");
			for (int s = 0; s < d + 2; s++)
			{
				printf("%f ", min_cluster[s]);
			}
			printf("\nmaxduk, %f\n", max_Duk);
		} */
	
		i++;
	}

	printf("#datapoints recieved: %d\nof dimension:%d\n", N,d);

	for (size_t m = 0; m < K; m++)
	{
		for (size_t j = 0; j < d+1; j++)
		{
			printf("%f, ", cluster_mean[m][j]);
		}
		printf("\n");
	}
	
	




	return 1;
}
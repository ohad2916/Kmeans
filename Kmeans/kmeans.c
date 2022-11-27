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

int main(int argc, char** argv) {
	size_t j, m, i, N, K, iter, d, size;
	char ch;
	char* input_data;
	double** data;
	double* p;
	char* end_ptr;
	char* begin_ptr;
	double* b;
	double** cluster_mean;
	double** curr_X;
	size_t min_cluster_index;
	double min_value, curr_euc_d;
	double* curr_cluster;
	double* min_cluster;
	double cluster_size;
	double curr_Muk;
	double max_Duk;
	N = 1;
	d = 1;
	size = 0;

	/*input validation*/
	if (argc < 2) {
		printf("Invalid number of CL-Arguments: %d\nPlease run the program in the following format:\n$./kmeans {N} {ITER} <{input_data.txt}, ITER is optional!,Default value set to 200.\n", argc);
		return 1;
	}
	K = atoi(*(argv + 1)); /*validate later*/
	if (argc == 3)
		iter = atoi(*(argv + 2));
	else
		iter = DEFAULT_ITER;
	if (iter >= 1000) {
		printf("Invalid maximum iteration!");
		return 1;
	}

	/*load file, calculate N, d. can be Optimized.*/
	input_data = malloc(sizeof(char) + 1);
	while ((ch = getchar()) != EOF) {
		input_data[size] = (char)ch;
		size++;
		if (!(input_data = realloc(input_data, sizeof(char) * size + 1))) return 1;
		N += (ch == '\n');
	}
	/*inputted K validation*/
	if (K >= N) {
		printf("Invalid number of clusters!");
		return 1;
	}
	input_data[size] = '\0';
	i = 0;
	while (input_data[i] != '\n') {
		d += input_data[i] == ',';
		i++;
	}
	/*convert loaded string to a double array*/
	data = malloc(sizeof(double*) * N);
	p = (double*)calloc(N * d, sizeof(double));
	for (i = 0; i < N; i++) {
		data[i] = p + i * d;
	}
	end_ptr = input_data;
	begin_ptr = input_data;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < d; j++)
		{
			while (*end_ptr != ',' && *end_ptr != '\n') {
				end_ptr++;
			}

			data[i][j] = strtod(begin_ptr, &end_ptr);
			begin_ptr = ++end_ptr;
		}
	}
	/*free(input_data);*/
	/*end convert
	initialize clusters*/
	b = calloc(K * (d + 2), sizeof(double));
	cluster_mean = malloc(K * sizeof(double*));
	for (i = 0; i < K; i++) {
		cluster_mean[i] = b + i * (d + 2);
	}

	for (i = 0; i < K; i++)
	{
		for (j = 0; j < d; j++)
		{
			cluster_mean[i][j] = data[i][j];
		}
	}
	for (i = 0; i < K; i++)
	{
		cluster_mean[i][d] = 1;
		cluster_mean[i][d + 1] = 0;
	}
	/*main algorithem*/
	curr_X = data;
	curr_X += K;
	i = 0;
	while (i < iter)
	{

		min_cluster_index = 0;
		/*decide closest cluster*/
		min_value = INT_MAX;
		for (j = 0; j < K; j++)
		{
			curr_euc_d = euc_d(cluster_mean[j], *(curr_X + i % (N-K)), d);
			if (curr_euc_d < min_value)
			{
				min_value = curr_euc_d;
				min_cluster_index = j;
			}
		}
		/*update centeroid*/
		curr_cluster = malloc(sizeof(double) * d);
		min_cluster = cluster_mean[min_cluster_index];
		memcpy(curr_cluster, min_cluster, sizeof(double) * d);

		for (j = 0; j < d; j++)
		{
			cluster_size = min_cluster[d];
			/*min_cluster[js] *= (cluster_size);
			min_cluster[j] += (curr_X[i][j]);
			min_cluster[j] /= (cluster_size + 1);*/
			min_cluster[j] = min_cluster[j] / (cluster_size + 1) * cluster_size + curr_X[i%(N-K)][j] / (cluster_size + 1);
		}
		min_cluster[d]++;

		curr_Muk = euc_d(curr_cluster, min_cluster, d);
		min_cluster[d + 1] = curr_Muk;

		max_Duk = cluster_mean[1][d + 1];
		for (m = 0; m < K; m++)
		{
			if (cluster_mean[m][d + 1] > max_Duk)
				max_Duk = cluster_mean[m][d + 1];
		}

		if (max_Duk < Convergence_VALUE) {
			printf("Converged!\n");
			break;
		}

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

	printf("#datapoints recieved: %lu of dimension:%lu\n", (unsigned long)N, (unsigned long)d);
	printf("Iterating %lu times over %lu clusters\n", (unsigned long)iter, (unsigned long)K);

	for (m = 0; m < K; m++)
	{
		for (j = 0; j < d + 1; j++)
		{
			printf("%f, ", cluster_mean[m][j]);
		}
		printf("\n");
	}

	return 1;
}

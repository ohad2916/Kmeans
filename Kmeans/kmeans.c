#include <stdio.h>
#include <stdlib.h>
#include <math.h>


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
	double** curr_clusters;
	double* min_cluster;
	double cluster_size;
	double curr_Muk;
	double max_Duk;
	N = 1;
	d = 1;

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
	input_data = NULL;
	size = 0;
	capacity = 100;
	while(!input_data)
		input_data = calloc(capacity + 1,sizeof(char));
	while ((ch = getchar()) != EOF) {
		input_data[size] = (char)ch;
		size++;
		if(size == capacity){
			capacity += 100;
			temp_input_data = realloc(input_data, sizeof(char) * capacity + 1);
			if (!temp_input_data) {
				printf("Error allocating memory while loading file!");
				return 1;
			}
			else
				input_data = temp_input_data;
		}
		N += (ch == '\n');
	}
	/*inputted K validation*/
	if (K >= N) {
		printf("Invalid number of clusters!");
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
	while(!data)
		data = calloc( N,sizeof(double*));
	p = NULL;
	while(!p)
		p = (double*)calloc(N * d, sizeof(double));
	for (i = 0; i < N; i++) {
		data[i] = p + i * d;
	}
	begin_ptr = &input_data[0];
	end_ptr = &begin_ptr[0];
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < d; j++)
		{
			/*while (*end_ptr != ',' && *end_ptr != '\n') {
				end_ptr++;
			}

			data[i][j] = strtod(begin_ptr, &end_ptr);
			begin_ptr = ++end_ptr;*/
			if (*end_ptr == '\0')
				break;
			else
				begin_ptr = end_ptr + 1;
			data[i][j] = strtod(begin_ptr, &end_ptr);
		}
	}
	free(input_data);
	/*end convert
	initialize clusters*/
	b = calloc(K * (d + 1), sizeof(double));
	cluster_mean = calloc(K , sizeof(double*));
	for (i = 0; i < K; i++) {
		cluster_mean[i] = b + i * (d + 1);
	}

	for (i = 0; i < K; i++)
	{
		for (j = 0; j < d; j++)
		{
			cluster_mean[i][j] = data[i][j];
		}
	}
	/*for (i = 0; i < K; i++)
	{
		cluster_mean[i][d] = 1;
	}*/
	/*main algorithem*/
	curr_X = data;
	i = 0;
	/*allocate temporary clusters to decide convergence*/
	c = calloc(K * (d + 1), sizeof(double));
	curr_clusters = calloc(K , sizeof(double*));
	for (i = 0; i < K; i++) {
		curr_clusters[i] = c + i * (d + 1);
	}

	printf("#datapoints recieved: %lu of dimension:%lu\n", (unsigned long)N, (unsigned long)d);
	printf("Iterating %lu times over %lu clusters\n", (unsigned long)iter, (unsigned long)K);
	
	i = 0;
	while (i < iter)
	{	
		/*copy current clusters to decide against.*/
		for (j = 0; j < K; j++)
		{
			for (m = 0; m < d + 1; m++)
			{
				curr_clusters[j][m] = cluster_mean[j][m];
			}
		}
		/*decide closest cluster against the copy and update it with new Xi*/
		for (m = 0; m < N ; m++) {
			min_cluster_index = 0;
			min_value = INT_MAX;
			for (j = 0; j < K; j++)
			{
				curr_euc_d = euc_d(curr_clusters[j], *(curr_X + m), d);
				if (curr_euc_d < min_value)
				{
					min_value = curr_euc_d;
					min_cluster_index = j;
				}
			}
			/*updating real one*/
			min_cluster = cluster_mean[min_cluster_index];
			cluster_size = min_cluster[d];
			for (j = 0; j < d; j++)
			{
				/*min_cluster[js] *= (cluster_size);
				min_cluster[j] += (curr_X[i][j]);
				min_cluster[j] /= (cluster_size + 1);*/
				min_cluster[j] = ((min_cluster[j] / (cluster_size + 1)) * cluster_size) + (curr_X[m][j] / (cluster_size + 1));
			}
			min_cluster[d]++;
		}

		/*decide convegerence*/
		max_Duk = 0;
		for (j = 0; j < K; j++) {
			curr_Muk = euc_d(cluster_mean[j], curr_clusters[j], d);
			if (curr_Muk > max_Duk)
				max_Duk = curr_Muk;
		}
		if (max_Duk <= Convergence_VALUE) {
			/*print statement for debugging*/
			/*printf("Converged after: %d iterations\n", i +1);*/
			break;
		}

		i++;
	}


	for (m = 0; m < K; m++)
	{
		for (j = 0; j < d + 1; j++)
		{
			printf("%f, ", cluster_mean[m][j]);
		}
		printf("\n");
	}
	free(b);
	free(c);

	return 1;
}

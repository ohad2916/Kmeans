import sys
import math

try:
    # total arguments
    arg_c = len(sys.argv)

    k = int(sys.argv[1])
    iter_ = 200
    epsilon = 0.001
    input_data_name = sys.argv[-1]

    if arg_c == 4:
        iter_ = int(sys.argv[2])
except:
    print("An error has Occurred")
    sys.exit()


if arg_c > 4 or arg_c < 2:
    print("An Error has Occurred")
    sys.exit()

if iter_ > 999:
    print("Invalid maximum iteration!")
    sys.exit()

points = []
centroids = []


def closest_centroid(centroids_, xi_, dim):
    index_closest = 0
    min_dist = sys.float_info.max
    for i in range(len(centroids_ )):
        dist = euc_d(xi_ , centroids_[i], dim)
        if dist < min_dist:
            index_closest = i
            min_dist = dist

    return index_closest


def euc_d(p1, p2, dim):
    sum_ = 0.0
    for i in range(dim):
        sum_ += (p1[i]-p2[i])**2

    return math.sqrt(sum_)


with open(input_data_name) as file:
    for line in file:
        points_in_line = [float(x) for x in line.split(",")]
        points.append(points_in_line)
#debugging
    # for point in points:
    #     print(*point)
if k <= 0 or k >= len(points):
    print("Invalid number of clusters!")
    sys.exit()

dimension = len(points[0])
centroids = []
for i in range(k):
    centroids.append(points[i].copy())
old_centroids = []

curr_iter = 0
convergence = sys.float_info.max

while curr_iter < iter_ and convergence > epsilon:
    addition_centroids = [[0 for i in range(dimension)] for j in range(k)]
    addition_centroids_sizes = [0 for i in range(k)]

    for i in range(len(points)):
        xi = points[i]
        closest_c_index = closest_centroid(centroids, xi, dimension)

        for j in range(dimension):
            addition_centroids[closest_c_index][j] += xi[j]

        addition_centroids_sizes[closest_c_index] += 1

    # updating the copy centroids
    for n in range(k):
        for m in range(dimension):
            if addition_centroids_sizes[n] > 0:
                addition_centroids[n][m] = addition_centroids[n][m] / addition_centroids_sizes[n]
            else:
                addition_centroids[n][m] = 0.0

    # updating convergence val
    convergence = 0
    curr_Muk = 0
    for i in range(k):
        curr_Muk = euc_d(addition_centroids[i], centroids[i], dimension)
        if curr_Muk > convergence:
            convergence = curr_Muk
    # updating real centroids
    centroids = [p.copy() for p in addition_centroids]
    curr_iter += 1


# print(f"final iter: {curr_iter}")
for cent in centroids:
    print(','.join(f'{c:.4f}' for c in cent))










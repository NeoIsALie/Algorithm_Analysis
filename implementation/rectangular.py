from random import randint
import numpy as np
import networkx as nx
from timeit import timeit
import matplotlib.pyplot as plt

def generate_matrix(vertices):
    matrix = np.full((vertices, vertices), np.inf, dtype=int)
    for i in range(vertices):
        for k in range(vertices):
            matrix[i][k] = randint(1, vertices) if i != k else 0
    return matrix


def rectangular(adjacency_matrix):
    n = len(adjacency_matrix)
    d, r = [], []
    d_0 = adjacency_matrix
    r_0 = [[0 for i in range(n)] for i in range(n)]

    for i in range(n):
        for k in range(n):
            r_0[i][k] = k if adjacency_matrix[i][k] != 0 else None
    d.append(d_0)
    r.append(r_0)

    for j in range(n):
        i, k = 0, 0
        d_j, r_j = [[0 for i in range(n)] for i in range(n)], [[0 for i in range(n)] for i in range(n)]
        while i < n:
            if 8 in d_j[i]:
                i += 1
                k += 1
            while k < n:
                d_j[i][k] = d[j - 1][i][k]
                if i == k or i == j or k == j or d_j[i][k] <= d_j[i][j] + d_j[j][k]:
                    r_j[i][k] = k
                else:
                    r_j[i][k] = j
                k += 1
            i += 1
        d.append(d_j)
        r.append(r_j)

    return d[n - 1], r[n - 1]


def measure_time(method, matrix):
    time_f = timeit(f"f(m)", number=10, globals=dict(f=method, m=matrix))

    return time_f

if __name__ == "__main__":
    measured_time_rectangular = []
    measured_time_floyd = []
    for k in range(100, 400):
        print(k)
        matrix = generate_matrix(k)
        G = nx.Graph()
        for i in range(k):
            for j in range(k):
                G.add_edge(i, j, weight=matrix[i][j])

        time_f = measure_time(rectangular, matrix)
        measured_time_rectangular.append(time_f)

    for k in range(100, 400):
        print(k)
        matrix = generate_matrix(k)
        G = nx.Graph()
        for i in range(k):
            for j in range(k):
                G.add_edge(i, j, weight=matrix[i][j])

        time_f = measure_time(nx.floyd_warshall, G)
        measured_time_floyd.append(time_f)

    x = [i for i in range(100, 400)]

    plt.plot(x,measured_time_floyd, color='r', label="Floyd-Warshall")
    plt.plot(x,measured_time_rectangular, color="m", label="Rectangular")
    plt.legend()
    plt.show()

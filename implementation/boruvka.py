from networkx.algorithms.tree import minimum_spanning_tree
from timeit import timeit
import networkx as nx
import numpy as np
from random import randint

def generate_matrix(vertices):
    matrix = np.full((vertices, vertices), np.inf, dtype=int)
    for i in range(vertices):
        for k in range(vertices):
            matrix[i][k] = randint(1, vertices) if i != k else 0
    return matrix

class Graph:

    def __init__(self, vertices):
        self.V = vertices
        self.graph = []

    def addEdge(self, u, v, w):
        self.graph.append([u, v, w])

    def make_set(self, parent, rank, cheapest, node):
        parent.append(node)
        rank.append(0)
        cheapest = [-1] * self.V
        return parent, rank, cheapest

    def find(self, parent, i):
        result = i if parent[i] == i else self.find(parent, parent[i])
        return result

    def union(self, parent, rank, x, y):
        xroot, yroot = self.find(parent, x), self.find(parent, y)

        if rank[xroot] < rank[yroot]:
            parent[xroot] = yroot
        elif rank[xroot] > rank[yroot]:
            parent[yroot] = xroot
        else:
            parent[yroot] = xroot
            rank[xroot] += 1

    def boruvkaMST(self):
        parent = []
        rank = []

        cheapest = []

        numTrees = self.V
        MSTweight = 0

        for node in range(self.V):
            parent, rank, cheapest = self.make_set(parent, rank, cheapest, node)# n

        while numTrees > 1: # n - 1
            for i in range(len(self.graph)): # n
                u, v, w = self.graph[i] # 4
                set1 = self.find(parent, u)
                set2 = self.find(parent, v)

                if set1 != set2:

                    if cheapest[set1] == -1 or cheapest[set1][2] > w:
                        cheapest[set1] = [u, v, w]

                    if cheapest[set2] == -1 or cheapest[set2][2] > w:
                        cheapest[set2] = [u, v, w]

            for node in range(self.V):
                if cheapest[node] != -1:
                    u, v, w = cheapest[node]
                    set1 = self.find(parent, u)
                    set2 = self.find(parent, v)

                    if set1 != set2:
                        MSTweight += w
                        self.union(parent, rank, set1, set2)
                        numTrees -= 1

            # reset cheapest array
            cheapest = [-1] * self.V

        print("Weight of MST is %d" % MSTweight)
        return MSTweight


def measure_time(method, graph, algo):

    time_f = timeit(f"f(g, algorithm=algo)", number=10, globals=dict(f=method, g=graph, algo=algo))

    return time_f


if __name__ == "__main__":

    measured_time = []
    for k in range(200, 500):
        print(k)
        matrix = generate_matrix(k)
        G = nx.Graph()
        g = Graph(k)
        for i in range(k):
            for j in range(k):
                G.add_edge(i, j, weight=matrix[i][j])
                g.addEdge(i, j, matrix[i][j])

        mst = minimum_spanning_tree(G, "boruvka")
        time_f = measure_time(minimum_spanning_tree, G, "boruvka")
        measured_time.append(time_f)

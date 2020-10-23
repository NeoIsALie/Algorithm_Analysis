#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include <limits>
#include <chrono>

typedef int vertex_t;
typedef int weight_t;

struct Edge
{
    vertex_t x;
    vertex_t y;
    weight_t weight;
};


inline int BellmanFord(vertex_t source, vertex_t vertices, vertex_t edges,
                        std::vector<Edge> &edgesList, std::vector<int> &distances)
{
    distances[source] = 0;
    bool done = false;
    for (vertex_t i = 0; !done && i < vertices; i++)
    {
        done = true;
        for (vertex_t j = 0; j < edges; j++)
        {
            int so = edgesList[j].x;
            int de = edgesList[j].y;
            if (distances[so] + edgesList[j].weight < distances[de])
            {
                distances[de] = distances[so] + edgesList[j].weight;
                done = false;
            }
        }
    }
    if (!done)
        return -1;
    return 0;
}

std::vector<std::vector<int>> generateAdjacencyMatrix(int verticesLimit, int edgesLimit)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist1(0, edgesLimit);

    std::vector<std::vector<int>> matrix(verticesLimit);
    for (int i = 0; i < verticesLimit; i++)
    {
        matrix[i] = std::vector<int>(verticesLimit);
    }

    int edgesAllocated = 0;
    for (int i = 0; i < verticesLimit; i++)
    {
        for (int j = 0; j < verticesLimit; j++)
        {
            if (i != j && edgesAllocated < edgesLimit)
            {
                int a = dist1(rng);
                if (a != 0)
                {
                    matrix[i][j] = a;
                    matrix[j][i] = a;
                    edgesAllocated++;
                }
            }
        }
    }
    return matrix;
}

std::vector<Edge> convertToEdgeList(std::vector<std::vector<int>> adjacencyMatrix, int edgesLimit)
{
    std::vector<Edge> edgesList(2 * edgesLimit);
    int edgesStart = 0;
    for (size_t i = 0; i < adjacencyMatrix.size(); i++)
    {
        for (size_t j = 0; j < adjacencyMatrix.size(); j++)
        {
            if(adjacencyMatrix[i][j] > 0 && edgesStart < edgesLimit)
            {
                edgesList[edgesStart].x = i;
                edgesList[edgesStart].y = j;
                edgesList[edgesStart].weight = adjacencyMatrix[i][j];

                edgesStart++;
            }
        }
    }
    return edgesList;
}



int main()
{
    const weight_t max_weight = 10000;
    std::vector<std::vector<int>> adjacencyMatrix = generateAdjacencyMatrix(100, 500);
    std::vector<Edge> edgesList = convertToEdgeList(adjacencyMatrix, 1000);
    std::vector<int> distances(100, max_weight);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    BellmanFord(0, 100, 500, edgesList, distances);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    for(int i = 0; i < 100; i++)
    {
        std::cout << distances[i] << std::endl;
    }
    return 0;
}

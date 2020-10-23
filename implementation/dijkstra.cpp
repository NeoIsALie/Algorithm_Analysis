#include <iostream>
#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <list>
#include <chrono>
#include <limits> // for numeric_limits

#include <set>
#include <utility> // for pair
#include <algorithm>
#include <iterator>

// Dijkstra on priority queue (so it should be hella fast)


typedef int vertex_t;
typedef int weight_t;


struct neighbor {
    vertex_t target;
    weight_t weight;
    neighbor(vertex_t arg_target, weight_t arg_weight)
        : target(arg_target), weight(arg_weight) { }
};

typedef std::vector<std::vector<neighbor>> adjacency_list_t;


void DijkstraComputePaths(vertex_t source,
                          const adjacency_list_t &adjacency_list,
                          std::vector<weight_t> &min_distance,
                          std::vector<vertex_t> &previous,
                          const weight_t max_weight)
{
    min_distance.clear();
    min_distance.resize(adjacency_list.size(), max_weight);
    min_distance[source] = 0;
    previous.clear();
    previous.resize(adjacency_list.size(), -1);
    std::set<std::pair<weight_t, vertex_t> > vertex_queue;
    vertex_queue.insert(std::make_pair(min_distance[source], source));

    while (!vertex_queue.empty())
    {
        weight_t dist = vertex_queue.begin()->first;
        vertex_t u = vertex_queue.begin()->second;
        vertex_queue.erase(vertex_queue.begin());

        // Visit each edge exiting u
	const std::vector<neighbor> &neighbors = adjacency_list[u];
        for (std::vector<neighbor>::const_iterator neighbor_iter = neighbors.begin();
             neighbor_iter != neighbors.end();
             neighbor_iter++)
        {
            vertex_t v = neighbor_iter->target;
            weight_t weight = neighbor_iter->weight;
            weight_t distance_through_u = dist + weight;
    	    if (distance_through_u < min_distance[v]) {
    	        vertex_queue.erase(std::make_pair(min_distance[v], v));

    	        min_distance[v] = distance_through_u;
    	        previous[v] = u;
    	        vertex_queue.insert(std::make_pair(min_distance[v], v));

    	    }

        }
    }
}


std::list<vertex_t> DijkstraGetShortestPathTo(
    vertex_t vertex, const std::vector<vertex_t> &previous)
{
    std::list<vertex_t> path;
    for ( ; vertex != -1; vertex = previous[vertex])
        path.push_front(vertex);
    return path;
}



std::vector<std::vector<int>> generateAdjacencyMatrix(int verticesLimit, int edgesLimit)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist1(0,edgesLimit);

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
            if (j != i && edgesAllocated < edgesLimit)
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

adjacency_list_t convertToList(std::vector<std::vector<int>> adjacencyMatrix)
{
    int vertices = adjacencyMatrix.size();
    adjacency_list_t adjacency_list(vertices);

    for (size_t i = 0; i < vertices; i++)
    {
        for (size_t j = 0; j < vertices; j++)
        {
            if(adjacencyMatrix[i][j] != 0)
            {
                adjacency_list[i].push_back(neighbor(j, adjacencyMatrix[i][j]));
            }
        }
    }
    return adjacency_list;
}




int main()
{
    const weight_t max_weight = 10000;
    std::vector<std::vector<int>> matrix = generateAdjacencyMatrix(100, 500);
    adjacency_list_t adjList = convertToList(matrix);
    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    DijkstraComputePaths(0, adjList, min_distance, previous, max_weight);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    for (int i = 0; i < matrix.size(); i++)
    {
        std::cout << "Distance from 0 to  " << i << ":" << min_distance[i] << std::endl;
        std::list<vertex_t> path = DijkstraGetShortestPathTo(i, previous);
        std::cout << "Path : ";
        std::copy(path.begin(), path.end(), std::ostream_iterator<vertex_t>(std::cout, " "));
        std::cout << std::endl;
    }

    return 0;
}

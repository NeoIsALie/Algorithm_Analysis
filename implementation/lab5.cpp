#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>
#include <string>
#include <chrono>


std::vector<std::vector<int>> generateAdjacencyMatrix(int verticesLimit, int edgesLimit)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist1(0,1);

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
                if (a == 1)
                {
                    matrix[i][j] = a;
                    matrix[j][i] = a;
                    edgesAllocated++;
                }
            }
        }
    }

    for (int i = 0; i < verticesLimit; i++)
    {
        for (int j = 0; j < verticesLimit; j++)
        {
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << '\n';
    }

    return matrix;
}

std::vector<std::vector<int>> convert(std::vector<std::vector<int>> matrix)
{
    std::vector<std::vector<int>> adjList(matrix.size());
    for (int i = 0; i < matrix.size(); i++)
    {
        for (int j = 0; j < matrix[i].size(); j++)
        {
            if (matrix[i][j] == 1)
            {
                adjList[i].push_back(j);
            }
        }
    }

    // print the adjacency list
    for (int i = 0; i < adjList.size(); i++)
    {
        std::cout << i;
        for(int j = 0; j < adjList[i].size(); j++)
        {
            if(j == adjList[i].size() - 1)
            {
                std::cout << " -> " << adjList[i][j] << '\n';
                break;
            }
            else
                std::cout << " -> " << adjList[i][j];
        }
    }
    std::cout << ' ' << std::endl;
    return adjList;
}

// connected components
void dfs (int vertex, std::vector<std::vector<int>> adjList,
        std::vector<int> &used, std::vector<int> &components)
{
	used[vertex] = 1;
	components.push_back (vertex);
	for (size_t i = 0; i < adjList[vertex].size(); i++) {
		int to = adjList[vertex][i];
		if (used[to] == 0)
			dfs (adjList[vertex][i], adjList, used, components);
	}
}

void find_comps(std::vector<std::vector<int>> adjList)
{
    std::vector<int> used(adjList.size(), 0);
    std::vector<int> components;
	for (int i = 0 ; i < adjList.size(); i++)
    {
		if (used[i] == 0)
        {
            for (size_t j = 0; j < components.size(); j++)
                std::cout << ' ' << components[j];

			components.clear();
			dfs (i, adjList, used, components);
			std::cout << "Component: ";
			for (size_t j = 0; j < components.size(); j++)
				std::cout << ' ' << components[j];
			std::cout << std::endl;
		}
    }
}

void bfs(std::vector<std::vector<int>> adjList, int startVertex)
{
    std::queue<int> q;
    q.push(startVertex);
    std::vector<bool> used(adjList.size());
    std::vector<int> paths(adjList.size()), parents(adjList.size());
    used[startVertex] = true;
    parents[startVertex] = -1;
    while (!q.empty())
    {
    	int vertex = q.front();
    	q.pop();
    	for (size_t i = 0; i < adjList[vertex].size(); i++)
        {
    		int to = adjList[vertex][i];
    		if (!used[to])
            {
    			used[to] = true;
    			q.push(to);
    			paths[to] = paths[vertex] + 1;
    			parents[to] = vertex;
    		}
            if (!used[to])
            	std::cout << "No path!";
            else {
            	std::vector<int> path;
            	for (int v = to; v != -1; v = parents[v])
            		path.push_back(v);
            	reverse(path.begin(), path.end());
            	std::cout << "Path: ";
            	for (size_t i = 0; i < path.size(); i++)
            		std::cout << path[i] + 1 << " ";
                std::cout << "\n";
            }
    	}

    }
}

// export to dotfile (future visualization)

void toDotFile(std::vector<std::vector<int>> adjList)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist1000(0,1000);


    std::string dotText = "graph generatedGraph {\n";
    for(int i = 0; i < adjList.size(); i++)
    {
        for (int j = 0; j < adjList[i].size(); j++)
        {
            dotText.append(std::to_string(i) + " -- " + std::to_string(j) + "\n");
        }
    }
    dotText.append("}");
    std::cout << dotText << '\n';

    std::ofstream out("graphLab5" + std::to_string(dist1000(rng)) + ".txt");
    out << dotText;
    out.close();
}

void toDot(std::vector<std::vector<int>> adjacencyMatrix)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist1000(0,1000);

    std::string dotText = "graph generatedGraph {\n";
    for (int diagonal = adjacencyMatrix.size() - 1; diagonal >= 0; --diagonal) {
        for (int i = 0; i + diagonal < adjacencyMatrix.size(); ++i) {
            if(adjacencyMatrix[diagonal][i] == 1)
                dotText.append(std::to_string(diagonal) + " -- " + std::to_string(i) + "\n");
        }
    }
    dotText.append("}");

    std::ofstream out("graphLab5" + std::to_string(dist1000(rng)) + ".txt");
    out << dotText;
    out.close();

}

int main()
{
    std::vector<std::vector<int>> matrix = generateAdjacencyMatrix(100, 200);
    std::vector<std::vector<int>> adjList = convert(matrix);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    find_comps(adjList);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    begin = std::chrono::steady_clock::now();
    bfs(adjList, 0);
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    toDot(matrix);
}

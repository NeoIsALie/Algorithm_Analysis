#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

typedef std::vector<std::vector<int>> matrix_t;

matrix_t generateMatrix(size_t size)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist1(1, size);

    matrix_t matrix(size);
    for (size_t i = 0; i < size; i++)
        matrix[i] = std::vector<int>(size);

    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
            matrix[i][j] = dist1(rng);
    }

    return matrix;
}

matrix_t matrixProductionNaive(matrix_t left, matrix_t right)
{
    matrix_t result(left.size());
    for (int i = 0; i < result.size(); i++)
        result[i] = std::vector<int>(left.size(), 0);

    for (size_t i = 0; i < left.size(); i++) {
        for (size_t j = 0; j < right.size(); j++) {
            result[i][j] = 0;
            for (size_t k = 0; k < left.size(); k++)
                result[i][j] += left[i][k] * right[k][j];
        }
    }
    return result;
}

matrix_t winogradMultiplication(matrix_t left, matrix_t right)
{
    matrix_t result(left.size());
    for (int i = 0; i < result.size(); i++)
        result[i] = std::vector<int>(right[0].size(), 0);

    int m = left.size();
    int n = right.size();
    int q = left[0].size();

    std::cout << m << " " << n << " " << q << "\n";

    std::vector<int> mulH (m, 0);
    std::vector<int> mulV (q, 0);

    for (size_t i = 0; i < m; i++){
        for (size_t j = 0; j < n - 1; j += 2)
            mulH[i] += left[i][j] * left[i][j + 1];
    }

    for (size_t i = 0; i < q; i++){
        for (size_t j = 0; j < n - 1; j += 2)
            mulV[i] += right[j][i] * right[j + 1][i];
    }

    for (size_t i = 0; i < m; i++){
        for (size_t j = 0; j < q; j++){
            result[i][j] = - (mulH[i] + mulV[j]);
            for (size_t k = 0; k < n - 1; k += 2)
                result[i][j] += (left[i][k] + right[k + 1][j]) * (left[i][k + 1] + right[k][j]);
            result[i][j] = (n % 2 != 0) ? result[i][j] + left[i][n - 1] * right[n - 1][j] : 0;
        }
    }
    return result;
}

int main()
{
   matrix_t matrix = generateMatrix(1000);
}

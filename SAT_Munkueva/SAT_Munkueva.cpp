#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <cstdio>

using namespace std;
vector<char> root;
vector<int> columnsToDelete;

#define MAX_LINE_LENGTH 100

int num_vars = 0;
int num_conjunctions = 0;
vector<vector<char>> matrix;
// Считываем pla файл
void read_pla_file(char* file_name) {
    FILE* file = nullptr;
    if (fopen_s(&file, file_name, "r") != 0) {
        printf("Ошибка! Невозможно открыть файл %s\n", file_name);
        return;
    }
    // Инициализируем переменные
    int current_line = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        current_line++;

        // Игнорируем комментарии и пустые строки
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        // Проврека входа
        if (line[0] == '.') { // количество переменный
            if (line[1] == 'i') {
                sscanf_s(line, ".i %d", &num_vars);
            }
            else if (line[1] == 'p') { // количество конъюнкций
                sscanf_s(line, ".p %d", &num_conjunctions);
                matrix.resize(num_conjunctions);
            }
        }
        else {
            if (current_line - 3 >= num_conjunctions)
            {
                break;
            }
            matrix[current_line - 3].resize(num_vars);
            strncpy_s(matrix[current_line - 3].data(), num_vars + 1, line, _TRUNCATE);
        }
    }

    // Выводим количество переменных и конъюнкий
    printf("Количество переменных: %d\n", num_vars);
    printf("Количество конъюнкций: %d\n", num_conjunctions);

    // Ввыодим сами конъюнкции
    for (int i = 0; i < num_conjunctions; i++) {
        printf("Конъюнкция %d: %s", i + 1, matrix[i].data());
        cout << endl;
    }

    fclose(file);
}






vector<vector<char>> lastmatrix = matrix; // Копируем матрицу
void solveDNF(bool start)
{
    // Правило 1: проверяем, есть ли только один '0' или '1' в строке, и удаляем строки 
    for (int i = 1; i < matrix.size(); i++)
    {
        int ones = 0, zeroes = 0, empty = 0;
        int index1 = -1, index0 = -1;
        int sizet = matrix[i].size();
        for (int j = 0; j < matrix[i].size(); j++)
        {
            if (matrix[i][j] == '1')
            {
                ones++;
                index1 = j;
            }
            else if (matrix[i][j] == '0')
            {
                zeroes++;
                index0 = j;
            }
            else
            {
                empty++;
            }
        }
        if (ones == 1 && matrix[i].size() == empty + 1) // есть единственный '1' в этой строке, и остальные символы -
        {
            int root_index = index1;
            root[(int)matrix[0][root_index]] = '0';


            i--;
            if (matrix[0].size() > 0)
            {
                for (int v = 1; v < matrix.size(); v++)
                {
                    if (matrix[v][root_index] == '1')
                    {
                        matrix.erase(matrix.begin() + v);
                        v--;
                    }
                }
            }
            else
            {
                break;
            }
            for (int k = 0; k < matrix.size(); k++)
            {
                matrix[k].erase(matrix[k].begin() + root_index);
            }
            continue;
        }
        else if (zeroes == 1 && matrix[i].size() == empty + 1) // есть единственный '0' в этой строке, и остальные символы -
        {
            int root_index = index0;
            root[(int)matrix[0][root_index]] = '1';

            i--;
            if (matrix[0].size() > 0)
            {
                for (int v = 1; v < matrix.size(); v++)
                {
                    if (matrix[v][root_index] == '0')
                    {
                        matrix.erase(matrix.begin() + v);
                        v--;
                    }
                }
            }
            else
            {
                break;
            }
            for (int k = 0; k < matrix.size(); k++)
            {
                matrix[k].erase(matrix[k].begin() + root_index);
            }
            continue;
        }
        if (matrix[0].size() == 0) // если нет больше столбцов, выходим из правила
        {
            break;
        }
    }

    // Правило 2: проверяем, все ли значения в строке равны «-»
    for (int i = 1; i < matrix.size(); i++)
    {
        if (matrix[0].size() > 0)
        {
            bool allMinus = true; //флаг для проверки, содержат ли все символы в строке '-'.

            for (int j = 0; j < matrix[i].size(); j++)
            {
                if (matrix[i][j] != '-')
                {
                    allMinus = false;
                    break;
                }
            }
            if (allMinus)
            {
                cout << "Корня не существует" << endl;
                return;
            }
        }
    }
    for (int j = 0; j < matrix[0].size(); j++)
    {
        bool allZeroes = true;
        for (int i = 1; i < matrix.size(); i++)
        {
            if (matrix[i][j] != '-')
            {
                allZeroes = false;
                break;
            }
        }
        if (allZeroes)
        {
            for (int i = 0; i < matrix.size(); i++)
            {
                matrix[i].erase(matrix[i].begin() + j); // удаляет столбцы, в которых все значения являются '-'
            }
            j--;
        }
    }





int main()
{
    char adress[] = "dnfRnd_test.pla";
    read_pla_file(adress);
    int rowLength = matrix[0].size();
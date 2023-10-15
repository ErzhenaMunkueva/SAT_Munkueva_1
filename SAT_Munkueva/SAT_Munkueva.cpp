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
        if (line[0] == '.') {
            if (line[1] == 'i') {
                sscanf_s(line, ".i %d", &num_vars);
            }
            else if (line[1] == 'p') {
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
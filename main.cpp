/*Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
p <= 30000, n <= 300000.
Использовать один из методов:
- С помощью префикс-функции;
- С помощью z-функции.*/

#include <iostream>
#include <stdio.h>
#include <vector>

void find_occurence(std::string& pattern, std::vector<uint32_t>& result){
    uint32_t current_pref_len = 0; //длина текущего рассматриваемого префикса
    std::vector<uint32_t> prefix_pattern_function(pattern.size(), 0);

    for (uint32_t i = 1; i < pattern.size(); ++i){ //считаем префикс-функцию для паттерна
        current_pref_len = prefix_pattern_function[i - 1];

        while (pattern[current_pref_len] != pattern[i] && current_pref_len != 0) {
            current_pref_len = prefix_pattern_function[current_pref_len - 1];
        }

        if (pattern[current_pref_len] == pattern[i]) {
            prefix_pattern_function[i] = current_pref_len + 1;
        }
        else {
            prefix_pattern_function[i] = 0;
        }
    }

    char s = getchar();
    uint32_t index = 0;
    current_pref_len = 0;
    pattern += '#'; //разделитель для корректного подсчета префикс-функции

    while ((s = getchar()) != '\n') {
        while(s != pattern[current_pref_len] && current_pref_len != 0) {
            current_pref_len = prefix_pattern_function[current_pref_len - 1];
        }

        if (s == pattern[current_pref_len]) {
            ++current_pref_len;

            if (current_pref_len == pattern.size() - 1){
                result.push_back(index - pattern.size() + 2);
            }
        }

        ++index;
    }
}


int main() {
    std::string pattern("");
    std::vector<uint32_t> result(0);

    std::cin >> pattern;

    find_occurence(pattern, result);

    for (auto v:result)  {
        std::cout << v << ' ';
    }

    return 0;
}
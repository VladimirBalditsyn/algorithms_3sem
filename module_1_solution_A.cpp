/*Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
p <= 30000, n <= 300000.
Использовать один из методов:
- С помощью префикс-функции;
- С помощью z-функции.*/

#include <fstream>
#include <vector>
#include <iterator>
#include <iostream>

std::vector<int> prefix_function(const std::string& text) {
    std::vector<int> prefix_function_result(text.length(), 0);
    uint32_t  current_pref_len = 0;

    for (uint32_t i = 1; i < text.length(); ++i){ //считаем префикс-функцию для паттерна
        current_pref_len = prefix_function_result[i - 1];

        while (text[current_pref_len] != text[i] && current_pref_len != 0) {
            current_pref_len = prefix_function_result[current_pref_len - 1];
        }

        if (text[current_pref_len] == text[i]) {
            prefix_function_result[i] = current_pref_len + 1;
        }
        else {
            prefix_function_result[i] = 0;
        }
    }
    return prefix_function_result;
}

template <class InputIterator, class OutputIterator>
void find_occurence(InputIterator& input, std::string& pattern, OutputIterator& output){
    std::vector<int> pattern_prefix_function = prefix_function(pattern);

    uint32_t current_pref_len = 0; //длина текущего рассматриваемого префикса
    char s; //текущий символ
    uint32_t index = 0;
    pattern += '#'; //разделитель для корректного подсчета префикс-функции
    while (input != std::istream_iterator<char>()) {
        s = *input;
        while(s != pattern[current_pref_len] && current_pref_len != 0) {
            current_pref_len = pattern_prefix_function[current_pref_len - 1];
        }

        if (s == pattern[current_pref_len]) {
            ++current_pref_len;

            if (current_pref_len == pattern.size() - 1) {
                *output = index - pattern.size() + 2;
            }
        }

        ++index;
        ++input;
    }
}


int main() {
    std::string pattern;

    std::cin >> pattern;

    std::cin.tie(nullptr);
    std::istream_iterator<char> input_iter(std::cin);
    std::ostream_iterator<size_t> output_iter(std::cout, " ");

    find_occurence<std::istream_iterator<char>, std::ostream_iterator<size_t>>(input_iter, pattern, output_iter);

    return 0;
}

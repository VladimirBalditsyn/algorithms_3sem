/*Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
p <= 30000, n <= 300000.
Использовать один из методов:
- С помощью префикс-функции;
- С помощью z-функции.*/

#include <fstream>
#include <vector>
#include <iterator>
#include <iostream>

size_t continue_prefix_function(const char c, size_t current_pref_len, const std::string& text,
        const std::vector<size_t>& prefix_function) {

    while (text[current_pref_len] != c && current_pref_len != 0) {
        current_pref_len = prefix_function[current_pref_len - 1];
    }

    if (text[current_pref_len] == c) {
        return current_pref_len + 1;
    }
    else {
        return 0;
    }
}

std::vector<size_t> prefix_function(const std::string& text) {
    std::vector<size_t> prefix_function_result(text.length(), 0);

    for (size_t i = 1; i < text.length(); ++i) { //считаем префикс-функцию для паттерна
        prefix_function_result[i] =
                continue_prefix_function(text[i], prefix_function_result[i - 1], text, prefix_function_result);
    }
    return prefix_function_result;
}

template <class InputIterator, class OutputIterator>
void find_occurence(InputIterator& input_start, InputIterator& input_end,
        std::string& pattern, OutputIterator& output) {

    std::vector<size_t> pattern_prefix_function = prefix_function(pattern);

    uint32_t current_pref_len = 0; //длина текущего рассматриваемого префикса
    char c; //текущий символ
    uint32_t index = 0;
    pattern += '#'; //разделитель для корректного подсчета префикс-функции

    while (input_start != input_end) {
        c = *input_start;
        current_pref_len = continue_prefix_function(c, current_pref_len, pattern, pattern_prefix_function);

        if (current_pref_len == pattern.size() - 1) {
            *output = index - pattern.size() + 2;
        }
        ++index;
        ++input_start;
    }
}


int main() {
    std::string pattern;

    std::cin >> pattern;

    std::cin.tie(nullptr);
    std::istream_iterator<char> input_iter(std::cin);
    std::istream_iterator<char> input_iter_end = std::istream_iterator<char>();
    std::ostream_iterator<size_t> output_iter(std::cout, " ");

    find_occurence<>(input_iter, input_iter_end, pattern, output_iter);

    return 0;
}

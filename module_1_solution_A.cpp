/*Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
p <= 30000, n <= 300000.
Использовать один из методов:
- С помощью префикс-функции;
- С помощью z-функции.*/

#include <iostream>
#include <vector>

std::vector<int> prefix_function(std::string& text) {
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

template <class Container>
void find_occurence(std::istream& input, std::string& pattern, std::insert_iterator<Container>& result){
    std::vector<int> pattern_prefix_function = prefix_function(pattern);

    uint32_t current_pref_len = 0; //длина текущего рассматриваемого префикса
    char s = input.get(); //текущий символ
    uint32_t index = 0;
    pattern += '#'; //разделитель для корректного подсчета префикс-функции

    while ((s = input.get()) != '\n') {
        while(s != pattern[current_pref_len] && current_pref_len != 0) {
            current_pref_len = pattern_prefix_function[current_pref_len - 1];
        }

        if (s == pattern[current_pref_len]) {
            ++current_pref_len;

            if (current_pref_len == pattern.size() - 1) {
                result = index - pattern.size() + 2;
                ++result;
            }
        }

        ++index;
    }
}


int main() {
    std::string pattern("");
    std::vector<int> result(0);

    std::cin >> pattern;

    std::insert_iterator<std::vector<int>> iter =
            std::insert_iterator<std::vector<int>>(result, result.begin());
    
    find_occurence<std::vector<int>>(std::cin, pattern, iter);

    for (auto v : result)  {
        std::cout << v << ' ';
    }

    return 0;
}

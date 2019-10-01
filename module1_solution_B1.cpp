/*Найти лексикографически-минимальную строку, построенную по префикс-функции, в алфавите a-z.*/
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

template <size_t T>
size_t least_significant_bit(std::bitset<T>& set) {
    for (int k = 1; k < T; ++k){
        if (set[k] != false){
            return k;
        }
    }
    return T;
}

std::string make_string(const std::vector<size_t >& pref_func) {
    std::string answer;
    constexpr size_t alphabet_size = 26;
    constexpr char first_letter = 'a';
    answer += first_letter;
    std::bitset<alphabet_size> alphabet;
    alphabet.set();
    for (size_t i = 1; i < pref_func.size(); ++i) {
        if (pref_func[i] == 0) {
            size_t prev_pref_func = pref_func[i - 1]; //значение префикс-функции для символа, стоящего перед
            // рассматриваемым
            alphabet[0] = false; //при префик-функции равной 0 новый символ не 'a'

            while (prev_pref_func != 0) {
                //проверяем, что не продлеваем  префикс меньшей длины
                alphabet[answer[prev_pref_func] - first_letter] = false;
                prev_pref_func = pref_func[prev_pref_func - 1];
            }

            size_t left_letter = least_significant_bit<alphabet_size>(alphabet);
            answer += static_cast<char>(left_letter + first_letter);

            alphabet.set();
            continue;
        }
        answer += static_cast<char>(answer[pref_func[i] - 1]);
    }
    return answer;
}

int main() {
    std::vector<size_t > input;
    size_t c = 0;
    while(std::cin >> c) {
        input.push_back(c);
    }
    std::string answer = make_string(input);
    std::cout << answer;
    return 0;
}

/*Дана строка длины n. Найти количество ее различных подстрок. 
Используйте суффиксный массив.

Построение суффиксного массива выполняйте за O(n log n). 
Вычисление количества различных подстрок выполняйте за O(n).*/

#include <istream>
#include <vector>
#include <iostream>

constexpr uint16_t ASCII_TABLE_SIZE = 256;

class suffix_array {
public:
    explicit suffix_array(const std::string& _str);

    std::vector<int32_t> array; //suffix array of _str
    std::vector<int32_t> lcp_array; //lcp[i] = length of least common prefix of suffix begins in array[i]
                                    //and suffix begins in array[i + 1]
    std::vector<size_t> position_array; //if array[i] = k, then position_array[k] = i

private:
    std::string_view str;
    std::vector<size_t> eq_classes; //vector< which contains for each position number of his equivalent class
    size_t eq_classes_amount; //amount of equivalents classes
    void counting_sort();
    void update_eq_classes(size_t step);
    void init_suf_array();
    void init_lcp_array();
};

suffix_array::suffix_array(const std::string& _str)
                : str(_str)
                , eq_classes_amount(ASCII_TABLE_SIZE) {
    init_suf_array();

    position_array.assign(str.length(), 0);
    for (size_t i = 0; i < str.length(); ++i) {
        position_array[array[i]] = i;
    }

    init_lcp_array();
}

void suffix_array::init_suf_array() {
    eq_classes.assign(str.length(), 0);
    array.assign(str.length(), 0);
    for (size_t i = 0; i < str.length(); ++i){ //preparation
        eq_classes[i] = str[i];
        array[i] = i;
    }

    //sort one symbol:
    counting_sort();
    eq_classes_amount = 1;
    eq_classes[array[0]] = 0;
    for (size_t i = 1; i < str.length(); ++i) {
        if (str[array[i]] != str[array[i - 1]])
            ++eq_classes_amount;
        eq_classes[array[i]] = eq_classes_amount - 1;
    }

    int32_t byte_to_move = 1;
    //sort 2^k cyclic permutation by sorting pair of 2^(k-1) permutation
    for (size_t i = 0; (str.length() >> i) > 0; ++i) {
        for (size_t j = 0; j < str.length(); ++j) {
            array[j] = (array[j] - (1 << i) >= 0 ? array[j] - (1 << i)
                    : str.length() + (array[j] - (1 << i))); //second element of pair is sorted
                                           // => to sort first subtract 2^(i) == 2^(k - 1)
        }
        counting_sort();
        update_eq_classes(i);
    }
}

void suffix_array::update_eq_classes(size_t step) {
    std::vector<size_t> prev_eq_classes = eq_classes; //number of equivalents classes on previous (k-1) stage
    eq_classes[array[0]] = 0;
    eq_classes_amount = 1;
    for (size_t i = 1; i < str.length(); ++i) {
        int32_t pair1_second = (array[i] + (1 << step)) % str.length();
        int32_t pair2_second = (array[i - 1] + (1 << step)) % str.length();
        if (prev_eq_classes[array[i]] != prev_eq_classes[array[i - 1]] ||
            prev_eq_classes[pair1_second] != prev_eq_classes[pair2_second])
            ++eq_classes_amount;
        eq_classes[array[i]] = eq_classes_amount - 1;
    }
}

void suffix_array::counting_sort() {
    std::vector<size_t> counter(eq_classes_amount, 0);
    std::vector<int32_t> prev_array = array; //suffix array on previous stage
    for (size_t i = 0; i < str.length(); ++i) {
        ++counter[eq_classes[i]];
    }
    for (size_t i = 1; i < eq_classes_amount; ++i){
        counter[i] += counter[i - 1];
    }
    for (int32_t i = prev_array.size() - 1; i >= 0; --i) {
        array[--counter[eq_classes[prev_array[i]]]] = prev_array[i];
    }
}

void suffix_array::init_lcp_array() {
    lcp_array.assign(str.length(), 0);
    int32_t current_common_pref = 0;

    for (int i = 0; i < str.length() - 1; ++i) {
        if (position_array[i] == str.length() - 1) {
            lcp_array[position_array[i]] = -1; //because it is undefined
            current_common_pref = 0;
            continue;
        }
        else {
            if (current_common_pref > 0) //because lcp[e] >= lcp[e-1] - 1, e - index in array
                --current_common_pref;
            int32_t j = array[position_array[i] + 1]; //index of beginning next suffix after i-suffix in array
            while ((i + current_common_pref < str.length())
                && (j + current_common_pref < str.length())
                && str[i + current_common_pref] == str[j + current_common_pref])
                ++current_common_pref;
            lcp_array[position_array[i]] = current_common_pref;
        }
    }
}

int32_t count_unique_substrings(const std::string& str) {
    suffix_array suf(str + "#");
    int32_t result = 0;

    for (int i = 1; i < str.length(); ++i) {
        result += str.length() - suf.array[i]; //amount of suffixes which begin in i position
        result -= suf.lcp_array[i]; //-amount of prefixes which are common with suffix in (i + 1) position
    }

    result += str.length() - suf.array[str.length()];

    return result;
}

int main() {
    std::string str;
    std::cin >> str;
    std::cout << count_unique_substrings(str);
    return 0;
}

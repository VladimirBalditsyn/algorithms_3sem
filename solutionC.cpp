/*Шаблон поиска задан строкой длины m, в которой кроме обычных символов
 * могут встречаться символы “?”. Найти позиции всех вхождений шаблона
 * в тексте длины n. Каждое вхождение шаблона предполагает, что все обычные
 * символы совпадают с соответствующими из текста, а вместо символа “?”
 * в тексте встречается произвольный символ.
Время работы - O(n + m + Z), где Z - общее -число вхождений подстрок шаблона
 “между вопросиками” в исходном тексте. m ≤ 5000, n ≤ 2000000.*/

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <memory>
#include <array>

constexpr char first_letter = 'a';
constexpr size_t alphabet_size = 26;

class Trie {
public:
    Trie(const std::vector<std::pair<std::string_view, size_t>>& subpatterns);
    std::vector<uint32_t> Aho_Korasick(const char c);

private:
    struct Node {
        Node(char _letter, bool _is_terminate)
                : letter(_letter)
                , is_terminate(_is_terminate) { }
        Node(char _letter, bool _is_terminate, std::shared_ptr<Node> _parent)
                : letter(_letter)
                , is_terminate(_is_terminate)
                , parent(_parent) { }

        char letter = -1;
        bool is_terminate = false;
        std::vector<size_t> index_of_pattern;
        std::array<std::shared_ptr<Node>, alphabet_size> next_vertices;
        std::weak_ptr<Node> parent;
        std::weak_ptr<Node> suf_link;
        std::weak_ptr<Node> short_suf_link;
    };

    std::shared_ptr<Node> root;
    std::shared_ptr<Node> current_vertex;

    void add_next_vertices(std::shared_ptr<Node> vertex, std::queue<std::shared_ptr<Node>>& output);
    void create_suf_ptr();
    void add_pattern(const std::string_view& pattern, size_t pattern_index);
};

Trie::Trie(const std::vector<std::pair<std::string_view, size_t>>& subpatterns) {
    root = std::make_shared<Node>(-1, false);

    for (size_t i = 0; i < subpatterns.size(); ++i){
        add_pattern(subpatterns[i].first, i);
    }

    create_suf_ptr();

    current_vertex = root;
}

void Trie::add_pattern(const std::string_view& pattern, size_t pattern_index) {
    std::shared_ptr<Node> current = root;
    for (auto c : pattern){
        if (current->next_vertices[c - first_letter] != nullptr) {
            current = current->next_vertices[c - first_letter];
        }
        else {
            current->next_vertices[c - first_letter] =
                    std::make_shared<Node>(c, false, current);
            current = current->next_vertices[c - first_letter];
        }
    }
    current->is_terminate = true;
    current->index_of_pattern.push_back(pattern_index);
}

void Trie::add_next_vertices(std::shared_ptr<Node> vertex, std::queue<std::shared_ptr<Node>>& output) {
    for (auto v : vertex->next_vertices) {
        if (v != nullptr) output.push(v);
    }
}

void Trie::create_suf_ptr() {
    std::queue<std::shared_ptr<Node>> BFS_queue;

    char step;

    root->suf_link = root;
    for (auto v : root->next_vertices) {
        if (v != nullptr) {
            v->suf_link = root;
            add_next_vertices(v, BFS_queue);
        }
    }

    while (!BFS_queue.empty()) {
        std::shared_ptr<Node> current = BFS_queue.front();
        std::shared_ptr<Node> current_suf = current->parent.lock();
        step = current->letter;
        BFS_queue.pop();

        while (true) { //построение суффиксных ссылок bfs
            if (current_suf->suf_link.lock()->next_vertices[step - first_letter] != nullptr) {
                current->suf_link = current_suf->suf_link.lock()->next_vertices[step - first_letter];
                break;
            }
            else if (current_suf->suf_link.lock() == root) {
                current->suf_link = root;
                break;
            }
            else {
                current_suf = current_suf->suf_link.lock();
            }
        }

        current_suf = current;

        while (true) { //построение сокращённых суффиксных ссылок bfs
            if (current_suf->suf_link.lock() == root){
                //current->short_suf_link = nullptr;
                break;
            }
            else if (current_suf->suf_link.lock()->is_terminate) {
                current->short_suf_link = current_suf->suf_link;
                break;
            }
            else {
                current_suf = current_suf->suf_link.lock();
            }
        }
        add_next_vertices(current, BFS_queue);
    }
}

std::vector<std::pair<std::string_view, size_t>> pattern_split(std::string_view pattern, const char separator) {
    std::vector<std::pair<std::string_view, size_t>> result;
    size_t last_subpattern_index = 0;
    for (size_t i = 0; i < pattern.length(); ++i) {
        if (pattern[i] == separator){
            if (i - last_subpattern_index > 0) {
                result.emplace_back(std::make_pair(std::string_view(pattern.substr(last_subpattern_index,
                        i - last_subpattern_index)), i - 1));
            }
            last_subpattern_index = i + 1;
        }
    }
    if (last_subpattern_index != pattern.length()) {
        result.emplace_back(std::make_pair(pattern.substr(last_subpattern_index,
                pattern.length() - last_subpattern_index), pattern.length() - 1));
    }
    return result;
}

std::vector<uint32_t> Trie::Aho_Korasick(const char c) {
    std::shared_ptr<Node> short_suf;
    std::vector<uint32_t> result;

    while (true) {
        if (current_vertex->next_vertices[c - first_letter] != nullptr) {
            current_vertex = current_vertex->next_vertices[c - first_letter];

            if (current_vertex->is_terminate) {
                for (auto v : current_vertex->index_of_pattern) {
                    result.push_back(v);
                }
            }

            short_suf = current_vertex->short_suf_link.lock();

            while (short_suf) {
                for (auto v : short_suf->index_of_pattern) {
                    result.push_back(v);
                }
                short_suf = short_suf->short_suf_link.lock();
            }
            break;
        } 
        else if (current_vertex != root) {
            while (current_vertex != root) {
                current_vertex = current_vertex->suf_link.lock();
                if (current_vertex->next_vertices[c - first_letter] != nullptr) break;
            }
        } 
        else break;
    }
    return result;
}

std::vector<uint32_t> find_patterns_occurances(std::string_view pattern, const std::string& text) {
    std::vector<std::pair<std::string_view, size_t>> subpatterns = pattern_split(pattern, '?');
    std::vector<uint16_t> count_occurances(text.length(), 0);
    std::vector<uint32_t> answer;
    Trie trie(subpatterns);

    for (size_t i = 0; i < text.length(); ++i) {
        std::vector<uint32_t> subpattern_occurences_index = trie.Aho_Korasick(text[i]);
        for (auto v : subpattern_occurences_index) {
            if (i >= subpatterns[v].second)
                ++count_occurances[i - subpatterns[v].second];
        }
    }
    size_t text_iterate_size = (text.length() > pattern.length() - 1 ? text.length() - pattern.length() + 1 : 0);
    for (size_t i = 0; i < text_iterate_size; ++i) {
        if (count_occurances[i] == subpatterns.size())
            answer.push_back(i);
    }
    return answer;
}

int main() {
    std::string text;
    std::string pattern;
    std::cin >> pattern;
    std::cin >> text;
    std::vector<uint32_t> result = find_patterns_occurances(std::string_view(pattern), text);
    for (auto v : result) {
        std::cout << v << ' ';
    }
    return 0;
}

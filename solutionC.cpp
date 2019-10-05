/*Шаблон поиска задан строкой длины m, в которой кроме обычных символов 
могут встречаться символы “?”. Найти позиции всех вхождений шаблона в тексте длины n. 
Каждое вхождение шаблона предполагает, что все обычные символы совпадают с соответствующими из текста, 
а вместо символа “?” в тексте встречается произвольный символ. 
Время работы - O(n + m + Z), где Z - общее -число вхождений подстрок шаблона “между вопросиками” в исходном тексте. 
m ≤ 5000, n ≤ 2000000.*/
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <memory>

constexpr char first_letter = 'a';
constexpr size_t alphabet_size = 26;

struct Node {
    Node() = default;
    Node(char _letter, bool _is_terminate)
        : letter(_letter)
        , is_terminate(_is_terminate) {
        next_vertices = std::vector<std::shared_ptr<Node>>(26, nullptr);
    }
    Node(char _letter, bool _is_terminate, std::shared_ptr<Node> _parent)
        : letter(_letter)
        , is_terminate(_is_terminate)
        , parent(_parent) {
        next_vertices = std::vector<std::shared_ptr<Node>>(26, nullptr);
    }

    char letter = -1;
    bool is_terminate = false;
    std::vector<size_t> index_of_pattern;
    std::vector<std::shared_ptr<Node>> next_vertices;
    std::weak_ptr<Node> parent;
    std::weak_ptr<Node> suf_link;
    std::weak_ptr<Node> short_suf_link;
};

std::shared_ptr<Node> add_pattern(std::shared_ptr<Node> root, const std::string& pattern, size_t pattern_index) {
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
    return root;
}

void get_next_vertices(std::shared_ptr<Node> vertex, std::queue<std::shared_ptr<Node>>& output) {
    for (auto v : vertex->next_vertices) {
        if (v != nullptr) output.push(v);
    }
}

void create_suf_ptr(std::shared_ptr<Node> root) {
    std::queue<std::shared_ptr<Node>> BFS_queue;

    char step;

    root->suf_link = root;
    for (auto v : root->next_vertices) {
        if (v != nullptr) {
            v->suf_link = root;
            get_next_vertices(v, BFS_queue);
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
        get_next_vertices(current, BFS_queue);
    }
}

std::shared_ptr<Node> create_trie(const std::vector<std::string>& patterns) {
    std::shared_ptr<Node> root = std::make_shared<Node>(-1, false);

    for (size_t i = 0; i < patterns.size(); ++i){
        root = add_pattern(root, patterns[i], i);
    }


    create_suf_ptr(root);
    return root;
}

std::vector<std::string> pattern_split(const std::string& pattern, const char spliter, std::vector<size_t>&
        subpt_dist) {
    std::vector<std::string> result;
    size_t last_index = 0;
    for (size_t i = 0; i < pattern.length(); ++i) {
        if (pattern[i] == spliter){
            if (i - last_index > 0) {
                result.push_back(pattern.substr(last_index, i - last_index));
                subpt_dist.push_back(i - 1);
            }
            last_index = i + 1;
        }
    }
    if (last_index != pattern.length()) {
        result.push_back(pattern.substr(last_index, pattern.length() - last_index));
        subpt_dist.push_back(pattern.length() - 1);
    }
    return result;
}


std::vector<uint32_t> find_patterns_incomings(const std::string& pattern, const std::string& text) {
    std::vector<size_t> subpattern_distance;
    std::vector<std::string> patterns = pattern_split(pattern, '?', subpattern_distance);
    std::shared_ptr<Node> trie = create_trie(patterns);
    std::vector<uint16_t> count_incomings(text.length(), 0);

    std::shared_ptr<Node> current_vertex = trie;
    std::shared_ptr<Node> short_suf;

    for (size_t i = 0; i < text.length(); ++i) {
         if (current_vertex->next_vertices[text[i] - first_letter] != nullptr) {
             current_vertex = current_vertex->next_vertices[text[i] - first_letter];

             if (current_vertex->is_terminate) {
                 for (auto v : current_vertex->index_of_pattern) {
                    if (subpattern_distance[v] <= i)
                        ++count_incomings[i - subpattern_distance[v]];
                 }
             }

             short_suf = current_vertex->short_suf_link.lock();

             while (short_suf) {
                 for (auto v : short_suf->index_of_pattern) {
                     if (subpattern_distance[v] <= i)
                         ++count_incomings[i - subpattern_distance[v]];
                 }
                 short_suf = short_suf->short_suf_link.lock();
             }
         }
         else if (current_vertex != trie){
             while (current_vertex != trie){
                 current_vertex = current_vertex->suf_link.lock();
                 if (current_vertex->next_vertices[text[i] - first_letter] != nullptr) break;
             }
             --i;//пройти цикл с этим символом ещё раз
         }
    }

    std::vector<uint32_t> answer;

    for (size_t i = 0; i < (text.length() > pattern.length() - 1 ? text.length() - pattern.length() + 1 : 0); ++i) {
        if (count_incomings[i] == patterns.size())
            answer.push_back(i);
    }
    return answer;
}

int main() {
    std::string text;
    std::string pattern;
    std::cin >> pattern;
    std::cin >> text;
    std::vector<uint32_t> result = find_patterns_incomings(pattern, text);
    for (auto v : result) {
        std::cout << v << ' ';
    }
    return 0;
}

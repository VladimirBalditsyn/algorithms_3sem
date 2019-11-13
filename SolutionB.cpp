/*Даны строки s и t. Постройте сжатое суффиксное дерево, которое содержит все суффиксы строки s и строки t. 
Найдите такое дерево, которое содержит минимальное количество вершин.

Формат ввода
В первой строке записана строка s (1 ≤ |s| ≤ 105), последний символ строки равен `$',
остальные символы строки — маленькие латинские буквы.

Во второй строке записана строка t (1 ≤ |t| ≤ 105), последний символ строки равен `#',
остальные символы строки — маленькие латинские буквы.

Формат вывода
Пронумеруйте вершины дерева от 0 до n-1 в порядке обхода в глубину, обходя поддеревья в порядке
лексикографической сортировки исходящих из вершины ребер. Используйте ASCII-коды символов для опре- деления их порядка.

В первой строке выведите целое число n — количество вершин дерева. В следующих n-1 строках выведите описание вершин дерева,
кроме корня, в порядке увеличения их номеров.

Описание вершины дерева v состоит из четырех целых чисел: p, w, lf, rg, где p (0 ≤ p < n, p ≠ v) — 
номер родителя текущей вершины, w (0 ≤ w ≤ 1) — номер строки для определения подстроки на ребре. 
Если w = 0, то на ребре, ведущем из p в v, написана подстрока s[lf … rg - 1] (0 ≤ lf < rg ≤ |s|). 
Если w = 1, то на ребре, ведущем из p в v, написана подстрока t[lf … rg -1] (0 ≤ lf < rg ≤ |t|).*/

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iterator>

constexpr size_t INF =  4294967295;

class suf_tree {
public:
    explicit suf_tree(const std::string& _str);

    template <class OutIterator>
    void DFS(OutIterator &out, size_t start_of_second_string);
    
private:
    struct Node;
    struct Edge;

    std::string_view str;
    std::shared_ptr<Node> root;

    std::shared_ptr<Node> active_node;
    std::shared_ptr<Edge> active_edge;
    size_t active_length;
    size_t reminder; //how much suffixes we should insert
    std::shared_ptr<Node> last_inserted_node;

    void append_letter(char letter, size_t index);
    void split_edge(char letter, size_t index);
    void insert_edge(char letter, size_t index);
    void dfs(std::shared_ptr<Node> current, std::vector<std::shared_ptr<Node>> &result, size_t &number);
    void correct_edge(size_t index);
    void create_suf_link(size_t index);
};

struct suf_tree::Node {
    Node() = default;
    explicit Node (std::shared_ptr<Node> _parent) : parent(_parent) { }
    std::map<char, std::shared_ptr<Edge>> edges;
    std::weak_ptr<Node> suf_link;
    std::weak_ptr<Node> parent;
    std::weak_ptr<Edge> edge_from_parent;
    size_t number;
};

struct suf_tree::Edge {
    Edge() = default;
    Edge(size_t index_from, std::shared_ptr<Node> node_from)
            : from(index_from)
            , to(INF) { next = std::make_shared<Node>(node_from); }
    Edge(size_t index_from, size_t index_to,
            std::shared_ptr<Node> node_from, std::shared_ptr<Node> node_to)
            : from(index_from)
            , to(index_to)
            , next(node_to) { next->parent = node_from; }
    std::shared_ptr<Node> next;
    size_t from;
    size_t to;

    size_t length() {
        return to - from + 1;
    }
};

suf_tree::suf_tree(const std::string& _str) : str(_str){
    root = std::make_shared<Node>();
    root->suf_link = root;
    active_node = root;
    active_length = 0;
    reminder = 0;

    for (size_t i = 0; i < str.length(); ++i)
        append_letter(str[i], i);
}

void suf_tree::append_letter(char letter, size_t index) {
    ++reminder;
    last_inserted_node = nullptr;
    while(reminder > 0) {
        if (active_length == 0 && active_node->edges.find(letter) != active_node->edges.end()) {
            active_edge = active_node->edges[letter];
            active_length = 1;
            create_suf_link(index);
            break;
        }
        else if (active_length > 0 && str[active_edge->from + active_length] == letter) {
            ++active_length;

            break;
        }
        else {
            --reminder;
            if (active_length > 0) {
                split_edge(letter, index);
            }
            else {
                insert_edge(letter, index);
            }
        }
    }
}

void suf_tree::insert_edge(char letter, size_t index) {
    active_node->edges.insert(std::make_pair(letter, std::make_shared<Edge>(index, active_node)));
    active_node->edges[letter]->next->edge_from_parent = active_node->edges[letter];
    create_suf_link(index);
    active_node = active_node->suf_link.lock();
}

void suf_tree::create_suf_link(size_t index) {
    if (last_inserted_node != nullptr) {
        last_inserted_node->suf_link = active_node;
    }
    correct_edge(index);
    last_inserted_node = active_node;
}

void suf_tree::correct_edge(size_t index) {
    while (active_length > 0 && active_edge->length() <= active_length) {
        active_length -= active_edge->length();
        active_node = active_edge->next;
        if (active_length > 0 &&
            active_node->edges.find(str[index - active_length]) != active_node->edges.end())

            active_edge = active_node->edges[str[index - active_length]];
        else
            break;
    }
}

void suf_tree::split_edge(char letter, size_t index) {
    std::shared_ptr<Node> buffer = active_edge->next;
    active_edge->next = std::make_shared<Node>(active_node);
    active_edge->next->edge_from_parent = active_edge;
    active_edge->next->suf_link = root;

    active_edge->next->edges.insert(std::make_pair(str[active_edge->from + active_length],
            std::make_shared<Edge>(active_edge->from + active_length,
                    active_edge->to, active_edge->next, buffer)));
    active_edge->next->edges[str[active_edge->from + active_length]]->next->edge_from_parent =
            active_edge->next->edges[str[active_edge->from + active_length]];

    active_edge->to = active_edge->from + active_length - 1;
    active_edge->next->edges.insert(std::make_pair(letter, std::make_shared<Edge>(index, active_edge->next)));
    active_edge->next->edges[letter]->next->edge_from_parent = active_edge->next->edges[letter];

    if (last_inserted_node != nullptr) {
        last_inserted_node->suf_link = active_edge->next;
    }
    last_inserted_node = active_edge->next; //there we need to correct edge after that and by special rules

    if (active_node == root) {
        --active_length;
        if (active_length > 0) {
            active_edge = active_node->edges[str[index - reminder + 1]];
            correct_edge(index);
        }
    }
    else {
        active_node = active_node->suf_link.lock();
        if (active_length > 0) {
            active_edge = active_node->edges[str[index - active_length]];
            correct_edge(index);
        }
    }
}

void suf_tree::dfs(std::shared_ptr<Node> current, std::vector<std::shared_ptr<Node>> &result, size_t &number) {
    current->number = number++;
    result.push_back(current);
    for (auto child : current->edges) {
        dfs(child.second->next, result, number);
    }
}

template <class OutIterator>
void suf_tree::DFS(OutIterator &out, size_t start_of_second_string) {
    std::vector<std::shared_ptr<Node>> result;
    size_t number = 0;
    dfs(root, result, number);
    std::string out_format;

    bool from_first = false;
    size_t from;
    size_t to;
    out_format += std::to_string(result.size());
    *out = out_format;
    ++out;
    out_format = "";
    for (size_t i = 1; i < result.size(); ++i) {
        out_format += std::to_string(result[i]->parent.lock()->number);
        out_format += ' ';

        from = result[i]->edge_from_parent.lock()->from;
        to = result[i]->edge_from_parent.lock()->to;
        from_first = from < start_of_second_string;

        out_format += std::to_string(from_first ? 0 : 1);
        out_format += ' ';

        out_format += std::to_string(from_first ? from : from - start_of_second_string);
        out_format += ' ';
        out_format += std::to_string(from_first ? (to == INF ? start_of_second_string : to + 1)
                : (to == INF ? str.length() - start_of_second_string : to - start_of_second_string + 1));

        *out = out_format;
        ++out;
        out_format = "";
    }

}

int main() {
    std::string first;
    std::string second;
    std::cin >> first;
    std::cin >> second;
    std::ostream_iterator<std::string> output_iter(std::cout, "\n");
    suf_tree tree(first + second);
    tree.DFS(output_iter, first.length());
    return 0;
}

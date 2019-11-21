/*Даны n точек в пространстве. Никакие 4 точки не лежат в одной полуплоскости.
Найдите выпуклую оболочку этих точек за O(n*log(n)).

Формат ввода
Первая строка содержит число m - количество тестов.
В последующих строках описаны сами тесты. Каждый тест начинается со строки,
содержащей n (n ≤ 1000) - число точек. Далее, в n строках даны по три числа - координаты точек.
Все координаты целые, не превосходят по модулю 500.

Формат вывода
Для каждого теста выведите следующее.
В первую строку выведите количество граней m.
Далее в последующие m строк выведите описание граней:
количество точек грани (=3) и номера точек в исходном множестве.
Точки нумеруются в том же порядке, в котором они даны во входном файле.
Точки в пределах грани должны быть отсортированы в порядке против часовой
стрелки относительно внешней нормали к грани. Первая точка – точка с минимальным номером.
Порядок граней лексикографический.*/


#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <array>
#include <set>

const double INF = 1e100; //constant for time in algorithm
const double epsilon = 0.0001;

double abs(double x) {
    return (x > 0 ? x : -x);
}

struct Vector {
    Vector() = default;
    explicit Vector(double _x, double _y, double _z) :  x(_x), y(_y), z(_z) {}
    explicit Vector(const Vector& start, const Vector& end) {
        x = end.x - start.x;
        y = end.y - start.y;
        z = end.z - start.z;
    }

    double length() const {
        return sqrt(x*x + y*y + z*z);
    }

    friend double scalar_product(const Vector& first, const Vector& second) {
        return first.x * second.x + first.y * second.y + first.z * second.z;
    }

    friend Vector proection(const Vector& base, const Vector& to_be_proected) {
        Vector result;
        double coef = scalar_product(base, to_be_proected) / base.length();
        result.x = base.x / abs(base.x) * coef;
        result.y = base.y / abs(base.y) * coef;
        result.z = base.z / abs(base.z) * coef;
        return result;
    }

    friend Vector operator+(const Vector& first, const Vector& second) {
        Vector result;
        result.x = first.x + second.x;
        result.y = first.y + second.y;
        result.z = first.z + second.z;
        return result;
    }

    friend Vector operator-(const Vector& first, const Vector& second) {
        Vector result;
        result.x = first.x - second.x;
        result.y = first.y - second.y;
        result.z = first.z - second.z;
        return result;
    }

    friend Vector operator*(double number, const Vector& vect) {
        Vector result(vect);
        result.x  *= number;
        result.y  *= number;
        result.z  *= number;
        return result;
    }

    double x;
    double y;
    double z;
};

double distance(const Vector& first, const Vector& second) {
    double result = sqrt((first.x - second.x) * (first.x - second.x)
                         + (first.y - second.y) * (first.y - second.y)
                         + (first.z - second.z) * (first.z - second.z));
    return result;
}

struct Tetrad {
    Tetrad() = default;
    explicit Tetrad(size_t _first) : first(_first)
                                   , second(0)
                                   , third(0)
                                   , fourth(0) {}
    size_t first;
    size_t second;
    size_t third;
    size_t fourth;

    void print() {
        std::cout << first << " " << second << " " << third << " " << fourth << "\n";
    }

    void organize() {
        char min = 1;
        size_t tmp = second;
        if (tmp > third) { tmp = third; min = 2; }
        if (tmp > fourth) min = 3;

        switch (min) {
            case 1:
                break;
            case 2:
                tmp = second;
                second = third;
                third = fourth;
                fourth = tmp;
                break;
            case 3:
                tmp = fourth;
                fourth = third;
                third = second;
                second = tmp;
                break;
            default:
                break;
        }
    }

    bool friend operator<(const Tetrad& first, const Tetrad& second) {
        if (first.second < second.second) return true;
        else if (first.second == second.second && first.third < second.third) return true;
        else return (first.second == second.second && first.third == second.third && first.fourth < second.fourth);
    }
};

struct Dot { //it is a vector with some additional information and function
    Vector current; //three dots define a face
    Dot* prev;
    Dot* next;
    size_t number;
    bool insert_if_possible();
    friend bool operator<(const Dot& first, const Dot& second) { return first.current.x < second.current.x; }
    void turn_around_zero(double angle);
};

void Dot::turn_around_zero(double angle) {
    double tmp_x, tmp_y, tmp_z;
    tmp_z = current.z * cos(angle) + current.y * sin(angle); //around Ox
    tmp_y = -current.z * sin(angle) + current.y * cos(angle);
    current.z = tmp_z;
    current.y = tmp_y;

    tmp_x = current.x * cos(angle) + current.z * sin(angle); //around Oy
    tmp_z = -current.x * sin(angle) + current.z * cos(angle);
    current.x = tmp_x;
    current.z = tmp_z;

    tmp_x = current.x * cos(angle) + current.y * sin(angle); //around Oz
    tmp_y = -current.x * sin(angle) + current.y * cos(angle);
    current.x = tmp_x;
    current.y = tmp_y;
}

bool Dot::insert_if_possible() { //in sequence of 2D events this function inserts *this dot in hull or delete
                                 //depends on the situation; returns true if inserted
    if (prev->next != this) { //insert
        prev->next = this;
        next->prev = this;
        return true;
    }
    else { //delete
        prev->next = next;
        next->prev = prev;
        return false;
    }
}

double sign_of_turn(Dot* p, Dot* q, Dot* r) { //Y-component of vector product [pq, pr]
    if (p == nullptr || q == nullptr || r == nullptr)
        return 1;

    return (q->current.x - p->current.x) * (r->current.y - p->current.y)
           - (r->current.x - p->current.x) * (q->current.y - p->current.y);
}

double time(Dot* p, Dot* q, Dot* r) { //return time when sign of turn changes <-> vectors pq, pr collinear
    if (p == nullptr || q == nullptr || r == nullptr)
        return INF;

    return ((q->current.x - p->current.x) * (r->current.z - p->current.z)
           - (r->current.x - p->current.x) * (q->current.z - p->current.z))
           / sign_of_turn(p, q, r);
}

//in that part we go form 3D to 2D, so Dots becomes Events
//there are two types of Events - insertion and deletion
std::vector<Dot*> _build_lower_part_of_convex_hull(std::vector<Dot>::iterator start_list_of_events
                                                 ,std::vector<Dot>::iterator end_list_of_events, size_t num) {
    Dot* u;
    Dot* v; //dots of bridge
    std::array<double, 6> times = {INF, INF, INF, INF, INF, INF};
    double cur_time;
    size_t index_of_cur_time = 0;
    double prev_time = -INF;

    if (start_list_of_events + 1 == end_list_of_events) {
        return std::vector<Dot*>();
    }

    auto middle = start_list_of_events + (num / 2); //divide
    std::vector<Dot*> left_part = _build_lower_part_of_convex_hull(start_list_of_events,
            middle, num / 2);
    std::vector<Dot*> right_part = _build_lower_part_of_convex_hull(middle,
            end_list_of_events, num - (num / 2));
    std::vector<Dot*> result;

    u = &(*(middle - 1));
    v = &(*middle);
    while (true) { //find bridge
        if (sign_of_turn(u, v, v->next) < 0)
            v = v->next;
        else if (sign_of_turn(u->prev, u, v) < 0)
            u = u->prev;
        else
            break;
    }

    size_t i = 0;
    size_t j = 0; //counters
    while (true) {
        //count times
        if (i < left_part.size()) {
            times[0] = time(left_part[i]->prev, left_part[i], left_part[i]->next); //appearance or disappearance
        }
        if (j < right_part.size()) {
            times[1] = time(right_part[j]->prev, right_part[j], right_part[j]->next);
        }
        times[2] = time(u, u->next, v); //overbuilding of bridge
        times[3] = time(u->prev, u, v);
        times[4] = time(u, v->prev, v);
        times[5] = time(u, v, v->next);

        cur_time = INF;
        for(size_t k = 0; k < 6; ++k) {
            if (times[k] < cur_time && times[k] > prev_time) {
                cur_time = times[k];
                index_of_cur_time = k;
            }
        }
        if (cur_time >= INF) break;
        prev_time = cur_time;

        switch (index_of_cur_time) {
            case 0:
                if (left_part[i]->current.x < u->current.x) {
                    result.push_back(left_part[i]);
                }
                left_part[i]->insert_if_possible();
                ++i;
                break;
            case 1:
                if (right_part[j]->current.x > v->current.x) {
                    result.push_back(right_part[j]);
                }
                right_part[j]->insert_if_possible();
                ++j;
                break;
            case 2:
                u = u->next;
                result.push_back(u);
                break;
            case 3:
                result.push_back(u);
                u = u->prev;
                break;
            case 4:
                v = v->prev;
                result.push_back(v);
                break;
            case 5:
                result.push_back(v);
                v = v->next;
                break;
            default:
                break;
        }
    }

    u->next = v;
    v->prev = u;
    for (int32_t k = result.size() - 1; k >= 0; --k) { //restore sequence of events
        if (result[k]->current.x <= u->current.x || result[k]->current.x >= v->current.x) {
            result[k]->insert_if_possible();
            if (result[k] == v) {
                v = v->next;
            }
            else if (result[k] == u) {
                u = u->prev;
            }
        }
        else {
            result[k]->prev = u;
            result[k]->next = v;
            u->next = result[k];
            v->prev = result[k];
            if (result[k]->current.x > (*(middle-1)).current.x) {
                v = result[k];
            } else {
                u = result[k];
            }
        }
    }
    return result;
}

void write_answer(const std::vector<Dot*>& from, std::set<Tetrad>& to, bool is_lower) {
    Tetrad tmp(3);
    bool flag;
    for (auto& e : from) {
        tmp.second = e->prev->number;
        tmp.third = e->number;
        tmp.fourth = e->next->number;
        flag = e->insert_if_possible();
        if (is_lower) {
            flag = !flag;
        }
        if (flag) { // to get a counterclockwise orientation
            std::swap(tmp.second, tmp.third);
        }
        tmp.organize();
        to.insert(tmp);
    }
}

std::set<Tetrad> build_convex_hull(std::vector<Dot>&& dots) {
    std::set<Tetrad> result;
    Tetrad tmp(3);
    for (auto& d : dots) { //move dots a little to avoid perpendicular to Oxy situation
        d.turn_around_zero(epsilon);
    }
    std::sort(dots.begin(), dots.end());

    std::vector<Dot*> events = _build_lower_part_of_convex_hull(dots.begin(), dots.end(), dots.size());
    write_answer(events, result, true);

    for (auto& d : dots) {
        d.current.z *= -1; //flip dots
        d.prev = nullptr;
        d.next = nullptr;
    }
    events = _build_lower_part_of_convex_hull(dots.begin(), dots.end(), dots.size());
    write_answer(events, result, false);

    return result;
}

int main() {
    int32_t n, size;
    Dot null = {Vector(0, 0, 0), nullptr, nullptr, 0};
    std::vector<Dot> input;
    std::cin.tie();
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::cin >> size;
        input.assign(size, null);
        for (int j = 0; j < size; ++j) {
            std::cin >> input[j].current.x >> input[j].current.y >> input[j].current.z;
            input[j].number = j;
        }
        std::set<Tetrad> result = build_convex_hull(std::move(input));
        std::cout << result.size() << '\n';
        for (auto r : result) {
            r.print();
        }
    }
    return 0;
}

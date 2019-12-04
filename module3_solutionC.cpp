/*Даны два выпуклых многоугольника на плоскости. В первом n точек, во втором m.
Определите, пересекаются ли они за O(n + m). Указание. Используйте сумму Минковского.

Формат ввода
Первая строка содержит число n точек первого многоугольника.
Затем идут n строчек с координатами точек первого многоугольника по часовой стрелке
(координаты — действительные числа, double). Второй прямоугольник задается аналогично. n, m ≤ 80000.
*/


#include <iostream>
#include <cmath>
#include <vector>

double abs(double x) {
    return (x > 0 ? x : -x);
}

bool is_same_sign(double first, double second) {
    return ((first >= 0 && second >= 0) || (first <= 0 && second <= 0));
}

struct Vector {
    Vector() = default;
    explicit Vector(double _x, double _y) :  x(_x), y(_y) {}
    explicit Vector(const Vector& start, const Vector& end) {
        x = end.x - start.x;
        y = end.y - start.y;
    }

    double length() const {
        return sqrt(x*x + y*y);
    }

    friend double scalar_product(const Vector& first, const Vector& second) {
        return first.x * second.x + first.y * second.y;
    }

    friend double vector_product(const Vector& first, const Vector& second) {
        return first.x * second.y - second.x * first.y;
    }

    friend Vector proection(const Vector& base, const Vector& to_be_proected) {
        Vector result;
        double coef = scalar_product(base, to_be_proected) / base.length();
        result.x = base.x / abs(base.x) * coef;
        result.y = base.y / abs(base.y) * coef;
        return result;
    }

    friend Vector operator+(const Vector& first, const Vector& second) {
        Vector result;
        result.x = first.x + second.x;
        result.y = first.y + second.y;
        return result;
    }

    friend Vector operator-(const Vector& first, const Vector& second) {
        Vector result;
        result.x = first.x - second.x;
        result.y = first.y - second.y;
        return result;
    }

    friend Vector operator*(double number, const Vector& vect) {
        Vector result(vect);
        result.x  *= number;
        result.y  *= number;
        return result;
    }

    double x;
    double y;
};

double distance(const Vector& first, const Vector& second) {
    double result = sqrt((first.x - second.x) * (first.x - second.x)
                         + (first.y - second.y) * (first.y - second.y));
    return result;
}

bool is_less_polar_angle(const Vector& first, const Vector& second) {
    double vect_prod = vector_product(first, second);
    if (vect_prod != 0) {
        return vect_prod > 0;
    }
    else {
        return scalar_product(first, second) > 0;
    }
}


class Polygon {
public:
    Polygon() = default;

    void add_to_polygon(const Vector& dot) {
        vertices.push_back(dot);
    }

    Vector& operator[](std::size_t index) {
        return vertices[index];
    }

    const Vector& operator[](std::size_t index) const {
        return vertices[index];
    }

    std::size_t count() const{
        return vertices.size();
    }

    bool is_inner(const Vector& dot);

    void orient();
private:
    std::vector<Vector> vertices;
};

bool Polygon::is_inner(const Vector &dot) {
    double first_prod = vector_product(vertices[vertices.size() - 1] - dot,
            vertices[0] - vertices[vertices.size() - 1]);

    for (int i = 1; i < vertices.size(); ++i) {
        if (!is_same_sign(first_prod, vector_product(vertices[i - 1] - dot, vertices[i] - vertices[i - 1])))
            return false;
    }
    return true;
}

void Polygon::orient() {
    size_t index_of_min = 0;
    for (size_t i = 1; i < vertices.size(); ++i) {
        if (vertices[index_of_min].y > vertices[i].y ||
            (vertices[index_of_min].y == vertices[i].y && vertices[index_of_min].x > vertices[i].x)) {

            index_of_min = i;
        }
    }
    auto iter = vertices.begin() + index_of_min;
    std::vector<Vector> new_vertices(iter, vertices.end());
    new_vertices.insert(new_vertices.end(), vertices.begin(), iter);
    vertices[0] = new_vertices[0];
    for (size_t i = 1; i < vertices.size(); ++i) {
        vertices[i] = new_vertices[vertices.size() - i];
    }
}

Polygon create_by_Minkowski_sum(const Polygon& first, const Polygon& second) {
    Polygon result;
    int i = 0;
    int j = 0;

    while (i < first.count() - 1 && j < second.count() - 1) {
        result.add_to_polygon(first[i] + second[j]);
        if (is_less_polar_angle(first[i + 1] - first[i], second[j + 1] - second[j]))
            ++i;
        else
            ++j;
    }
    if (i == first.count() - 1) {
        for (; j < second.count(); ++j) {
            result.add_to_polygon(first[i] + second[j]);
            if (i != 0 && is_less_polar_angle(first[0] - first[i], second[j + 1] - second[j])) {
                i = 0;
                --j;
            }
        }
    }
    else {
        for (; i < first.count(); ++i) {
            result.add_to_polygon(first[i] + second[j]);
            if (j != 0 && is_less_polar_angle(second[0] - second[j], first[i + 1] - first[i])) {
                j = 0;
                --i;
            }
        }
    }
    return result;
}

bool is_intersection(Polygon first, Polygon second) {
    for (int i = 0; i < second.count(); ++i){
        second[i] = (-1) * second[i];
    }
    first.orient();
    second.orient();
    Polygon mink_sum = create_by_Minkowski_sum(first, second);
    return mink_sum.is_inner(Vector(0, 0));
}

int main() {
    int n;
    double d1, d2;
    Polygon first;
    Polygon second;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::cin >> d1 >> d2;
        first.add_to_polygon(Vector(d1, d2));
    }
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::cin >> d1 >> d2;
        second.add_to_polygon(Vector(d1, d2));
    }
    std::string result = is_intersection(first, second) ? "YES" : "NO";
    std::cout << result;

    return 0;
}

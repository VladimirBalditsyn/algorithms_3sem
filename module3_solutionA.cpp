/*Даны два отрезка в пространстве (x1, y1, z1) - (x2, y2, z2)
и (x3, y3, z3) - (x4, y4, z4). Найдите расстояние между отрезками.*/

#include <iostream>
#include <cmath>

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

struct Segment {
    explicit Segment(const Vector& _start, const Vector& _end) : start(_start), direction(_end - _start), end(_end) {}
    double length() { return direction.length(); }
    std::pair<Vector, Vector> split_on_three_parts() const;

    Vector start;
    Vector direction;
    Vector end;
};

std::pair<Vector, Vector> Segment::split_on_three_parts() const {
    Vector first = start + ((1./3) * direction);
    Vector second = start + ((2./3) * direction);
    return std::make_pair(first, second);
}

//class Set_of_Objects must have function split_on_three_parts and constructor
//form two Objects
template<class Object, class Set_of_Objects>
std::pair<double, Object> ternary_search(const Object& dot, const Set_of_Objects& segment,
        double (*distance)(const Object&, const Object&), double epsilon) {

    std::pair<Object, Object> dots = segment.split_on_three_parts();
    Set_of_Objects cur_seg(segment);
    double  result = 0;
    while (cur_seg.length() > epsilon) {
        double first_dist = distance(dot, dots.first);
        double second_dist = distance(dot, dots.second);

        if (first_dist > second_dist) {
            cur_seg = Set_of_Objects(dots.first, cur_seg.end);
        }
        else {
            cur_seg = Set_of_Objects(cur_seg.start, dots.second);
        }
        dots = cur_seg.split_on_three_parts();
        result = 0.5 * (first_dist + second_dist);
    }
    return std::make_pair(result, 0.5 * (dots.first + dots.second));
}

double find_distance_between_segments(const Segment& first, const Segment& second) {
    const double epsilon = 0.00000001;
    Segment _first(first);
    double result;
    std::pair<Vector, Vector> f_dots, s_dots; //dots which will be returned by split in ternary search
    //prefix f_ means that Vectors are from segment first, s_ -- from second
    f_dots = first.split_on_three_parts();
    s_dots = second.split_on_three_parts();
    do {
        s_dots.first = ternary_search<Vector, Segment>(f_dots.first, second, distance, epsilon).second;
        s_dots.second = ternary_search<Vector, Segment>(f_dots.second, second, distance, epsilon).second;
        double s_first_dist = distance(f_dots.first, s_dots.first);
        double s_second_dist = distance(f_dots.second, s_dots.second);

        if (s_first_dist > s_second_dist) {
            _first = Segment(f_dots.first, _first.start + _first.direction);
        }
        else {
            _first = Segment(_first.start, f_dots.second);
        }
        f_dots = _first.split_on_three_parts();
    } while (_first.length() > epsilon);

    result = distance(0.5 * (f_dots.second + f_dots.first), 0.5 * (s_dots.second + s_dots.first));
    return result;
}

int main() {
    double x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
    std::cin >> x1 >> y1 >> z1;
    std::cin >> x2 >> y2 >> z2;
    std::cin >> x3 >> y3 >> z3;
    std::cin >> x4 >> y4 >> z4;
    Segment first(Vector(x1, y1, z1), Vector(x2, y2, z2));
    Segment second(Vector(x3, y3, z3), Vector(x4, y4, z4));
    double result = find_distance_between_segments(first, second);
    printf("%.10f", result);
    return 0;
}

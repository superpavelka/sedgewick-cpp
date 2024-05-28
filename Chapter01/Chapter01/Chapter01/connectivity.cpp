// connectivity problem

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <random>
#include <utility>
#include <numeric>
#include <Windows.h>

class Connectivity 
{
protected:
    std::vector<int> id_;
    int n_ = 0;
    int accesses_ = 0;
    int prev_accesses_ = 0;

public:
    explicit Connectivity(int n = 10) { reset(n); }

    void run(const std::list<std::pair<int, int>>& pairs) 
    {
        for (const auto& p : pairs)
            add(p.first, p.second);

        std::cout << "total accesses: " << accesses_ << "\n\n";
    }

    void randomInput(const std::vector<int> size) 
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        // пример.1.22
        // генерация случайных пар чисел 0..N-1
        // добавляем пары до тех пор, пока не произойдет объединение N-1
        // выводим количество ребер в графе
        for (int n : size) 
        {
            std::cout << "N = " << n << ' ';
            std::uniform_int_distribution<int> dist(0, n - 1);
            reset(n);
            int unions = 0;
            int edges = 0;
            do 
            {
                ++edges;
                if (!add_impl(dist(gen), dist(gen))) 
                    ++unions;
            } while (unions < n - 1);
            std::cout << "done\n";
            std::cout << "edges = " << edges << "\n\n";
        }
    }

protected:
    virtual void reset(int n) 
    {
        n_ = n;
        id_.resize(n_);
        std::iota(id_.begin(), id_.end(), 0);
    }

    void add(int p, int q) 
    {
        std::cout << "input " << p << ',' << q << '\t';
        if (add_impl(p, q))
            std::cout << "found ";
        else
            std::cout << "merge ";

        print();
    }

    virtual bool add_impl(int p, int q) = 0;

    virtual void print(int i) { std::cout << id_[i] << ' '; }

    void print() 
    {
        std::cout << "id: ";
        for (int i = 0; i < n_; ++i)
            print(i);

        std::cout << '\t';

        std::cout << " accesses: " << accesses_ - prev_accesses_ << '\n';
        prev_accesses_ = accesses_;
    }

    int& at(int i) 
    {
        ++accesses_;
        return id_.at(i);
    }
};

// Программа 1.1
// Метод быстрого поиска: объекты p и q связаны, если a[p] == a[q]
// объединение: (все объекты со значением[p]) = a[q]
class QuickFind : public Connectivity 
{
public:
    bool add_impl(int p, int q) override 
    {
        // быстрый поиск
        if (at(p) == at(q)) 
            return true;
        // медленное объединение
        const int t = at(p);
        for (int i = 0; i < n_; ++i)
            if (at(i) == t) at(i) = at(q);

        return false;
    }
};

// Программа 1.2
// поиск: i -> a[i], представляет ссылку на родительский объект,
// a[i] == i - означает, что i является корневым
// если корни для i и j совпадают - они связаны
// объединение: a[i] = j, делаем i узел дочерним по отношению к j узлу
class QuickUnion : public Connectivity 
{
public:
    bool add_impl(int p, int q) override 
    {
        // медленный поиск
        int i, j;
        for (i = p; i != at(i); i = at(i));
        for (j = q; j != at(j); j = at(j));

        if (i == j) 
            return true;

        // быстрое объединение
        at(i) = j;

        return false;
    }
};

// Программа 1.3
// поиск: i -> a[i], представляет ссылку на родительский объект,
// a[i] == i - означает, что i является корневым
// если корни для i и j совпадают - они связаны
// объединение: создание ссылки, как в программе 1.2, но соединение меньшего дерева с большим деревом
class WeightedQuickUnion : public Connectivity 
{
protected:
    std::vector<int> sz_;

    void print(int i) override { std::cout << id_[i] << '(' << sz_[i] << ") "; }

public:
    explicit WeightedQuickUnion(int n = 10) { reset(n); }

    bool add_impl(int p, int q) override 
    {
        // медленный поиск
        int i, j;
        for (i = p; i != at(i); i = at(i));
        for (j = q; j != at(j); j = at(j));
        if (i == j) 
            return true;

        // проверка размера дерева и объединение
        if (sz_[i] < sz_[j]) 
        {
            at(i) = j;
            sz_[j] += sz_[i];
        }
        else 
        {
            at(j) = i;
            sz_[i] += sz_[j];
        }

        return false;
    };

protected:
    void reset(int n) override 
    {
        Connectivity::reset(n);
        sz_.resize(n_);
        std::fill(sz_.begin(), sz_.end(), 1);
    }
};

// программа 1.4
// поиск: i -> a[i], представляет ссылку на родительский объект,
// a[i] == i - означает, что i является корнем, также создает ссылки
// от i к текущему прародителю (a[i] - родительский, a[a[i]] - прародительский)
// если корни для i и j совпадают - они связаны
// объединение: создание ссылки, как в программе 1.2, но соединение меньшего дерево с большим деревом.
class WeightedQuickUnionWithPathCompressionByHalving : public Connectivity {
protected:
    std::vector<int> sz_;

    void print(int i) override { std::cout << id_[i] << '(' << sz_[i] << ") "; }

public:
    explicit WeightedQuickUnionWithPathCompressionByHalving(int n = 10) 
    {
        reset(n);
    }

    bool add_impl(int p, int q) override {
        // медленный поиск
        int i, j;
        for (i = p; i != at(i); i = at(i)) at(i) = at(at(i));
        for (j = q; j != at(j); j = at(j)) at(j) = at(at(j));
        if (i == j) 
            return true;

        // проверка размера дерева и объединение
        if (sz_[i] < sz_[j]) 
        {
            at(i) = j;
            sz_[j] += sz_[i];
        }
        else {
            at(j) = i;
            sz_[i] += sz_[j];
        }

        return false;
    }

protected:
    void reset(int n) override 
    {
        Connectivity::reset(n);
        sz_.resize(n_);
        std::fill(sz_.begin(), sz_.end(), 1);
    }
};

// упр 1.16
// найти: i -> a[i], представляет ссылку на родительский объект,
// a[i] == i - означает, что i является корнем, также создает ссылки
// для i на корень родительского объекта
// если корни у i и j одинаковые - они связаны
// объединение: создаем ссылку, как в программе 1.2, но соединяем меньшее дерево с большим деревом
class WeightedQuickUnionWithPathFullCompression : public Connectivity {
protected:
    std::vector<int> sz_;

    void print(int i) override { std::cout << id_[i] << '(' << sz_[i] << ") "; }

public:
    explicit WeightedQuickUnionWithPathFullCompression(int n = 10) { reset(n); }

    bool add_impl(int p, int q) override {
        // медленный поиск
        int i, j;
        for (i = p; i != at(i); i = at(i))
            ;
        for (j = q; j != at(j); j = at(j))
            ;
        if (i == j) return true;

        // проверка размера дерева и объединение с меньшим деревом
        int root = -1;
        if (sz_[i] < sz_[j]) {
            at(i) = j;
            sz_[j] += sz_[i];
            root = j;
        }
        else {
            at(j) = i;
            sz_[i] += sz_[j];
            root = i;
        }

        // сжатие пути
        i = p;
        while (i != at(i)) {
            int parent = at(i);
            at(i) = root;
            i = parent;
        }

        j = q;
        while (j != at(j)) {
            int parent = at(j);
            at(j) = root;
            j = parent;
        }

        return false;
    }

protected:
    void reset(int n) override 
    {
        Connectivity::reset(n);
        sz_.resize(n_);
        std::fill(sz_.begin(), sz_.end(), 1);
    }
};

int main() 
{
    SetConsoleOutputCP(CP_UTF8);

    using Input = std::list<std::pair<int, int>>;

    Input input1_1 = { {0, 2}, {1, 4}, {2, 5}, {3, 6}, {0, 4}, {6, 0}, {1, 3} };

    Input input1_7 = { {3, 4}, {4, 9}, {8, 0}, {2, 3}, {5, 6},
                      {5, 9}, {7, 3}, {8, 4}, {6, 1} };
    Input input1_8 = { {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9},
                      {0, 2}, {4, 6}, {0, 4}, {6, 8} };
    Input input1_9 = { {3, 4}, {4, 9}, {8, 0}, {2, 3}, {5, 6},
                      {5, 9}, {7, 3}, {8, 4}, {6, 1} };
    // упр.1.4
    {
        QuickFind f;
        std::cout << "Быстрый поиск:\n";
        f.run(input1_1);
    }

    // упр.1.5
    {
        QuickUnion u;
        std::cout << "Быстрое объединение:\n";
        u.run(input1_1);
    }

    // упр.1.6
    {
        WeightedQuickUnion w;
        std::cout << "Взвешенное быстрое объединение (рис 1.7):\n";
        w.run(input1_7);
    }
    {
        WeightedQuickUnion w;
        std::cout << "Взвешенное быстрое объединение (рис 1.8):\n";
        w.run(input1_8);
    }

    // упр.1.7
    {
        WeightedQuickUnion w;
        std::cout << "Взвешенное быстрое объединение (рис 1.1):\n";
        w.run(input1_1);
    }

    // упр.1.8
    {
        WeightedQuickUnionWithPathCompressionByHalving h;
        std::cout << "Взвешенное быстрое объединение сжатие пути делением пополам:\n";
        h.run(input1_1);
    }

    // упр.1.17
    {
        WeightedQuickUnionWithPathFullCompression c;
        std::cout << "Взвешенное быстрое объединение полное сжатие пути:\n";
        c.run(input1_9);
    }

    // упр.1.22
    {
        WeightedQuickUnionWithPathCompressionByHalving h;
        std::cout << "Взвешенное быстрое объединение сжатие пути делением пополам для случайных чисел";
        h.randomInput({ 1'000, 10'000, 100'000, 1'000'000 });
    }

    return 0;
}
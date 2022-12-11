#include "../common.h"

using namespace std;

template <class T>
class Set
{
public:
    bool member(const T &item) const;
    void insert(const T &item);
    void remove(const T &item);
    std::size_t size() const;

private:
    std::list<T> _rep;
};

template <typename T>
inline bool Set<T>::member(const T &item) const
{
    return std::find(_rep.begin(), _rep.end(), item) != _rep.end();
}

template <typename T>
inline void Set<T>::insert(const T &item)
{
    if (!member(item))
    {
        _rep.push_back(item);
    }
}

template <typename T>
inline void Set<T>::remove(const T &item)
{
    typename std::list<T>::iterator iter = std::find(_rep.begin(), _rep.end(), item);
    if (iter == _rep.end())
    {
        return;
    }
    _rep.erase(iter);
}

template <typename T>
inline std::size_t Set<T>::size() const
{
    return _rep.size();
}

int main()
{

    Set<int> s;
    for (int i = 0; i < 10; i++)
    {
        s.insert(i);
    }
    printf("size: %d\n", (int)s.size());

    s.remove(3);

    return 0;
}
#include <iterator>
#include <functional>
#include <type_traits>
#include <algorithm>

/* a generic quick sort for bidirectional iterators */

template<
    class Iterator,
    class T = typename std::iterator_traits<Iterator>::value_type,
    class Compare = typename std::less<T>,
    class = std::enable_if_t<
        std::is_base_of_v<std::bidirectional_iterator_tag,
            typename std::iterator_traits<Iterator>::iterator_category>
        >>
void
quicksort(Iterator first, Iterator last, Compare compare = Compare{})
{
    if ( first == last || std::next(first) == last ) return;

    Iterator last_exist = std::prev(last);
    const T &pivot = *last_exist;

    Iterator i = first,
             j = std::prev(last_exist);

    while ( i != j ) {
        if ( !compare(*i, pivot) && compare(*j, pivot) ) {
            /* *i >= pivot && *j < pivot */
            std::swap(*i, *j);
            ++i;
            if ( i != j ) {
                --j;
                continue;
            } else {
                break;
            }
        }

        if ( compare(*i, pivot) ) {
            /* *i < pivot */
            ++i;
            continue;
        }

        if ( !compare(*j, pivot) ) {
            /* *j >= pivot */
            if ( i != j ) {
                --j;
                continue;
            } else {
                break;
            }
        }
    }

    if ( compare(*i, pivot) ) {
        /* *i < pivot */
        ++i;
    }
    std::swap(*i, *last_exist);

    quicksort(first, i, compare);
    quicksort(std::next(i), last, compare);
}

#include <vector>
#include <list>
#include <iostream>
#include <ctime>
#include <cassert>

template<class container = std::list<int>>
void
test()
{
    std::srand(std::time(NULL));
    container v(10);
    auto randGen = [] () { return std::rand() % 10000; };
    std::generate(v.begin(), v.end(), randGen);

    quicksort(v.begin(), v.end()); //, std::greater<int>{});
    for ( auto &e : v ) {
        std::cout << e << "\n";
    }
}

int main() {
    test();
}

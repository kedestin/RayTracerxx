#include <iostream>
#include <type_traits>
/*
 *
 * Meta function that checks if class is derived from base template
 *
 *
 */
template <int... args>
class Base {
        template <class T>
        void func(const T &);
};

template <int... args>
class Derived : public Base<args...> {};

template <int... args>
class Multi_Derived : public Derived<args...> {};

std::false_type is_base_impl(...);
template <int... args>
std::true_type is_base_impl(Base<args...> *);

template <typename T>
using is_derived_from_base = decltype(is_base_impl(std::declval<T *>()));

/*
 *
 *
 *  Metafunction that evaluates the equality of two parameter packs
 *
 *
 */

// General template that takes list of integers
template <int... args>
struct tp_list;
template <>
struct tp_list<> {};

// Specialized template parameter list that peels off first element of list
// Creates Pseudo-LinkedList terminated by the empty specialization
template <int b, int... args>
struct tp_list<b, args...> {
        enum { value = b };
        typedef tp_list<args...> next;
};

//Seperates the variadic parameter pack from template, and returns a
// tp_list representing that list
template <int... args>
tp_list<args...> seperator(const Base<args...> &);

// General Metafunction Template
template <typename T, typename S>
struct func;

// Iterates through every element in the each linked list, checking for
// pairwise equality
template <int... a, int... b>
struct func<tp_list<a...>, tp_list<b...>> {
        static_assert(sizeof...(a) == sizeof...(b),
                      "Lists must have same length");
        enum { value = sizeof...(a) == sizeof...(b) &&
                       (tp_list<a...>::value == tp_list<b...>::value &&
                        func<typename tp_list<a...>::next,
                             typename tp_list<b...>::next>::value) };
};

template <typename T>
struct get_tp_list {
        typedef decltype(seperator(std::declval<T &>())) type;
};

// Base case
template <>
struct func<tp_list<>, tp_list<>> {
        enum { value = true };
};

/**
 * @brief Varidic std::is_same
 *
 */

template <class... args>
struct is_same_variadic {
        enum { value = true };
};

template <class a, class b, class... rest>
struct is_same_variadic<a, b, rest...> {
        enum { value = std::is_same<a, b>::value &&
                       is_same_variadic<b, rest...>::value };
};


/**
 * @brief Metafunction that precomputes weights from linear index formula
 *
 */
template <unsigned... i>
struct linearIndex {
        enum { value = 1 };
        static unsigned convert(const unsigned *data) {
                (void)data;
                return 0;
        }
};

template <unsigned first, unsigned... rest>
struct linearIndex<first, rest...> {
        enum { value = first * linearIndex<rest...>::value };
        typedef linearIndex<rest...> next;
        static unsigned              convert(const unsigned *data) {
                return next::value * (data[0]) + next::convert(data + 1);
        }
};

using namespace std;

int main() {
        cout << func<get_tp_list<Base<1, 2, 3>>::type,
                     get_tp_list<Derived<1, 2, 3>>::type>::value
             << endl;
        cout << is_derived_from_base<Multi_Derived<1, 2, 3>>::value << endl;
        cout << is_same_variadic<unsigned, unsigned, unsigned>::value << "\n";
        return 0;
}

/*
 *
 * explanation of decltype(seperator(declval<Base<1,2,3>&>()))
 *
 * declval<Base<1,2,3>&>   = Ficticiously create a "thing" of type Base<1,2,3>&
 *
 * declval<Base<1,2,3>&>() = Call its constructor
 *
 * decltype(seperator(...)) = Infer the type of the function call given the
 * args
 *
 * So essentially, this is simulating a function call to seperator, to generate
 * a the type tp_list<args...> where args... is the parameter list of the
 * seperator argument
 */


# Wrapper
Neat Wrapper function written in C++ for preceding and suffixing the function calls to one instance, and thus guarding ownership restrictions.


Used in practice e.g. for controlling the access to an element which owership
expires when a thread marks transfers the ownership back to the handler.

Wrapper is extended by the implementation of the `prefix` function.


```c++
#include <Wrapper.hpp>
#include <iostream>

/**
 * Custom prefix class that has access to the encapsulated data.
 * @tparam T     Hard-wired in a real use-case.
 */
template <class T>
class CustomPrefix : util::wrapper::CustomPrefix<T> {
public:
  CustomPrefix(T *value) : util::wrapper::CustomPrefix<T>(value) {}

  virtual bool call() {
    std::cout<< "PRE";
    return true;
  }
}

class X {
public:
  X() { std::cout << "X()|\n"; }
  ~X() { std::cout << "~X()|\n"; }
  int f() const { std::cout << "|f( )|"; return 1; }
  void g() const { std::cout << "|g( )|"; }
};

int main() {
  auto k = new CustomPrefix();
  util::wrapper::Wrapper<X, CustomPrefix, void(*) ()> xx (new X, *k, suffix);
  util::wrapper::Wrapper<X, CustomPrefix, void(*) ()> x2 = xx;
  X x;
  if (xx->f()) std::cout << "return value received!\n";
  xx->g();
  x2->g();
  xx = x2;
  x2->g();
  return 0;
}

```

Output:

```c++
X()|
X()|
PRE|f( )|suf
return value received!
PRE|g( )|suf
PRE|g( )|suf
PRE|g( )|suf
~X()|
~X()|
```

Output if return false:
```c++
X()|
X()|
terminate called after throwing an instance of 'NotAuthorizedException'
  what():  
```

The 


Strongly inspired by 
`Stroustrup, B. (2000). Wrapping c++ member function calls. C++ Report,
12(6).`.


# Wrapper
Neat Wrapper function written in C++ for preceding and suffixing the function calls to one instance, and thus guarding ownership restrictions.


Used in practice e.g. for controlling the access to an element which owership
expires when a thread marks transfers the ownership back to the handler.

Wrapper is extended by the implementation of the `prefix` function.


```c++
#include <Wrapper.hpp>
#include <iostream>
void suffix() { std::cout << "suf\n"; }

class X {
public:
  X() { std::cout << "X()|\n"; }
  ~X() { std::cout << "~X()|\n"; }
  int f() const { std::cout << "|f( )|"; return 1; }
  void g() const { std::cout << "|g( )|"; }
};

  int main() {
    Wrapper<X, void(*) ()> xx (new X, suffix);
    Wrapper<X, void(*) ()> x2 = xx;
    X x;
    if (xx->f()) std::cout << "return value received!\n";
    xx->g();
    x2->g();
    xx = x2;
    x2->g();
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


Strongly inspired by 
`Stroustrup, B. (2000). Wrapping c++ member function calls. C++ Report,
12(6).`.

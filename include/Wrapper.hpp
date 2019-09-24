// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-12

#ifndef _WRAPPER_HPP_
#define _WRAPPER_HPP_

#include <iostream>
#include <exception>
#include <string>
#include <sstream>

#include <exception/NotAuthorized.h>
#include <executionHandler/ExecutionHandler.hpp>
#include <SuffixHandler.hpp>




#define WRAPPER2_r(c, J) Wrapper<TYPE(c) >(c, new J<TYPE(c)>(c))
#define WRAPPER2_p(c, J) Wrapper<TYPE(*c) >(c, new J<TYPE(*c)>(c))
#define WRAPPER3_p(c, J, mm) Wrapper<typeof(*c)>(c, new J<typeof(*c)>(c), mm) 

// 
// marco for overloading  the wrapper macro function:
// Works only for range of different amounts of variables (in this case 2-3)
//
// in case 2 arguments are provided to the function, wrapper 3 is stored in _3
// and the function name is WRAPPER2.
#define OVERLOAD(_1, _2, _3, NAME, ...) NAME
#define WRAPPER(...) OVERLOAD(__VA_ARGS__, WRAPPER3_p, WRAPPER2_p)(__VA_ARGS__)

namespace wrapper {

/**
 * <brief>
 * Wraps function calls to an instance of an object by a user-defined
 * prefix and suffix function. Offers the possibility to issue a call
 * to a function when the last reference to the Wrapper is lost.
 * </brief>
 *
 * By making use of the #onDelete functionality, this wrapper can be used
 * as a smart pointer implementation for freeing the allocated memory when
 * the last Wrapper instance responsible for the #content is destroyed.
 * This is possible out of the box by providing the manageMemory flag to the
 * wrapper on its instantiation.
 *
 * The Prefix function can be used for checking access rights. If it returns
 * false, a NotAuthorized exception is thrown.
 *
 * For issuing the suffix call, #SuffixHandler is used by the Wrapper
 * implementation.
 *
 * @tparam T            The class of the instance that is wrapped by this.
 *                      All function calls on that instance through the
 *                      Wrapper are then preceded by prefix code  and
 *                      succeeded by suffix code defined in an
 *                      implementation of ExecutionHandler.
 */
template <class T> class Wrapper {

public:
  /**
   * Constructor which takes its #content by reference and thus is never
   * responsible for freeing the memory.
   *
   * @param co              #content
   */
  explicit Wrapper(T &co, ExecutionHandler<T> *pr, bool manageExHandlerDel = true)
      : content(&co), owned(new int(1)), manageMemory(false),
        manageExHandlerDel(manageExHandlerDel), prefix(pr) {}

  /**
   * Constructor which takes the content as a pointer.
   * In case this Wrapper is defined to be responsible for deleting the
   * instance, do so when the last reference is deleted.
   *
   * @param co      #content
   * @param mm      whether to manage the memory. True by default,
   *                defines whether this Wrapper has to deal with
   *                deleting the referenced #content.
   */
  explicit Wrapper(T *co, ExecutionHandler<T> *pr, bool mm = true, bool manageExHandlerDel = true)
      : content(co), owned(new int(1)), manageMemory(mm), manageExHandlerDel(manageExHandlerDel),
        prefix(pr) {}

  /**
   * Overwrite copy constructor to increase the amount of owners in case the
   * deletion of the referenced #content is required by the user.
   */
  Wrapper(const Wrapper &a)
      : content(a.content), owned(a.owned), manageMemory(a.manageMemory), 
        manageExHandlerDel(a.manageExHandlerDel), prefix(a.prefix) {
    increaseOwned();
  }

  /**
   *
   * @param a
   * @return
   */
  Wrapper &operator=(const Wrapper &a) {
    a.increaseOwned();
    decreaseOwned();
    content = a.content;
    owned = a.owned;
    manageMemory = a.manageMemory;
    manageExHandlerDel = a.manageExHandlerDel;
    prefix = a.prefix;
    return *this;
  }

  /**
   * Destructor: decrease the owner counter in case the memory is to be managed
   * by the wrapper.
   */
  ~Wrapper() { decreaseOwned(); }

  /**
   * Executes #prefix prefix and suffix
   * @return
   */
  SuffixHandler<T> operator->() {
    if (prefix->prefix())
      return SuffixHandler<T>(content, prefix);
    throw exception::NotAuthorized();
  }

public:
  ExecutionHandler<T> *executionHandler() { return prefix; }

private:
  /*
   * Increase and decrease the amount of owners in case the memory is
   * managed by the Wrapper class (:= the #owned is defined).
   */

  void increaseOwned() const {
    if (owned)
      ++*owned;
  }
  void decreaseOwned() const {
    if (owned && --*owned == 0) {
      if (manageMemory)
        delete (content);
      prefix->onDestroy();
      delete (prefix);
      delete (owned);
    }
  }

  friend ExecutionHandler<T>;

  /**
   * Element to be wrapped.
   */
  T *content;

  /**
   * Amount of owners:
   */
  int *owned;

  /**
   * Whether the wrapper is responsible to delete the instance #content
   * when the last wrapper referencing this object is lost.
   */
  bool manageMemory;

  bool manageExHandlerDel;

  /**
   * User defined prefix which is a friend.
   */
  ExecutionHandler<T> *prefix;


  };

  } // namespace wrapper

#endif //_WRAPPER_HPP_

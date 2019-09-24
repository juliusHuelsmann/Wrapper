// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-24

#ifndef _SESSION_H_
#define _SESSION_H_

#include <Wrapper.hpp>

#include <mutex>

namespace wrapper {



/**
 * Custom handler for guarding node access.
 */
template <class T> class Session : public wrapper::ExecutionHandler<T> {

public:
  Session(T *value) : wrapper::ExecutionHandler<T>(value), mutex() {}

  virtual ~Session() = default;

  /**
   * Create new session.
   * @return
   */
  static Wrapper<T> createSession(T *content, bool handleMemory = false) {
    return Wrapper<T>(content, new Session(content), handleMemory);
  }

  /**
   * Manages access rights, executed prior to any function call to the
   * object.
   *
   * @return  whether the access rights are currently belonging to the
   *          thread that is currently executing or not.
   */
  virtual bool prefix() {
    mutex.lock();
    return true;
  }

  /**
   * Manages access rights, executed prior to any function call to the
   * object.
   */
  virtual void suffix() { mutex.unlock(); }

  /**
   * Code to be executed when the last reference to the #Node_session is
   * lost.
   */
  virtual void onDestroy() {}

private:
  /**
   * Mutex on the element.
   */
  std::recursive_mutex mutex;
};



// base form member idiom

template <class T> class Owner {
protected:
  template <typename... Prs> explicit Owner(Prs &... p) noexcept : protectedContent(p...) {}
  T protectedContent;
};

template <class T> class OwningSessionBaseFromMember final : private Owner<T>, public Session<T> {
public:
  template <typename... Prs>
  OwningSessionBaseFromMember(Prs &... p) : Owner<T>(p...), Session<T>(&this->protectedContent) {}
};


// adapted base from member idiom 
template <class T> class OwningSession final : private T, public Session<T> {
public:
  template <typename... Prs> OwningSession(Prs &... p) : T(p...), Session<T>(this) {}

  template <typename... Prs> static Wrapper<T> createSession(Prs &... p) {
    auto const os = new OwningSession(p...);
    return Wrapper<T>(os, os, false, false);
  }
};

} // namespace wrapper

#endif //_SESSION_H_

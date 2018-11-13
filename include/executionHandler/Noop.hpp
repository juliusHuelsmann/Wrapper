// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-12

#ifndef _NOOP_HPP_
#define _NOOP_HPP_

#include <Wrapper.hpp>

namespace wrapper {

  /**
   * Implementation of Execution handler that is used if no function wrapping 
   * functionality is required (but for instance just memory management).
   */
  template <class T> class Noop: public ExecutionHandler<T> {

    public:

      Noop(T* t): ExecutionHandler<T>(t) { }

      virtual ~Noop() = default;

      virtual bool prefix() { return true; }
      virtual void suffix() { }
      virtual void onDestroy() {}
  };
}

#endif // _NOOP_HPP_

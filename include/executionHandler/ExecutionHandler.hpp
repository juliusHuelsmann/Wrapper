// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/13/18

#ifndef _EXECUTION_HANDLER_HPP_
#define _EXECUTION_HANDLER_HPP_

namespace wrapper {

  /**
   * Abstract class that is to be implemented with custom #prefix, #suffix and
   * #onDestroy functions for encapsulating function calls and life time of
   * a reference to a shielded object.
   *
   * @tparam T    object shielded by a #Wrapper.
   */
  template <class T> class ExecutionHandler {

    public:

      /**
       * Default constructor: save handle of content that is to be shielded for
       * access to the #suffix and #prefix functions.
       * @param value
       */
      explicit ExecutionHandler(T* _content) : content(_content) { }
      virtual ~ExecutionHandler() = default;

      /**
       * Executed prior to function call.
       *
       * @return    whether to continue to execute the function call or to stop
       *            the execution by throwing an exception.
       *
       */
      virtual bool prefix() = 0;

      /**
       * Executed after function call.
       */
      virtual void suffix() = 0;

      /**
       * Executed when the last reference to the attached wrapper of #content
       * is lost (e.g. for deleting pointer).
       */
      virtual void onDestroy() = 0;

    protected:

      /**
       * Instance shielded by the wrapper.
       */
      T* content;
  };

}

#endif //_EXECUTION_HANDLER_HPP_

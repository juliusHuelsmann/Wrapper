// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-12

#ifndef _SUFFIX_HANDLER_
#define _SUFFIX_HANDLER_

#include <iostream>
#include <exception>
#include <string>
#include <sstream>

#include <include/exception/NotAuthorized.h>
#include <include/executionHandler/ExecutionHandler.hpp>


namespace wrapper {

  template <class T> class Wrapper;


  /**
   * Used by the Wrapper.
   * Responsible for calling Suffix after function call. Can only be
   * instantiated by #Wrapper.
   *
   * @tparam T            The class of the instance that is wrapped by this.
   *                      All function calls on that instance through the
   *                      Wrapper are then preceded by prefix code  and
   *                      succeeded by suffix code defined in an
   *                      implementation of ExecutionHandler.
   */
  template<class T> class SuffixHandler {

    public:

      /**
       * Copy constructor; transfer ownership. Is never called most likely, only
       * if a compiler is used that does not optimize at all.
       */
      SuffixHandler(const SuffixHandler& a): content(a.content), own(true) {
        a.own = false;
      }

      /**
       * Evoke #suffix  exactly once per function call.
       */
      virtual ~SuffixHandler() { if (own) prefix->suffix(); }

      /**
       * Expose the shielded object for the function call.
       *
       * XXX: Provide the option to analyze call stack for finding out 
       *      which function is going to be called.
       */
      T* operator->() const {
        // auto returnAddress = get_program_counter(0);
        // std::cout << returnAddress << "\n"

        // This feature is be optional.
        // 1. get program counter
        // 2. get return address
        // 3. print stack and save return address
        // disassemble here for being able to provide the name of the
        // function that called  by storing the return register and
        // revisiting it later.
        return content;
      }

    private:

      /**
       * The actual of which all functions are to be preceded by specified code.
       */
      T* content;

      /**
       * Boolean indicating whether the current call proxy is the last call
       * proxy * that deals with the content T*.
       * Used as it might be the case (for compilers that optimize very poorly)
       * that the SuffixHandler is instantiated twice for the same object.
       */
      mutable bool own;

      /**
       * The suffix that is to be called
       */
      ExecutionHandler<T>* prefix;

      /**
       * Constructor that is only available to the Wrapper.
       * @param co      content that is shielded by the wrapper.
       * @param pr      ExecutionHandler that contains the `suffix` and
       *                `onDelete` functions.
       */
      SuffixHandler(T* co, ExecutionHandler<T>* pr):
        content(co), own(true), prefix(pr) { }

      /**
       * prevent assignment.
       */
      SuffixHandler& operator=(SuffixHandler const& ) = delete;
      SuffixHandler& operator=(SuffixHandler const &&) = delete;
      SuffixHandler& operator=(SuffixHandler &&) = delete;

      /**
       * Wrapper is a friend class and thus can instantiate the suffix handler.
       */
      template<class U> friend class Wrapper;
  };
}

#endif //_SUFFIX_HANDLER_

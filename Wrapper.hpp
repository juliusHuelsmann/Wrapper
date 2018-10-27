// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-12

#ifndef _UTILS_WRAPPER_HPP_
#define _UTILS_WRAPPER_HPP_

#include <iostream>
#include <exception>
#include <string>
#include <sstream>


/**
 * Custom exception class to be called whenever the prefix function returns
 * that the execution has to be stopped.
 */
class NotAuthorizedException: public std::exception {

public:

  NotAuthorizedException(std::string custom="") {
    this->custom = custom;
  }

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Exception: The current thread is authorized to use this reference anymore\n";
    ss << custom << std::endl;
    auto k = ss.str();
    return k.c_str();
  }

private:

  /**
   * Custom information on the exception.
   */
  std::string custom;

};


namespace util {
  namespace wrapper {

    template <class T, class Ex> class Wrapper;

    /**
     * Abstract class that is to be implemented with custom #prefix, #suffix and
     * #onDestroy functions for encapsulating funciton calls and livetime of
     * a reference to a shielded object.
     *
     * @tparam T    object shielded by a #Wrapper.
     */
    template <class T>
    class ExecutionHandler {
    public:

      /**
       * Default constructor: save handle of content that is to be shielded for
       * access to the #suffix and #prefix funcitons.
       * @param value
       */
      ExecutionHandler(T* value) {
        this->value = value;
      }

      virtual ~ExecutionHandler() { }

      /**
       * Executed prior to function call.
       *
       * @return    whether the function call is to be executed or an exception
       *            is to be raised.
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

      /**
       * Can be invoked directly on the wrapper, referenced information does
       * not have to be freed.
       * @param information
       */
      virtual void notification(void* information) = 0;

    protected:

      /**
       * Instance shielded by the wrapper.
       */
      T* value;
    };

    /**
     * Responsible for calling Suffix after function call. Can only be
     * instantiated by #Wrapper.
     *
     * @tparam T            The class of the instance that is wrapped by this.
     *                      All function calls on that instance through the
     *                      Wrapper are then preceded by prefix code  and
     *                      succeeded by suffix code defined in an
     *                      implementation of ExecutionHandler.
     * @tparam Ex           ExecutionHandler handle (see #T)
     */
    template<class T, class Ex> class SuffixHandler {

      private:
        /**
         * The actual of which all functions are to be preceeded by specified code.
         */
        T* content;

        /**
         * Boolean indicating whether the current call proxy is the last call proxy
         * that deals with the content T*. Used as it might be the case (for
         * compilers that optimize very poorly) that the SuffixHandler is instantiated
         * twice for the same object.
         */
        mutable bool own;

        /**
         * The suffix that is to be called
         */
        Ex* prefix;

        SuffixHandler(T* co, Ex* pr) : content(co), own(true) {
          this->prefix = pr;
        }

        /**
         * prevent assignment
         */
        SuffixHandler& operator=(const SuffixHandler&);

        /**
         * Wrapper is a friend class and thus can instantiate the suffix handler.
         */
        template<class U, class S> friend class Wrapper;

      public:

        /**
         * Copy constructor; transfer ownership. Is most likely never called.
         */
        SuffixHandler(const SuffixHandler& a):
          content(a.content), own(true) {
            a.own = false;
          }

        /**
         * Evoke #suffix  exactly once per function call.
         */
        ~SuffixHandler() { if (own) prefix->suffix(); }

        /**
         * Expose the shielded object.
         */
        T* operator->() const { return content; }



    };

    /**
     * Issues a call to a user defined prefix - function for each function
     * that is to be called on its content.
     *
     * Can be used for freeing the allocated memory when the last Wrapper
     * instance responsible for the #content is destroyed.
     *
     * Creates an instance of #SuffixHandler for executing the suffix whenever a
     * function was called.
     *
     * @tparam T            The class of the instance that is wrapped by this.
     *                      All function calls on that instance through the
     *                      Wrapper are then preceded by prefix code  and
     *                      succeeded by suffix code defined in an
     *                      implementation of ExecutionHandler.
     * @tparam Ex           ExecutionHandler handle (see #T)
     */
    template <class T, class Ex>
    class Wrapper {

      public:

        /**
         * Constructor which takes its #content by reference and thus is never
         * responsible for freeing the memory.
         *
         * @param co              #content
         * @param su              #suffix
         */
        Wrapper (T& co, bool mm = false):
          content(&co), owned(new int(1)), prefix(&co), manageMemory(mm) {
        }
        Wrapper (T& co, Ex* pr, bool mm=false):
          content(&co), owned(new int(1)), manageMemory(mm) {
          prefix = pr;
        }


        /**
         * Constructor which takes the content as a pointer.
         * In case this Wrapper is defined to be responsible for freeing the pointer
         * if it is the last living Wrapper to deal with the pointer, free it if
         * the last reference is deleted.
         *
         * @param co              #content
         * @param memoryOwner     true by default, defines whether this Wrapper has
         *                        to deal with deleting the referenced #content.
         */
        Wrapper(T* co, bool mm=true): content(co),
           owned(new int(1)), prefix(&co), manageMemory(mm) {
          }
        Wrapper(T* co, Ex* pr, bool mm=true): content(co),
           owned(new int(1)), manageMemory(mm) {
          prefix = pr;
        }

        /**
         * Overwrite copy constructor to increase the amount of owners in case the
         * deletion of the referenced #content is required by the user.
         */
        Wrapper(const Wrapper& a): content(a.content), owned(a.owned),
                                   prefix(a.prefix),
                                   manageMemory(a.manageMemory) {
            increaseOwned();
          }

        /**
         *
         * @param a
         * @return
         */
        Wrapper& operator = (const Wrapper& a) {
          a.increaseOwned();
          decreaseOwned();
          content = a.content;
          owned = a.owned;
          manageMemory= a.manageMemory;
          prefix = a.prefix;
          return *this;
        }

        /**
         * Destructor: decrease the owner counter in case the memory is to be managed
         * by the wrapper.
         */
        ~Wrapper() {
          decreaseOwned();
        }


        /**
         * Executes #prefix prefix and suffix
         * @return
         */
        SuffixHandler<T,Ex> operator->() {
          if (prefix->prefix()) return SuffixHandler<T, Ex>(content, prefix);
          throw NotAuthorizedException();
        }

        void notification(void* information) {
          this->prefix.notification(information);
        }

      private:

        /**
         * Element to be wrapped.
         */
        T* content;



        /*
         * Increase and decrease the amount of owners in case the memory is managed
         * by the Wrapper class (:= the #owned is defined).
         */

        void increaseOwned() const {
          if (owned) ++*owned;
        }
        void decreaseOwned() const {
          if (owned && --*owned == 0) {
            if (manageMemory) delete(content);
            prefix->onDestroy();
            delete(prefix);
            delete(owned);
          }
        }

        /**
         * Amount of owners:
         */
        int* owned;

        /**
         * User defined prefix which is a friend.
         */
        Ex* prefix;
        friend Ex;

        bool manageMemory;

    };
  }
}

#endif //_UTILS_WRAPPER_HPP_

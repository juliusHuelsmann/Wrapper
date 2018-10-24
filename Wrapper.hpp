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

  const char* what() const throw() {
    std::stringstream ss;
    ss << "Exception: The current thread is authorized to use this reference anymore\n";
    ss << custom;
    return ss.str().c_str();
  }

private:

  /**
   * Custom information on the exception.
   */
  std::string custom;

};


namespace util {
  namespace wrapper {

    template <class T, class Pre, class Suf> class Wrapper;

    template <class T>
    class Prefix{
    public:

      Prefix(T* value) {
        this->value = value;
      }

      virtual bool call() = 0;

    protected:
      T* value;
    };

    /**
     * Class used for calling user defined suffix.
     * Is instantiated only by #Wrapper.
     *
     * @tparam T            The class of the instance that is wrapped by this.
     *                      All function calls on that instance through the wrapper
     *                      are then preceded by code executed in #Pref and
     *                      succeeded by the code defined in #Suf
     * @tparam Suf          Suffix handle (see #T
     */
    template<class T, class Suf> class SuffixHandler {

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
        Suf suffix;

        SuffixHandler(T* co, Suf su) : content(co), own(true), suffix(su) { }

        /**
         * prevent assignment
         */
        SuffixHandler& operator=(const SuffixHandler&);

        /**
         * Wrapper is a friend class and thus can instantiate the suffix handler.
         */
        template<class U, class S, class A> friend class Wrapper;

      public:

        /**
         * Copy constructor; transfer ownership. Is most likely never called.
         */
        SuffixHandler(const SuffixHandler& a):
          content(a.content), own(true), suffix(a.suffix) {
            a.own = false;
          }

        /**
         * Evoke #suffix  exactly once per function call.
         */
        ~SuffixHandler() { if (own) suffix(); }

        /**
         * Expose the shielded object.
         */
        T* operator->() const { return content; }



    };

    /**
     * Issues a call to a user defined prefix - function for each function
     * that is to be called on its content.
     *
     * Can be used for freeing the allocated memory when the last Wrapper instance
     * responsible for the #content is destroyed.
     *
     * Creates an instance of #SuffixHandler for executing the suffix whenever a
     * function was called. Is the class that is able to do so.
     *
     * @tparam T            The class of the instance that is wrapped by this.
     *                      All function calls on that instance through the wrapper
     *                      are then preceded by code executed in #Pref and
     *                      succeeded by the code defined in #Suf
     * @tparam Suf          Suffix handle (see #T)
     */
    template <class T, class Pre, class Suf>
    class Wrapper {

      public:

        /**
         * Constructor which takes its #content by reference and thus is never
         * responsible for freeing the memory.
         *
         * @param co              #content
         * @param su              #suffix
         */
        Wrapper (T& co, Suf su):
          content(&co), owned(0), prefix(&co), suffix(su) {
        }

        /**
         * Constructor which takes the content as a pointer.
         * In case this Wrapper is defined to be responsible for freeing the pointer
         * if it is the last living Wrapper to deal with the pointer, free it if
         * the last reference is deleted.
         *
         * @param co              #content
         * @param su              #suffix
         * @param memoryOwner     true by default, defines whether this Wrapper has
         *                        to deal with deleting the referenced #content.
         */
        Wrapper(T* co, Suf su, bool memoryOwner=true): content(co),
           owned(memoryOwner ? new int(1) : 0), prefix(co), suffix(su) {

          }

        /**
         * Overwrite copy constructor to increase the amount of owners in case the
         * deletion of the referenced #content is required by the user.
         */
        Wrapper(const Wrapper& a): content(a.content),
           owned(a.owned), prefix(a.prefix), suffix(a.suffix) {
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
          prefix = a.prefix;
          suffix = a.suffix;
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
         * Executes #prefix and #suffix
         * @return
         */
        SuffixHandler<T,Suf> operator->() {
          if (prefix.call()) return SuffixHandler<T, Suf>(content, suffix);
          throw NotAuthorizedException();
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
            delete(content); delete(owned);
          }
        }

        /**
         * Amount of owners if memory is managed by this, otherwise nullptr.
         */
        int* owned;

        /**
         *  Suffix handle, called whenever the operator.
         */
        Suf suffix;

        /**
         * User defined prefix which is a friend.
         */
        Pre prefix;
        friend Pre;

    };
  }
}

#endif //_UTILS_WRAPPER_HPP_

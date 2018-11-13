// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-12

#ifndef _PROFILER_HPP_
#define _PROFILER_HPP_

#include <Wrapper.hpp>

#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <string>

namespace wrapper {

  /**
   * Implementation of the wrapper functionality for profiling the time
   * consumption of public class member functions / methods of template class.
   *
   * The recorded timestamps are emitted to the #handleDuration function that
   * is to be implemented by a specialisation class of #Profiler.
   */
  template <class T> class Profiler: public ExecutionHandler<T> {

    public:

      /**
       *
       * @param t               shielded type
       * @param customPrefix    custom prefix to be emitted alongside the
       *                        recorded time consumption to the #handleDuration
       *                        function implementation.
       */
      Profiler(T* t, std::string customPrefix) : ExecutionHandler<T>(t),
      customPrefix(customPrefix),
      t1() {
        t1 = std::chrono::steady_clock::time_point();
      }

      virtual ~Profiler() = default;

      /**
       * Record the time at the beginning of the computation.
       */
      virtual bool prefix() {
        t1 = std::chrono::steady_clock::now();
        return true;
      }

      /**
       * Record the timestamp after the execution and measure the duration.
       *
       * Analyze the call stack for finding out which function call currently is
       * profiled.
       */
      virtual void suffix() {

        // XXX: When the stack analysis is enabled use it here.
        /*
         
        void *array[10];
        size_t size;

        // get void*'s for all entries on the stack
        size = backtrace(array, 10);

        // print out all the frames to stderr
        backtrace_symbols_fd(array, size, STDERR_FILENO);

        */


        auto t2 = std::chrono::steady_clock::now();
        long d = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        std::string functionCall = "N/A";//XXX:
        handleDuration(d, functionCall, this->customPrefix);
      }

      /**
       * Purely virtual function that is receiving the duration of the last
       * function call and the name of the function that was executed.
       *
       * @param duration        the duration in ms.
       * @param functionCall    the function's name
       * @param customPrefix    the custom prefix specified when instantiating
       *                        the profiler.
       */
      virtual void handleDuration(long duration,
          std::string functionCall,
          std::string customPrefix) = 0;

      /*
       * Functionality of the wrapper that is not required and thus empty.
       */

      virtual void onDestroy() {}
      virtual void notification(void* ) {}

    protected:

      /**
       * Custom prefix that can be set in case there are multiple profiler
       * running on different objects.
       *
       * Can be used by the implementation of the profiler.
       */
      std:: string customPrefix;

    private:

      /**
       * Temporary variable: contains the measurement of the start function.
       */
      std::chrono::steady_clock::time_point t1;
  };
}

#endif // _PROFILER_HPP_

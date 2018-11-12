// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-12

#ifndef _PROFILER_HPP_
#define _PROFILER_HPP_


#include <Wrapper.hpp>

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

namespace wrapper {

  /**
   * Custom handler for guarding node access.
   */
  template <class T>
    class Profiler: public ExecutionHandler<T> {

    public:


      Profiler(T* t, std::string customPrefix) : ExecutionHandler<T>(t) {
        this->customPrefix = customPrefix;
      }

      virtual ~Profiler() {

      }
      
      /**
       * Simply measure the timestamp from starting the execution.
       */
      virtual bool prefix() {
        t1 = std::chrono::steady_clock::now();
        return true;
      }

      /**
       * Measure the timestamp after the execution.
       *
       * Analyze the call stack for finding out which function call currently is
       * profiled.
       */
      virtual void suffix() {

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
       * function call and the name of the funtion that was executed.
       *
       * @param duration
       */
      virtual void handleDuration(long duration,
                                  std::string functionCall,
                                  std::string customPrefix) = 0;

      /*
       * Functionality of the wrapper that is not required and thus empty.
       */
      virtual void onDestroy() {}
      virtual void notification(void* ) {}

    private:

      /**
       * Temporary variable: contains the measurement of the start function.
       * .
       */
      std::chrono::steady_clock::time_point t1;

    protected:
      /**
       * Custom prefix that can be set in case there are multiple profiler
       * running on diefferen tobjects.
       *
       * Can be used by the implementation of the profiler.
       */
      std:: string customPrefix;

    };
}

#endif



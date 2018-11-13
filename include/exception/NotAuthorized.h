// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/13/18

#ifndef _NOT_AUTHORIZED_EXCEPTION_H_
#define _NOT_AUTHORIZED_EXCEPTION_H_

#include <exception>
#include <string>

namespace wrapper {
  namespace exception {

    /**
     * Custom exception class to be called whenever the prefix function returns
     * that the execution has to be stopped.
     */
    class NotAuthorized: public std::exception {

      public:

        /**
         * Compute #msg by appending a default text to the custom error message.
         * @param custom        custom error message appended to the exception
         *                      string that is emitted whenever access rights
         *                      are violated.
         */
        explicit NotAuthorized(const std::string& custom="");
        virtual const char* what() const throw();

      private:

        /**
         * Message printed when the exception is thrown.
         */
        std::string msg;

    };

  }
}

#endif //_NOT_AUTHORIZED_EXCEPTION_H_

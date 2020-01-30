// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/13/18

#ifndef NOT_AUTHORIZED_EXCEPTION_H
#define NOT_AUTHORIZED_EXCEPTION_H

#include <exception>
#include <string>

namespace wrapper::exception {

/**
 * Custom exception class to be called whenever the prefix function returns
 * that the execution has to be stopped.
 */
struct NotAuthorized : public std::exception {
  /**
   * Compute #msg by appending a default text to the custom error message.
   * @param custom        custom error message appended to the exception
   *                      string that is emitted whenever access rights
   *                      are violated.
   */
  explicit NotAuthorized(const std::string &custom = "") noexcept;

  char const *what() const throw() final;

private:
  /**
   * Message printed when the exception is thrown.
   */
  std::string const msg;
};

} // namespace wrapper::exception

#endif //NOT_AUTHORIZED_EXCEPTION_H

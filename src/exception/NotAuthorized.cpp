// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-11-13

#include <exception/NotAuthorized.h>


wrapper::exception::NotAuthorized::NotAuthorized(const std::string& custom):
  msg("Thread usage session expired!\n" + custom + "\n") { }

const char* wrapper::exception::NotAuthorized::what() const throw() {
  return msg.c_str();
}

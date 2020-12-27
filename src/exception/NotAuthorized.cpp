// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-11-13

#include <exception/NotAuthorized.h>


wrapper::exception::NotAuthorized::NotAuthorized(const std::string& custom) noexcept :
  msg("Thread usage session expired!\n" + custom + "\n") { }

const char* wrapper::exception::NotAuthorized::what() const throw() {
  //(void) this->what();
  return msg.c_str();
}

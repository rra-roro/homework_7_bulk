#include <iostream>
#include <vector>
#include <string>
#include "publisher.h"

namespace roro_lib
{
      class command_reader : public publisher_mixin<void(std::vector<std::string>&)>
      {
            size_t size_bulk;

        public:
            command_reader(size_t size_bulk) : size_bulk(size_bulk){};
      };

      struct subscriber1
      {
            void operator()(std::vector<std::string>& i)
            {
                  std::cout << "subscriber1_op=" << i[0] << "\n";
            }

            void fn(std::vector<std::string>& i)
            {
                  std::cout << "subscriber1_fn=" << i[0] << "\n";
            }
      };

      void subscriber2(std::vector<std::string>& i)
      {
            std::cout << "subscriber2=" << i[0] << "\n";
      }

}

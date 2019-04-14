#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include "publisher.h"

namespace roro_lib
{
      class command_reader : public publisher_mixin<void(std::vector<std::string>&, std::time_t)>
      {
            size_t size_bulk;

        public:
            command_reader(size_t size_bulk) : size_bulk(size_bulk){};

            void read()
            {
                  size_t i = 0;
                  std::time_t time_first_cmd;
                  std::vector<std::string> command_list;

                  for (std::string line; getline(std::cin, line);)
                  {
                        if (i == 0)
                        {
                              time_first_cmd = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                        }

                        command_list.push_back(line);

                        if (i == size_bulk - 1)
                        {
                              notify(command_list, time_first_cmd);
                              i = 0;
                              command_list.clear();
                              continue;
                        }                        
                        i++;
                  }
            };
      };

      void output_to_console(std::vector<std::string>& command_list, std::time_t)
      {
            std::string bulk =  "bulk:";
            for (auto cmd : command_list)
            {
                  bulk += " " + cmd + ",";
            };

            bulk.back() = '\n';

            std::cout << bulk;
      }

      struct save_log_file
      {
            std::string get_filename(std::time_t time_first_cmd)
            {
                  std::stringstream sstr;
                  sstr << "./bulk" << time_first_cmd << ".log";

                  return sstr.str();
            }

            void save(std::vector<std::string>& command_list, std::time_t time_first_cmd)
            {
                  std::fstream fout(get_filename(time_first_cmd), std::fstream::out);

                  for (auto cmd : command_list)
                  { 
                        fout << cmd << "\n";
                  }
            };
      };

}

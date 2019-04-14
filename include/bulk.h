#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include <cstdlib>
#include "publisher.h"

namespace roro_lib
{
      class command_reader : public publisher_mixin<void(const std::vector<std::string>&, std::time_t)>
      {
            size_t size_bulk;

        public:
            command_reader(size_t size_bulk) : size_bulk(size_bulk){};

            std::time_t get_time()
            {
                  return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            }

            void notify_subscribers(std::vector<std::string>& command_list, std::time_t time_first_cmd)
            {
                  if (!command_list.empty())
                  {
                        notify(command_list, time_first_cmd);
                        command_list.clear();
                  }
            }

            void brackets_read()
            {
                  size_t count = 1;
                  std::time_t time_first_cmd = 0;
                  std::vector<std::string> command_list;

                  for (std::string line; count != 0;)
                  {
                        if (!getline(std::cin, line))
                              exit(EXIT_SUCCESS);

                        if (time_first_cmd == 0)                        
                              time_first_cmd = get_time();
                        

                        if (line == "{")
                        {
                              ++count;
                              continue;
                        }

                        if (line == "}")
                        {
                              --count;
                              continue;
                        }

                        command_list.push_back(line);
                  }

                  notify_subscribers(command_list, time_first_cmd);
            }

            void read()
            {
                  size_t i = 0;
                  std::time_t time_first_cmd = 0;
                  std::vector<std::string> command_list;

                  for (std::string line; getline(std::cin, line);)
                  {
                        if (i == 0)
                              time_first_cmd = get_time();

                        if (line == "{")
                        {
                              notify_subscribers(command_list, time_first_cmd);
                              brackets_read();
                              i = 0;
                              continue;
                        }

                        command_list.push_back(line);

                        if (i == size_bulk - 1)
                        {
                              notify_subscribers(command_list, time_first_cmd);
                              i = 0;
                              continue;
                        }
                        i++;
                  }

                  notify_subscribers(command_list, time_first_cmd);
            };
      };

      void output_to_console(const std::vector<std::string>& command_list, std::time_t)
      {
            std::string bulk = "bulk:";
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

            void save(const std::vector<std::string>& command_list, std::time_t time_first_cmd)
            {
                  std::fstream fout(get_filename(time_first_cmd), std::fstream::out);

                  for (auto cmd : command_list)
                  {
                        fout << cmd << "\n";
                  }
            };
      };

}

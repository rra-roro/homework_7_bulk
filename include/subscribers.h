﻿#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>

namespace roro_lib
{
      void output_to_console(const std::vector<std::string>& command_list, std::time_t)
      {
            try
            {
                  std::string bulk = "bulk:";
                  for (auto cmd : command_list)
                  {
                        bulk += " " + cmd + ",";
                  };

                  bulk.back() = '\n';

                  std::cout << bulk;
            }
            catch (...)
            {
                  std::throw_with_nested(std::runtime_error("output_to_console() failed."));
            }
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
                  try
                  {
                        std::fstream fout(get_filename(time_first_cmd), std::fstream::out);

                        for (auto cmd : command_list)
                        {
                              fout << cmd << "\n";
                        }
                  }
                  catch (...)
                  {
                        std::throw_with_nested(std::runtime_error("save_log_file::save() failed."));
                  }
            };
      };
}

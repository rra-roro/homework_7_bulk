#include "exception_list.h"
#include "subscribers.h"
#include "bulk_reader.h"
#include <iostream>
#include <exception>
#include <charconv>

#include "lib_version.h"
#include "CLParser.h"


using namespace std;
using namespace roro_lib;

void help()
{
      cout << R"(
 This demo is my bulk.

    bulk  [-version | -? | -N <size>]
       Options:  
       -version      -get version of program
       -?            -about program (this info)
       -N <size>     -count of command in the block. Should be > 1
                                
)" << endl;
}

void version_bulk()
{
      cout << "Version bulk: " << version() << endl;
}

#ifndef _TEST

int main(int argc, char* argv[])
{
      try
      {
            size_t size_bulk = 0;
            ParserCommandLine PCL;
            PCL.AddFormatOfArg("?", no_argument, '?');
            PCL.AddFormatOfArg("help", no_argument, '?');
            PCL.AddFormatOfArg("version", no_argument, 'v');
            PCL.AddFormatOfArg("N", required_argument, 'n');

            PCL.SetShowError(false);
            PCL.Parser(argc, argv);

            if (PCL.Option['?'])
            {
                  help();
                  return 0;
            }
            if (PCL.Option['v'])
            {
                  version_bulk();
                  return 0;
            }

            if (!PCL.Option['n'])
            {
                  help();
                  return 0;
            }
            else
            {
                  const size_t size_param = PCL.Option['n'].ParamOption[0].size();
                  const char* const ptr_str = PCL.Option['n'].ParamOption[0].data();
                  from_chars(ptr_str, ptr_str + size_param, size_bulk);

                  if (size_bulk == 0)
                  {
                        help();
                        return 0;
                  }
            }

            command_reader cmdr(size_bulk);

            save_log_file log;
            cmdr.subscribe(log, &save_log_file::save);
            cmdr.subscribe(output_to_console);

            cmdr.read();
      }
      catch (const exception_ptr_list& ex_list)
      {
            exception_ptr_list::print(ex_list);
            return EXIT_FAILURE;
      }
      catch (const exception& ex)
      {
            print_nested_exception(ex);
            return EXIT_FAILURE;
      }
      catch (...)
      {
            cerr << "Error: unknown exception" << endl;
            return EXIT_FAILURE;
      }

      return 0;
}

#endif

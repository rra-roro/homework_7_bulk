#include "publisher.h"
#include <iostream>
#include <exception>

#include "lib_version.h"
#include "CLParser.h"


using namespace std;
using namespace roro_lib;

void help()
{
      cout << R"(
 This demo is my bulk.

    bulk  [-version | -? | -math_oder_dimensions]
       Options:  
       -version                -get version of program
       -?                      -about program (this info)
       -math_oder_dimensions   -output dimensions at math order (like x, y). Example: column, row   
                                By Default: row, column
)" << endl;
}

void version_bulk()
{
      cout << "Version bulk: " << version() << endl;
}

class publisher : public publisher_mixin<void(int)>
{
};

struct subscriber1
{
      void operator()(int i)
      {
            std::cout << "subscriber1_op=" << i << "\n";
      }

      void fn(int i)
      {
            std::cout << "subscriber1_fn=" << i << "\n";
      }
};

void subscriber2(int i)
{
      std::cout << "subscriber2=" << i << "\n";
}

#ifndef _TEST

int main(int argc, char* argv[])
{
      try
      {
            ParserCommandLine PCL;
            PCL.AddFormatOfArg("?", no_argument, '?');
            PCL.AddFormatOfArg("help", no_argument, '?');
            PCL.AddFormatOfArg("version", no_argument, 'v');
            PCL.AddFormatOfArg("math_oder_dimensions", no_argument, 'm');

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

            publisher pub;

            subscriber1 sub;

            pub.add_subscriber(sub, &subscriber1::fn);
            pub.add_subscriber(subscriber2);
            pub.add_subscriber(sub);

            pub.notify(5);

      }
      catch (const exception& ex)
      {
            cerr << "Error: " << ex.what() << endl;
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

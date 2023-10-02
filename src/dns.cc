#include <iostream>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

int get_args(int argc, char **argv)
{
    int opt;
    while (opt = getopt(argc, argv, "rx6s:p") != -1)
    {
        switch(opt)
        {
          case 'r':
          break;

          case 'x':
          break;

          case '6':
          break;

          case 's':
          break;

          case 'p':
          break;
          
          deafult:
          break;
        }
    }
}

int main(int argc, char **argv)
{

    return 0;
}

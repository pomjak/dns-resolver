#include <iostream>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

int get_args(int argc, char **argv)
{
    int opt;
    while (opt = getopt(argc, argv, "rx6spa;"))
    {
        switch(opt)
        {
            
        }
    }
}

int main(int argc, char **argv)
{

    return 0;
}
#include "md5.hpp"
#include <cstring>
#include <iostream>

int main (int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf (stderr, "usage: %s string-to-hash\n", argv[0]);
		exit (1);
	}

	MD5 context(argv[1], strlen(argv[1]));
    std::cout << context.hexdigest() << std::endl;
	return 0;
}

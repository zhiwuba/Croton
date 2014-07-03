#include "gtest/gtest.h"
#ifdef WIN32
#include <Winsock2.h>
#endif

int main(int argc ,char** argv)
{
	int n;
#ifdef WIN32
	struct WSAData wsa;
	WSAStartup(MAKEWORD (2,2), &wsa);
#endif

	testing::InitGoogleTest(&argc,argv);
	n = RUN_ALL_TESTS();

#ifdef WIN32
	WSACleanup();
#endif
	return n;
}

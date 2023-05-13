// #define _GNU_SOURCE
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;

// ----------------------------------------------------------------------------

#define ASSERT(x)                                    \
	if (x < 0)                                       \
	{                                                \
		cerr << __FILE__ << "(" << __LINE__ << "): " \
			 << strerror(errno) << endl;             \
		exit(EXIT_FAILURE);                          \
	}

// ----------------------------------------------------------------------------

int main(int, char **)
{
	// сокет
	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	ASSERT(sock);

	sockaddr_in addr = {AF_INET, htons(49002), htonl(INADDR_LOOPBACK)};

	// подключение
	int res = ::connect(sock, (sockaddr *)&addr, sizeof(addr));
	ASSERT(res);

	// остановка мониторинга
	bool stop = false;

	// поток мониторинга подключения
	thread monitor(
		[&]
		{
		// соединение установлено
		bool conn = false;

		while (!stop)
		{
			pollfd fds[1] = { { sock, POLLRDHUP } };
			res = ::poll(fds, 1, 100);
			ASSERT(res);

			if (res == 1)
			{
				if (fds[0].revents & (POLLHUP | POLLRDHUP))
				{
					if (conn)
					{
						conn = false;
						cout << "Disconnected!" << endl;

	// прерывание обмена
	res = ::shutdown(sock, SHUT_RDWR);
	ASSERT(res);

	// очистка
	res = ::close(sock);
	ASSERT(res);
					}
					else
					{
						conn = true;
						cout << "Connected!" << endl;
					}
				}
			}

			//this_thread::sleep_for(milliseconds(100));
		} });

	cout << "Press ENTER to disconnect..." << endl;

	char c;
	cin >> noskipws >> c;

	// остановка мониторинга
	stop = true;
	monitor.join();

	return EXIT_SUCCESS;
}

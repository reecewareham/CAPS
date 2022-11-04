#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <vector>

#include "config.h"
#include "TCPServer.h"
#include "TCPClient.h"
#include "RequestParser.h"

#define DEFAULT_PORT 12345

bool terminateServer = false;

void serverThreadFunction(TCPServer* server, ReceivedSocketData && data);

int main()
{
	TCPServer server(DEFAULT_PORT);

	ReceivedSocketData receivedData;

	std::vector<std::thread> serverThreads;

	std::cout << "Starting server. Send \"exit\" (without quotes) to terminate." << std::endl;

	while (!terminateServer)
	{
		receivedData = server.accept();

		if (!terminateServer)
		{
			serverThreads.emplace_back(serverThreadFunction, &server, receivedData);
		}
	}

	for (auto& th : serverThreads)
		th.join();

	std::cout << "Server terminated." << std::endl;

	return 0;
}

void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data)
{
	unsigned int socketIndex = (unsigned int)data.ClientSocket;
	std::string request;

	do {

		server->receiveData(data, 0);
		request = data.request;

		

			PostRequest post = PostRequest::parse(request);
			if (post.valid)
			{
				std::cout << "Post request: " << post.toString() << std::endl;
				std::cout << "Post topic: " << post.getTopicId() << std::endl;
				std::cout << "Post message: " << post.getMessage() << std::endl;
				server->sendReply(data);
				
				continue;
			}

			ReadRequest read = ReadRequest::parse(request);
			if (read.valid)
			{
				std::cout << "Read request" << read.toString() << std::endl;
				std::cout << "Read topic: " << read.getTopicId() << std::endl;
				std::cout << "Read post id: " << read.getPostId() << std::endl;
				server->sendReply(data);
				
				continue;
			}

			CountRequest count = CountRequest::parse(request);
			if (count.valid)
			{
				std::cout << "Count request: " << count.toString() << std::endl;
				std::cout << "Count topic: " << count.getTopicId() << std::endl;
				server->sendReply(data);
				
				continue;
			}

			ListRequest list = ListRequest::parse(request);
			if (list.valid)
			{
				std::cout << "List request: " << list.toString() << std::endl;
				server->sendReply(data);
	
				continue;
			}

			ExitRequest exitReq = ExitRequest::parse(request);
			if (exitReq.valid)
			{
				std::cout << "Exit request: " << exitReq.toString() << std::endl;
				terminateServer = true;
				server->sendReply(data);
				continue;
			}

			std::cout << "Unknown request: " << request << std::endl;
			std::cout << std::endl;

			if (request == "") {
				terminateServer = true;
			}
		

	} while (terminateServer == false);

	if (terminateServer == true)
		{
			terminateServer = true;

			TCPClient tempClient(std::string("127.0.0.1"), DEFAULT_PORT);
			tempClient.OpenConnection();
			tempClient.CloseConnection();
			server->closeClientSocket(data);

		}
}

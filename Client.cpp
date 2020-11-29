#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;
using namespace std;

class ClientThread : public Thread {
	private:
		ByteArray data;
		string dataStr;
		Socket& socket;

	public:
		ClientThread(Socket& socket) : socket(socket){}

		~ClientThread(){}
		bool finished = false;

		virtual long ThreadMain()
		{
			socket.Open();

			while (!finished){
				std::cout << "What would you like to send?: ";
				std::cout.flush();

				// Get the users input
				std::getline(std::cin, dataStr);
				data = ByteArray(dataStr);

				// Check if the user wants to quit
				if (dataStr == "done" || dataStr == "Done"){  
					finished = true;
					cout << "Closing socket and cleaning up";
					break;
				}

				// Write data to the server
				cout<< "Writing to server: " << data.ToString() << endl;
				socket.Write(data); 

				// Read the server response
				socket.Read(data);
				
				cout << "Server returned: " << data.ToString() << endl; 
			}
			return 0;
		}
};

int main(void)
{
	// Welcome the user 
	std::cout << "SE3313 Lab 3 Client" << std::endl;

	// Create our socket
	Socket socket("127.0.0.1", 3000);
	ClientThread clientThread(socket);

	// Check if the thread is done
	while(!clientThread.finished)
	{
		sleep(1);
	}

	// Close the socket and notify the user
	socket.Close();
	cout << "The socket is now closed" << endl;

	return 0;
}

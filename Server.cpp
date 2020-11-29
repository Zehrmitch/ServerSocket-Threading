#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>

using namespace Sync;
using namespace std;

class SocketThread : public Thread {
    private:
        Socket& socket;
        ByteArray data;
    public:
        SocketThread(Socket& socket): socket(socket) {}
        ~SocketThread(){}

        Socket& GetSocket()
        {
            return socket;
        }

        virtual long ThreadMain()
        {
            while(true) {
                try {
                    // Get socket data
                    socket.Read(data);
                    if (data.ToString() != "done" && data.ToString() != "Done") {
                        // Change the data before its returned 
                        string newData = "X_" + data.ToString() + "_X";
                        data = ByteArray(newData);

                        // Write the changed string
                        socket.Write(data);
                    }
                    else {
                        cout << "User closed connection" << endl ;
                        break;
                    }
                }
                catch (...) {
                    cout << "Error Catch" << endl;
                }
            }
            return 0;
        }
};

// This thread handles the server operations
class ServerThread : public Thread {
    private:
        SocketServer& server;
        bool finished = false;
    public:
        // New vector to store threads
        vector<SocketThread*> socketConnections;
        ServerThread(SocketServer& server) : server(server) {}

        ~ServerThread()
        {
            // Cleanup
            finished = true;
            cout << "Closing all connections" << endl;

            // Loop through vector of connections and close them
            for (auto connection : socketConnections){
                try{
                    connection -> GetSocket().Close();
                    cout << "Connection closed" << endl; 
                }
                catch(...){
                    cout << "Error closing";
                }
            }
        cout << "All connections closed" << endl ; 
        }

        virtual long ThreadMain()
        {
            while(!finished) {
                Socket* newConnection = new Socket(server.Accept());
                // A reference to this pointer 
                Socket& socketReference = *newConnection;
                socketConnections.push_back(new SocketThread(socketReference));
            }
        return 1;
        }
};


int main(void)
{
    std::cout << "I am a server." << std::endl;
	std::cout << "Press Enter to quit" << std::endl;
    // Create our server
    SocketServer server(3000);    

    // Need a thread to perform server operations
    ServerThread serverThread(server);
	
    // This will wait for input to shutdown the server
    FlexWait cinWaiter(1, stdin);
    cinWaiter.Wait();
    std::cin.get();


    // Shut down and clean up the server
    cout << "The server is now shutting down" << endl;
    server.Shutdown();
    cout << "The server is shut down" << endl;

    return 0;
}

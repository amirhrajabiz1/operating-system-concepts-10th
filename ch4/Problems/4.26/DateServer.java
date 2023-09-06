import java.net.*;
import java.io.*;

public class DateServer
{
    public static void main(String[] args) {
        try {
            ServerSocket sock = new ServerSocket(6013);

            /* now listen for connection */
            while (true) {
                Socket client = sock.accept();

                // create a new handler for the client socket
                ClientHandler handler = new ClientHandler(client);

                // start a new thread with the handler
                Thread t = new Thread(handler);
                t.start();

            }
        }
        catch (IOException ioe) {
            System.err.println(ioe);
        }
    }
}

// a class that handles the communication with a client socket
class ClientHandler implements Runnable {
    private Socket client;

    // constructor that takes a client socket as an argument
    public ClientHandler(Socket client) {
        this.client = client;
    }

    // override the run method to handle the client request
    public void run() {
        try{
                PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

                /* write the Date to the socket */
                pout.println(new java.util.Date().toString());

                /* close the socket and resume */
                /* listening for connections */
                client.close();
        }
        catch (IOException ioe) {
            System.err.print(ioe);
        }
    }
}
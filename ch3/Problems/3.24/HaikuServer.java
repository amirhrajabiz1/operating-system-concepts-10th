import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class HaikuServer {
      private static final int PORT = 6017;
      private static final String QUOTE_OF_THE_DAY = "An old silent pond\nA frog jumps into the pond—\nSplash! Silence again.";
      
      public static void main(String[] args) {
      	try {
		ServerSocket serverSocket = new ServerSocket(PORT);
		System.out.println("Server listening on port " + PORT);
	
		while (true) {
	  		Socket clientSocket = serverSocket.accept();
	  		System.out.println("Client connected: " + clientSocket.getInetAddress().getHostAddress());
	  		
			OutputStream outputStream = clientSocket.getOutputStream();
			outputStream.write(QUOTE_OF_THE_DAY.getBytes());
			outputStream.close();

			System.out.println("Haiku sent to client");

			clientSocket.close();
		}
	} catch (IOException e) {
	  e.printStackTrace();
	}
     }
}

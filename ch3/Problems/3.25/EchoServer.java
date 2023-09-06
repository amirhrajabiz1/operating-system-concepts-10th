import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class EchoServer {
  public static void main(String[] args) {
    try {
      ServerSocket serverSocket = new ServerSocket(12345);
      System.out.println("Server started. Listening on port 12345...");


      while (true) {
	Socket clientSocket = serverSocket.accept();
	System.out.println("Client connected: " + clientSocket.getInetAddress());

	// Create input and output streams for client socket
	InputStream inputStream = clientSocket.getInputStream();
	OutputStream outputStream = clientSocket.getOutputStream();

	byte[] buffer = new byte[1024];
	int bytesRead;


	while ((bytesRead = inputStream.read(buffer)) != -1) {
	  // Write the received data back to the client
	  outputStream.write(buffer, 0, bytesRead);
	}

	// Close the client socket
	clientSocket.close();
	System.out.println("Client disconnected.");
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}

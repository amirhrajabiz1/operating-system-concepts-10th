import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Scanner;


public class EchoClient {
  public static void main(String[] args) {
    try {
      Socket socket = new Socket("192.168.1.158", 12345);
      System.out.println("Connected to server.");


      InputStream inputStream = socket.getInputStream();
      OutputStream outputStream = socket.getOutputStream();

      Scanner scanner = new Scanner(System.in);

      while (true) {
	System.out.print("Enter a message (or 'exit' to quit): ");
	String message = scanner.nextLine();

	if (message.equalsIgnoreCase("exit")) {
	  break;
	}

	// Send the message to the server
	outputStream.write(message.getBytes());

	// Read the echoed message from the server
	byte[] buffer = new byte[1024];
	int bytesRead = inputStream.read(buffer);

	// Print the echoed message
	String echoedMessage = new String(buffer, 0, bytesRead);
	System.out.println("server response: " + echoedMessage);
      }

      // Close the socket
      socket.close();
      System.out.println("Disconnected from server.");
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}

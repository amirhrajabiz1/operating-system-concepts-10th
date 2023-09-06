import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;


public class HaikuClient {
  private static final String SERVER_ADDRESS = "localhost";
  private static final int SERVER_PORT = 6017;


  public static void main(String[] args) {
    try {
      Socket socket = new Socket(SERVER_ADDRESS, SERVER_PORT);

      BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));

      String line;
      System.out.println("Haiku received from server:");
      while ((line = reader.readLine()) != null) {
	System.out.println(line);
      }


      socket.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}
  

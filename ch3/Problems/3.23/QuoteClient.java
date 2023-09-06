import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;


public class QuoteClient {
  private static final String SERVER_ADDRESS = "localhost";
  private static final int SERVER_PORT = 6017;


  public static void main(String[] args) {
    try {
      Socket socket = new Socket(SERVER_ADDRESS, SERVER_PORT);

      BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
      String quote = reader.readLine();

      System.out.println("Quote of the day: " + quote);


      socket.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}
  

import java.io.*;
import java.net.*;
import java.nio.file.*;
import java.util.Arrays;
import java.util.stream.Stream; 

class UDPClient {
   public static void main(String args[]) throws Exception {
      BufferedReader inFromUser = new BufferedReader(new InputStreamReader(System.in));
      DatagramSocket clientSocket = new DatagramSocket();
      InetAddress IPAddress = InetAddress.getByName(args[0]);
      int port = Integer.parseInt(args[1]);

      /*read udp header from file*/
      /*Path fileLocation = Paths.get("udpheaderclient.txt");
      byte[] data = Files.readAllBytes(fileLocation);
      /***************************/
      byte[] sendData = new byte[1024];
      byte[] receiveData = new byte[1024];

      System.out.println("Write something...");
      String sentence = inFromUser.readLine();
      sentence = sentence.trim();
      /*byte[] c = new byte[data.length + sentence.getBytes().length];
      System.arraycopy(data, 0, c, 0, data.length);
      System.arraycopy(sentence.getBytes(), 0, c, data.length, sentence.getBytes().length);

      DatagramPacket sendPacket = new DatagramPacket(c, c.length, IPAddress, port);*/
      DatagramPacket sendPacket = new DatagramPacket(sentence.getBytes(),
            sentence.getBytes().length, IPAddress, port);
      clientSocket.send(sendPacket);
      DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
      clientSocket.receive(receivePacket);
      String modifiedSentence = new String(receivePacket.getData());
      System.out.println("FROM SERVER:" + modifiedSentence);
      clientSocket.close();
   }
}

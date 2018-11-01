import java.io.*;
import java.net.*;
import java.nio.file.*;
import java.nio.charset.*;
import java.util.stream.Stream; 
import java.util.Arrays; 

class UDPServer {
   public static void main(String args[]) throws Exception {
      int port_listening = Integer.parseInt(args[0]);
      DatagramSocket serverSocket = new DatagramSocket(port_listening);
      byte[] receiveData = new byte[1024];
      byte[] sendData = new byte[1024];

      System.out.println("Server is listening on port " + port_listening);
      while(true) {
         DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
         serverSocket.receive(receivePacket);
         String sentence = new String( receivePacket.getData(), StandardCharsets.UTF_8).trim();
         System.out.println("RECEIVED: " + sentence);
         InetAddress IPAddress = receivePacket.getAddress();
         int port = receivePacket.getPort();
         


         /*read udp header from file*/
         /*Path fileLocation = Paths.get("udpheaderserver.txt");
         byte[] data = Files.readAllBytes(fileLocation);
         /***************************/
         /*byte[] c = new byte[data.length + sentence.getBytes().length];
         System.arraycopy(data, 0, c, 0, data.length);
         System.arraycopy(sentence.trim().getBytes(), 0, c, data.length, sentence.trim().getBytes().length);*/

         System.out.println("-----------------");
         System.out.println("port: " + port);
         //System.out.println(data.length);
         System.out.println(sentence.trim().length());
         System.out.println("Message size: " + (/*data.length + */sentence.getBytes().length));
         System.out.println("-----------------");

         sentence = "Received: " + sentence;
         sendData = sentence.trim().getBytes(); //c;
         DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
         

         serverSocket.send(sendPacket);
      }
   }
}

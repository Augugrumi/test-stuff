import java.io.*;
import java.net.*;
import java.nio.file.*;
import java.nio.charset.*;
import java.util.stream.Stream; 
import java.util.Arrays; 

class UDPEcho {
   public static void main(String args[]) throws Exception {
      int port_listening = Integer.parseInt(args[0]);
      DatagramSocket serverSocket = new DatagramSocket(port_listening);
      byte[] receiveData = new byte[1024];
      byte[] sendData = new byte[1024];

      System.out.println("Server is listening on port " + port_listening);
      while(true) {
         DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
         serverSocket.receive(receivePacket);
         InetAddress IPAddress = receivePacket.getAddress();
         int port = receivePacket.getPort();
         serverSocket.send(receivePacket);
      }
   }
}

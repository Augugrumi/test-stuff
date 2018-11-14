import java.io.*;
import java.net.*;
import java.nio.file.*;
import java.nio.charset.*;
import java.util.stream.Stream; 
import java.util.Arrays; 

class UDPServerCalc {
   public static volatile int tot = 0;

   public static void main(String args[]) throws Exception {
      int port_listening = Integer.parseInt(args[0]);
      DatagramSocket serverSocket = new DatagramSocket(port_listening);
      byte[] receiveData = new byte[65535];
      //volatile int tot = 0;
      System.out.println("Server is listening on port " + port_listening);

      Runtime.getRuntime().addShutdownHook(new Thread() {
         public void run() {
            System.out.println("");
         }
      });

      new Thread(new Runnable() {
         public void run() {
            int tmp;
            boolean flag = true;
            while(true) {
               tmp = tot;
               try {
                  Thread.sleep(1000);
                  if (tmp == tot && tot > 0 && flag) {
                     System.out.println("");
                     flag = false;
                  } 
                  if (tmp != tot && !flag) {
                     flag = true;
                     tot = 0;
                  }
               } catch(Exception e){}
            }
         }
      }).start();

      while(true) {
         DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
         serverSocket.receive(receivePacket);
         tot += receivePacket.getLength();
         System.out.print(tot + "\r");
      }
   }
}

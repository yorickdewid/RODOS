package rodosgui;

import java.io.IOException;
import java.net.*;

//import rodos.gui.data.GuiCommand;
//import rodos.gui.data.GuiEvent;

/**
 * Mainclass of the Java-Rodos-GUI. Handles the networking stuff and creates a
 * RodosGuiFrame for the graphics-part.
 * 
 * @author Tobias Mikschl
 */
public class RodosGui implements Runnable {
    // Incoming port for gui-commands
    public static final int udpInPort = 6021;
    // Outgoing port for gui-events
    public static final int udpOutPort = 6022;

    // Socket for listening thread
    private DatagramSocket serverSocket;

    // Frame for graphical output
    private RodosGuiFrame guiFrame;

    /**
     * UDP server thread listening for incoming packages
     */
    public class udpListener implements Runnable {
	/**
	 * Execute to run listening process. processes udp-packages in infinite
	 * loop.
	 */
	public void run() {
	    byte[] receiveData = new byte[1024];

	    while (true) {
		try {
		    DatagramPacket packet = new DatagramPacket(receiveData,
			    receiveData.length);
		    serverSocket.receive(packet);
		    GuiCommand com = new GuiCommand(packet.getData(),
			    packet.getLength());
		    guiFrame.addGuiCommand(com);
		} catch (IOException e) {
		    e.printStackTrace();
		}

	    }
	}
    }

    /**
     * Execute to start initialization.
     */
    public void run() {
	try {
	    serverSocket = new DatagramSocket(udpInPort);
	} catch (SocketException e) {
	    e.printStackTrace();
	}
	guiFrame = new RodosGuiFrame();
	guiFrame.setSize(640, 480);
	guiFrame.setVisible(true);
	new Thread(new udpListener()).start();
    }

    /**
     * Send guiEvent to Rodos via UDP-connection. static - needs no
     * initialization.
     * 
     * @param eve
     *            GuiEvent to send
     */
    public static void sendGuiEvent(GuiEvent eve) {
	try {
	    DatagramSocket sendSocket = new DatagramSocket();
	    InetAddress IPAddress = InetAddress.getByName("localhost");
	    byte[] data = eve.toByteArray();
	    DatagramPacket sendPacket = new DatagramPacket(data, data.length,
		    IPAddress, udpOutPort);
	    sendSocket.send(sendPacket);
	} catch (UnknownHostException e) {
	    e.printStackTrace();
	} catch (IOException e) {
	    e.printStackTrace();
	}

    }

    /**
     * Creates a RodosGui-class and runs it.
     * 
     * @param args
     *            not in use
     */
    public static void main(String[] args) throws Exception {
	RodosGui myGui = new RodosGui();
	myGui.run();
    }

}

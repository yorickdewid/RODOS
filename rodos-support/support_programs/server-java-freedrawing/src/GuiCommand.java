package rodosgui;

import java.util.Arrays;

// import rodos.gui.utils.*;

/**
 * Represents a GuiCommand. Allows Conversion between JavaObject and
 * UdpByteStream.
 * 
 * @author Tobias Mikschl
 */
public class GuiCommand {
    // Maximum number of Parameters
    private static final int MAX_NUM_OF_PARAMS = 64;
    // WindowNr
    private int windowNr;
    // CommandNr
    private int commandNr;
    // Parameters
    private int[] param;
    // Parameter length
    private int paramLength = 0;

    /**
     * Return parameter length.
     * 
     * @return parameter length.
     */
    public int getParamLength() {
	return paramLength;
    }

    /**
     * static class for gui command codes.
     */
    public class guiCommandCode {
	public static final int GUICMD_NOP = 0,
		GUICMD_ACTIVATE_MOUSEMOVE_EVENTS = 1, 
		GUICMD_POS_AND_SIZE = 2,
		GUICMD_CLEAR = 3, 
		GUICMD_BACKGROUND = 4, 
		GUICMD_DRAW_TEXT = 5,
		GUICMD_DRAW_LINE = 6, 
		GUICMD_DRAW_BOX = 7,
		GUICMD_DRAW_CIRCLE = 8, 
		GUICMD_FILL_BOX = 9,
		GUICMD_FILL_CIRCLE = 10, 
		GUICMD_DRAW_POLYGON = 11,
		GUICMD_FILL_POLYGON = 12, 
		GUICMD_3D_COMMANDS = 16000,
		GUICMD_USER_COMMANDS = 0x010000000,
		GUICMD_USER_COMMAND1 = 0x010000001,
		GUICMD_USER_COMMAND2 = 0x010000002,
		GUICMD_USER_COMMAND3 = 0x010000003,
		GUICMD_USER_COMMAND4 = 0x010000004;
    };

    /**
     * Constructor for GuiCommand.
     * 
     * @param data
     *            byte data array from the Udp-Packet.
     * @param length
     *            length of the data array
     */
    public GuiCommand(byte[] data, int length) {
	// length ok?
	if (length < 8 || length - 8 > MAX_NUM_OF_PARAMS * 4 || length % 4 != 0)
	    throw new RuntimeException(
		    "GuiCommand: array length out of boundaries");

	// Parameters are saved as int-array -> convert
	paramLength = (length / 4) - 2;
	int[] idata = ByteArrayHelpers.toIntArray(data, length);
	windowNr = idata[0];
	commandNr = idata[1];
	param = new int[paramLength];
	for (int i = 2; i < length / 4; i++) {
	    param[i - 2] = idata[i];
	}

    }

    /**
     * Returns command number.
     * 
     * @return command number.
     */
    public int getCommandNr() {
	return commandNr;
    }

    /**
     * Return window number.
     * 
     * @return window number.
     */
    public int getWindowNr() {
	return windowNr;
    }

    /**
     * returns parameters.
     * 
     * @return int-array of parameters
     */
    public int[] getParam() {
	return param;
    }

    /**
     * Returns single parameter at specified position.
     * 
     * @param pos
     *            position
     * @return parameter (integer)
     */
    public int getParam(int pos) {
	return param[pos];
    }

    /**
     * Converts GuiCommand back to ByteArray (for network transmission)
     * 
     * @return byte-array
     */
    public byte[] toByteArray() {
	int[] idata = new int[2 + param.length];
	idata[0] = windowNr;
	idata[1] = commandNr;
	for (int i = 0; i < param.length; i++) {
	    idata[i + 2] = param[i];
	}
	return ByteArrayHelpers.toByteArray(idata);
    }

    /**
     * String representation of the GuiCommand.
     */
    public String toString() {
	return "GuiCommand [windowNr=" + windowNr + ", commandNr=" + commandNr
		+ ", param=" + Arrays.toString(param) + ", paramLength="
		+ paramLength + "]";
    }

}

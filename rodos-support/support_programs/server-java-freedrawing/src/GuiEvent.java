package rodosgui;

// import rodos.gui.utils.ByteArrayHelpers;

/**
 * Represents a GuiEvent. Allows Conversion between JavaObject and
 * UdpByteStream.
 * 
 * @author Tobias Mikschl
 */
public class GuiEvent {
    // Window number
    private int windowNr;
    // position of the event - coordinates x,y
    private int x, y;
    // Pressed key (Button Number)
    private int pressedKey;
    // Released key (Button number)
    private int releasedKey;
    // user event
    private int userEvent;

    /**
     * Constructor for a GuiEvent.
     * 
     * @param windowNr
     *            window number.
     * @param x
     *            Coordinate X.
     * @param y
     *            Coordinate Y.
     * @param pressedKey
     *            Pressed Button.
     * @param releasedKey
     *            Released Button.
     * @param userEvent
     *            User event.
     */
    public GuiEvent(int windowNr, int x, int y, int pressedKey,
	    int releasedKey, int userEvent) {
	this.windowNr = windowNr;
	this.x = x;
	this.y = y;
	this.pressedKey = pressedKey;
	this.releasedKey = releasedKey;
	this.userEvent = userEvent;
    }

    /**
     * Convert to byte-array (for network transfer).
     * 
     * @return byte-array for the gui event.
     */
    public byte[] toByteArray() {
	int[] idata = new int[6];
	idata[0] = windowNr;
	idata[1] = x;
	idata[2] = y;
	idata[3] = pressedKey;
	idata[4] = releasedKey;
	idata[5] = userEvent;
	return ByteArrayHelpers.toByteArray(idata);
    }

    /**
     * String representation of the GuiEvent.
     */
    public String toString() {
	return "GuiEvent [windowNr=" + windowNr + ", x=" + x + ", y=" + y
		+ ", pressedKey=" + pressedKey + ", releasedKey=" + releasedKey
		+ ", userEvent=" + userEvent + "]";
    }

}

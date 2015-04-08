package rodosgui;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Class with helper-functions concerning byte-arrays.
 * 
 * @author Tobias Mikschl
 * 
 */
public class ByteArrayHelpers {
    /**
     * Converts a byte-array to an int-array. With conversion from LittleEndian
     * (network) to BigEndian (Java)
     * 
     * @param barr
     *            Byte-array to convert.
     * @param length
     *            Length of the byte array.
     * @return Converted Int-Array
     */
    public static int[] toIntArray(byte[] barr, int length) {
	int size = (length / 4);

	// Byte-buffer with enough size
	ByteBuffer bb = ByteBuffer.allocate(size * 4);
	// Fill buffer with byte-array
	bb.put(barr, 0, length);

	// byte-array oder is LittleEndian
	bb.order(ByteOrder.LITTLE_ENDIAN);

	// output int-array
	int[] result = new int[size];

	// rewind buffer
	bb.rewind();
	// take step by step integers out
	while (bb.remaining() > 0) {
	    result[bb.position() / 4] = bb.getInt();
	}

	return result;
    }

    /**
     * Converts an int-array to a byte-array (for network transfer).
     * 
     * @param iarr
     *            int-array to convert.
     * @return converted byte-array.
     */
    public static byte[] toByteArray(int[] iarr) {
	// byte array with sufficient size
	int size = iarr.length * 4;

	// Buffer used for conversion
	ByteBuffer bb = ByteBuffer.allocate(size);

	// change ByteOrder from Big to LittleEndian on insert.
	bb.order(ByteOrder.LITTLE_ENDIAN);
	for (int a : iarr) {
	    bb.putInt(a);
	}

	// result-array
	byte[] result = new byte[size];

	// rewinf buffer
	bb.rewind();

	// read single bytes from buffer
	while (bb.remaining() > 0) {
	    result[bb.position()] = bb.get();
	}
	return result;
    }

    /**
     * Helper Function to extract a char-string from an int-array. Function
     * special for network transfered, converted Int-Arrays, with the
     * text-string previously coded in LittleEndian Format.
     * 
     * @param iarr
     *            Int-array to extract from. Conversion stops on a '\0'.
     * @param offset
     *            Offset at which to start the conversion.
     * @return Java String
     */
    public static String extractByteString(int[] iarr, int offset) {
	// Maximum length of String
	int size = (iarr.length - offset) * 4;

	// Result String
	String result = "";

	// Buffer for conversion
	ByteBuffer bb = ByteBuffer.allocate(size);

	// LittleEndian byte format.
	bb.order(ByteOrder.LITTLE_ENDIAN);

	// Fill buffer
	for (int i = offset; i < iarr.length; i++) {
	    bb.putInt(iarr[i]);
	}

	// rewind buffer
	bb.rewind();

	// extract string
	while (bb.remaining() > 0) {

	    // end condition
	    if (bb.get(bb.position()) == 0)
		break;
	    // character
	    else
		result += Character.toString((char) bb.get());

	}

	return result;
    }

}

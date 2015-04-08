package rodosgui;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Polygon;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JComponent;
import javax.swing.JFrame;

//import rodos.gui.data.GuiCommand;
//import rodos.gui.data.GuiEvent;
//import rodos.gui.utils.ByteArrayHelpers;

@SuppressWarnings("serial")
/**
 * JFrame for displaying Rodos-GuiCommands.
 * @author Tobias Mikschl
 */
public class RodosGuiFrame extends JFrame {
	// PaintArea for direct drawing.
	private PaintArea pArea;

	private int lastX, lastY;

	private boolean mouseReports = false;

	/**
	 * PaintArea allows direct drawing of GuiCommands. GuiCommands are saved in
	 * a Collection, so in case of a repaint they can be redrawn.
	 */
	private static class PaintArea extends JComponent {
		// List oof GuiCommands
		private List<GuiCommand> commands = Collections
				.synchronizedList(new LinkedList<GuiCommand>());

		/**
		 * Add GuiCOmmand to PaintArea.
		 * 
		 * @param com
		 */
		public void addGuiCommand(GuiCommand com) {
			commands.add(com);
			this.repaint();
		}

		/**
		 * Remove all current GuiCommands from PaintArea.
		 */
		public void clearCommands() {
			commands.clear();
			repaint();
		}

		/**
		 * Component shall be painted. For manual repaint use repaint().
		 */
		public void paint(Graphics g) {
			// has to be thread-save
			synchronized (commands) {

				// draw each if the current GuiCommands in the Collection
				for (GuiCommand com : commands) {
					drawGuiCommand(com, g);
				}
			}
		}

		/**
		 * Draw a GuiCommand to the PaintArea.
		 * 
		 * @param com
		 *            GuiCommand to draw.
		 * @param g
		 *            Graphic-Context to draw to.
		 */
		private void drawGuiCommand(GuiCommand com, Graphics g) {
			Polygon polygon;
			switch (com.getCommandNr()) {
			case GuiCommand.guiCommandCode.GUICMD_DRAW_TEXT:
				g.setColor(new Color(com.getParam(2), com.getParam(3), com
						.getParam(4)));
				String msg = ByteArrayHelpers.extractByteString(com.getParam(),
						5);
				g.drawBytes(msg.getBytes(), 0, msg.length(), com.getParam(0),
						com.getParam(1));
				break;
			case GuiCommand.guiCommandCode.GUICMD_DRAW_BOX:
				g.setColor(new Color(com.getParam(4), com.getParam(5), com
						.getParam(6)));
				g.drawRect(com.getParam(0), com.getParam(1), com.getParam(2),
						com.getParam(3));
				break;
			case GuiCommand.guiCommandCode.GUICMD_FILL_BOX:
				g.setColor(new Color(com.getParam(4), com.getParam(5), com
						.getParam(6)));
				g.fillRect(com.getParam(0), com.getParam(1), com.getParam(2),
						com.getParam(3));
				break;
			case GuiCommand.guiCommandCode.GUICMD_DRAW_CIRCLE:
				g.setColor(new Color(com.getParam(3), com.getParam(4), com
						.getParam(5)));
				g.drawOval(com.getParam(0) + com.getParam(2), com.getParam(1)
						+ com.getParam(2), com.getParam(2) * 2,
						com.getParam(2) * 2);
				break;
			case GuiCommand.guiCommandCode.GUICMD_FILL_CIRCLE:
				g.setColor(new Color(com.getParam(3), com.getParam(4), com
						.getParam(5)));
				g.fillOval(com.getParam(0) + com.getParam(2), com.getParam(1)
						+ com.getParam(2), com.getParam(2) * 2,
						com.getParam(2) * 2);
				break;
			case GuiCommand.guiCommandCode.GUICMD_DRAW_POLYGON:
				g.setColor(new Color(com.getParam(0), com.getParam(1), com
						.getParam(2)));
				polygon = new Polygon();
				polygon.addPoint(com.getParam(5), com.getParam(6));
				polygon.addPoint(com.getParam(7), com.getParam(8));
				polygon.addPoint(com.getParam(9), com.getParam(10));
				for (int j = 0; j < com.getParam(4) - 3; j++) {
					polygon.addPoint(com.getParam((j * 2) + 11),
							com.getParam((j * 2) + 12));
				}
				g.drawPolygon(polygon);
				break;
			case GuiCommand.guiCommandCode.GUICMD_FILL_POLYGON:
				g.setColor(new Color(com.getParam(0), com.getParam(1), com
						.getParam(2)));
				polygon = new Polygon();
				polygon.addPoint(com.getParam(5), com.getParam(6));
				polygon.addPoint(com.getParam(7), com.getParam(8));
				polygon.addPoint(com.getParam(9), com.getParam(10));
				for (int j = 0; j < com.getParam(4) - 3; j++) {
					polygon.addPoint(com.getParam((j * 2) + 11),
							com.getParam((j * 2) + 12));
				}
				g.fillPolygon(polygon);
				break;
			default:
				break;
			}
		}
	}

	/**
	 * KeyListener class for keyboard-events.
	 */
	private class myKeyListener implements KeyListener {
		@Override
		/**
		 * Called on key down.
		 */
		public void keyPressed(KeyEvent arg0) {
			GuiEvent gEve = new GuiEvent(1, lastX, lastY, arg0.getKeyCode(), 0,
					0);
			RodosGui.sendGuiEvent(gEve);
		}

		@Override
		/**
		 * Called on key up.
		 */
		public void keyReleased(KeyEvent arg0) {
			GuiEvent gEve = new GuiEvent(1, lastX, lastY, 0, arg0.getKeyCode(),
					0);
			RodosGui.sendGuiEvent(gEve);
		}

		@Override
		/**
		 * Calles on key typed.
		 */
		public void keyTyped(KeyEvent arg0) {
			// not used

		}

	}

	/**
	 * MouseListener-Class for mouse actions.
	 */
	private class myMouseListener implements MouseListener, MouseMotionListener {

		@Override
		/**
		 * Called on mouse-click.
		 */
		public void mouseClicked(MouseEvent arg0) {
			if (arg0.getClickCount() == 2) {
				GuiEvent gEve = new GuiEvent(1, arg0.getX(), arg0.getY(),
						arg0.getButton(), arg0.getButton(), 0);
				RodosGui.sendGuiEvent(gEve);
			}
		}

		@Override
		/**
		 * Called on mouse-enter.
		 */
		public void mouseEntered(MouseEvent arg0) {
			// not used

		}

		@Override
		/**
		 * Called on mouse-exit.
		 */
		public void mouseExited(MouseEvent arg0) {
			// not used

		}

		@Override
		/**
		 * Called on mousebutton-down.
		 */
		public void mousePressed(MouseEvent arg0) {
			GuiEvent gEve = new GuiEvent(1, arg0.getX(), arg0.getY(),
					arg0.getButton(), 0, 0);
			RodosGui.sendGuiEvent(gEve);

		}

		@Override
		/**
		 * Called on mousebutton-up.
		 */
		public void mouseReleased(MouseEvent arg0) {
			GuiEvent gEve = new GuiEvent(1, arg0.getX(), arg0.getY(), 0,
					arg0.getButton(), 0);
			RodosGui.sendGuiEvent(gEve);

		}

		@Override
		/**
		 * Called on mouse-drag.
		 */
		public void mouseDragged(MouseEvent arg0) {
			lastX = arg0.getX();
			lastY = arg0.getY();
			if (mouseReports) {
				GuiEvent gEve = new GuiEvent(1, arg0.getX(), arg0.getY(), 0, 0,
						0);
				RodosGui.sendGuiEvent(gEve);
			}

		}

		@Override
		/**
		 * Called on mouse-move.
		 */
		public void mouseMoved(MouseEvent arg0) {
			lastX = arg0.getX();
			lastY = arg0.getY();
			if (mouseReports) {
				GuiEvent gEve = new GuiEvent(1, arg0.getX(), arg0.getY(), 0, 0,
						0);
				RodosGui.sendGuiEvent(gEve);
			}
		}

	}

	/**
	 * Constructor for new GuiFrame. Adds Paintarea and enables ListenerS.
	 */
	public RodosGuiFrame() {
		pArea = new PaintArea();
		this.setContentPane(pArea);
		myMouseListener mouseListener = new myMouseListener();
		pArea.addMouseListener(mouseListener);
		pArea.addMouseMotionListener(mouseListener);
		this.addKeyListener(new myKeyListener());
	}

	/**
	 * Add GuiCommand to GuiFrame.
	 * 
	 * @param com
	 *            GuiCommand to add.
	 */
	public void addGuiCommand(GuiCommand com) {

		// If GuiCommand is no draw-Command execute here.
		if (com.getCommandNr() == GuiCommand.guiCommandCode.GUICMD_BACKGROUND) {
			this.setBackground(new Color(com.getParam(0), com.getParam(1), com
					.getParam(2), com.getParam(3)));
		} else if (com.getCommandNr() == GuiCommand.guiCommandCode.GUICMD_CLEAR) {
			pArea.clearCommands();
		} else if (com.getCommandNr() == GuiCommand.guiCommandCode.GUICMD_POS_AND_SIZE) {
			pArea.setSize(com.getParam(2), com.getParam(3));
			pArea.setPreferredSize(pArea.getSize());
			this.setLocation(com.getParam(0), com.getParam(1));
			this.pack();
		} else if (com.getCommandNr() == GuiCommand.guiCommandCode.GUICMD_ACTIVATE_MOUSEMOVE_EVENTS) {
			mouseReports = (com.getParam(0) == 1);
		}
		// else redirect to PaintArea.
		else {
			pArea.addGuiCommand(com);
		}
	}
}

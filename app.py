import sys
import os
import codecs
import pyperclip
import config

from PyQt5 import QtWidgets
from PyQt5.QtGui import QIcon

from controller_input import ControllerThread


# Main Window
class Window(QtWidgets.QMainWindow):

    # Whether or not input is Paused
    paused = False
    # Whether or not a script is open
    reading = False
    # The name of the current script being read
    script = ""
    # The 0-indexed index of the current l ine being read
    line = 0
    # The list of lines to read from the current script
    lines = []
    # The Directory where the current script being read resides in
    script_dir = ""
    # List of other scripts to move to next
    scripts = []
    # The thread where controller input runs
    controller_thread = ControllerThread()

    def __init__(self):
        super().__init__()

        # The main widget to put inside the main window
        self.main_widget = QtWidgets.QWidget(self)
        # The widget to hold the current line number being read
        self.current_line = QtWidgets.QLineEdit("No script")
        self.current_line.returnPressed.connect(self.on_num_return)
        # The widget to hold the actual text of the line being read
        self.line_text = QtWidgets.QLineEdit("No Line")
        # The label holding the name of the script currently being read
        self.current_label = QtWidgets.QLabel("Current Script: None")
        # The button to get to the next line
        self.next = QtWidgets.QPushButton("Next")
        self.next.clicked.connect(self.next_line)
        # Invert the button on the controller for the next line
        self.next_btn_invert = QtWidgets.QCheckBox("Use South Button for next line")

        self.init_ui()

    def init_ui(self):
        # The action to open a script
        openscr = QtWidgets.QAction(QIcon('open.png'), '&Open', self)
        openscr.setShortcut('Ctrl+O')
        openscr.setStatusTip('Open Script')
        openscr.triggered.connect(self.open_script)

        # The action to save the current progress
        savescr = QtWidgets.QAction(QIcon('save.png'), "&Save", self)
        savescr.setShortcut('Ctrl+S')
        savescr.setStatusTip('Save Current Progress')
        savescr.triggered.connect(self.save)

        # Setting up the menu bar
        self.statusBar()
        menubar = self.menuBar()
        menubar.addAction(openscr)
        menubar.addAction(savescr)

        # Inner horizontal layout for button and lineedit
        h_box = QtWidgets.QHBoxLayout()
        h_box.addWidget(self.current_line, 1)
        h_box.addWidget(self.next, 2)

        # Outer vertical layout for the rest of the elements
        v_box = QtWidgets.QVBoxLayout()
        v_box.addLayout(h_box)
        v_box.addWidget(self.line_text)
        v_box.addWidget(self.current_label)
        v_box.addWidget(self.next_btn_invert)

        # Starting to listen for controller connections
        self.controller_thread.controller_input.connect(self.on_input)
        self.controller_thread.start()

        # Set up window properties
        self.setWindowTitle(config.window_title)
        self.main_widget.setLayout(v_box)
        self.setCentralWidget(self.main_widget)

        # Attempt to load from save
        self.get_save()

        self.show()

    # Lets the user select a Script to open
    def open_script(self):
        path = QtWidgets.QFileDialog.getOpenFileName()[0]
        self.script_dir, self.script = os.path.split(path)  # set fields
        self.set_script(path)  # open the script

    # Sets the current script being read
    def set_script(self, path):
        self.get_scripts(self.script_dir)  # Re-populates known scripts to move to
        self.parse(path)  # open the file and read it
        self.reading = True  # There is a script open
        self.current_line.setText(str(self.line))  # update UI elements
        self.line_text.setText(self.lines[self.line])
        self.save()  # save to the save file

    # Moves user to the next line
    def next_line(self):
        if not self.paused and self.reading:
            self.line += 1  # Try to move to the next line
            if self.line == len(self.lines):  # If it's the last line
                next_script = self.scripts.index(self.script) + 1  # Try to move to the next script
                if next_script != len(self.scripts):  # If it exists, open it
                    self.script = self.scripts[next_script]
                    self.set_script(os.path.join(self.script_dir, self.scripts[next_script]))
                else:
                    # Otherwise notify user
                    self.current_line.setText("Select New Directory")
                    self.current_label.setText("Current Script: None")
                    self.reading = False
            else:
                # Otherwise just move to said line
                self.set_line(self.line)

    # When "enter" is pressed on the line count
    def on_num_return(self):
        # Try to get a number from it and set the line
        try:
            self.set_line(int(self.current_line.text()))
        except ValueError:
            self.current_line.setText("Invalid Number")

    # Sets the current line to a new one
    def set_line(self, line_num):
        # If it's not valid number
        if line_num >= len(self.lines) or line_num < 0:
            self.current_line.setText("Invalid Number")
        else:
            # Else update fields and UI and copy the line
            self.line = line_num
            self.current_line.setText(str(self.line))
            self.line_text.setText(self.lines[self.line])
            pyperclip.copy(self.lines[self.line])

    # Handles PAUSE mode
    def pause(self):
        self.paused = not self.paused
        if self.reading:
            if not self.paused:  # Unpause
                self.current_line.setText(str(self.line))
            else:  # Pause
                self.current_line.setText("PAUSED")

    # When there is new controller input, see controller_input.py for controller signals
    def on_input(self, action):
        # Invert button if need be
        next_btn = "BTN_EAST"
        if self.next_btn_invert.isChecked():
            next_btn = "BTN_SOUTH"

        if action is next_btn or action is "UP_ARROW":
            self.next_line()
        elif action is "LEFT_ARROW":
            self.pause()

    # Opens a file and reads its lines
    def parse(self, file):
        f = codecs.open(file, "r", "utf-8")
        self.line = 0
        self.lines = f.readlines()
        f.close()
        self.current_label.setText("Current Script: " + self.script)

    # Gets the other potential scripts in the same folder
    def get_scripts(self, path):
        self.scripts = [i for i in os.listdir(path) if i.startswith(config.script_prefix)]

    # Saves the current progress to the file
    def save(self):
        save_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), config.save_file)
        with codecs.open(save_path, "w", "utf-8") as f:
            f.write(",".join((self.script, self.script_dir, str(self.line))))

    # Attempts to open the save file and read it
    def get_save(self):
        save_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), config.save_file)
        if os.path.exists(save_path) and os.path.isfile(save_path):
            f = codecs.open(save_path, "r", "utf-8")
            save = f.readline().strip().split(",")
            f.close()
            try:
                self.script = save[0]
                self.script_dir = save[1]
                self.line = save[2]
                self.set_script(os.path.join(self.script_dir, self.script))
                self.set_line(int(save[2]))
            except IndexError:
                return

    # Saves on close
    def closeEvent(self, event):
        if self.script is not "":
            self.save()
        event.accept()


app = QtWidgets.QApplication(sys.argv)
main_window = Window()
sys.exit(app.exec_())

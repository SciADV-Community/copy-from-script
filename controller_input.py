import time

from PyQt5.QtCore import QThread, pyqtSignal
from inputs import get_gamepad


class ControllerThread(QThread):

    controller_input = pyqtSignal(object)

    def __init__(self, parent=None):
        super(ControllerThread, self).__init__(parent)

    def run(self):
        while True:  # Event Loop
            events = get_gamepad()
            for event in events:
                if event.code is "ABS_HAT0X" and event.state is -1:
                    self.controller_input.emit("LEFT_ARROW")
                elif event.code is "ABS_HAT0Y" and event.state is -1:
                    self.controller_input.emit("UP_ARROW")
                elif (event.code is "BTN_EAST" or event.code is "BTN_SOUTH") and event.state is 1:
                    self.controller_input.emit(event.code)

            time.sleep(0.035)

            # Triangle Press = BTN_NORTH 1 | Release = 0
            # Square Press = BTN_WEST 1
            # Cross Press = BTN_SOUTH 1
            # Circle Press = BTN_EAST 1

            # Up Arrow Press = ABS_HAT0Y -1 | Release = 0
            # Left Arrow Press = ABS_HAT0X -1
            # Bottom Arrow Press = ABS_HAT0Y = 1
            # Right Arrow Press = ABS_HAT0X 1

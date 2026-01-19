import sys
from PyQt6.QtWidgets import (
    QApplication, QWidget, QPushButton, QLabel, QLineEdit,
    QVBoxLayout, QHBoxLayout, QGridLayout, QFrame
)
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QFont


class MouseConfigApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Mouse Config")
        self.setMinimumSize(900, 600)

        main_layout = QVBoxLayout()
        main_layout.setSpacing(20)

        # -------- TOP PROFILE BAR --------
        profile_bar = QHBoxLayout()
        profiles = ["Profile 1", "Profile 2", "Profile 3", "Profile 4"]
        for name in profiles:
            b = QPushButton(name)
            b.setFixedHeight(40)
            profile_bar.addWidget(b)

        main_layout.addLayout(profile_bar)

        # -------- CONTENT AREA --------
        content_layout = QHBoxLayout()
        content_layout.setSpacing(30)

        # LEFT PANEL (Button assignments)
        left_panel = self.build_left_panel()
        content_layout.addWidget(left_panel)

        # CENTER PANEL (mouse preview)
        center_panel = self.build_preview_panel()
        content_layout.addWidget(center_panel)

        main_layout.addLayout(content_layout)

        # -------- SCROLLING SPEED SECTION --------
        speed_section = self.build_speed_section()
        main_layout.addWidget(speed_section)

        # -------- BOTTOM BUTTONS --------
        bottom = QHBoxLayout()
        bottom.addStretch()

        bottom.addWidget(QLabel("DPI"))
        dpi_input = QLineEdit("1000")
        dpi_input.setFixedWidth(100)
        bottom.addWidget(dpi_input)

        reset_btn = QPushButton("Restore to Default")
        apply_btn = QPushButton("Apply Changes")

        bottom.addWidget(reset_btn)
        bottom.addWidget(apply_btn)

        main_layout.addLayout(bottom)

        self.setLayout(main_layout)
        self.apply_styles()


    # --------------------------------------------------------
    # BUILD LEFT PANEL
    # --------------------------------------------------------
    def build_left_panel(self):
        frame = QFrame()
        layout = QVBoxLayout()

        button_names = ["Left Click", "Right Click", "Ctrl + C", "Ctrl + V"]

        for i, name in enumerate(button_names, start=1):
            row = QHBoxLayout()

            num = QLabel(str(i))
            num.setFixedSize(30, 30)
            num.setAlignment(Qt.AlignmentFlag.AlignCenter)

            btn = QPushButton(name)
            btn.setFixedHeight(35)

            row.addWidget(num)
            row.addWidget(btn)

            layout.addLayout(row)

        edit = QPushButton("Edit Buttons")
        edit.setFixedHeight(30)
        layout.addWidget(edit)

        frame.setLayout(layout)
        return frame


    # --------------------------------------------------------
    # BUILD CENTER PANEL
    # --------------------------------------------------------
    def build_preview_panel(self):
        frame = QFrame()
        frame.setFixedSize(380, 260)
        layout = QGridLayout()

        # Just placeholders for now
        for i in range(1, 5):
            b = QPushButton(str(i))
            b.setFixedSize(80, 60)
            layout.addWidget(b, (i-1)//2, (i-1) % 2)

        frame.setLayout(layout)
        return frame


    # --------------------------------------------------------
    # SCROLLING SPEED SECTION
    # --------------------------------------------------------
    def build_speed_section(self):
        frame = QFrame()
        layout = QVBoxLayout()

        title = QLabel("Scrolling Speed")
        title.setFont(QFont("Segoe UI", 12, QFont.Weight.Bold))
        layout.addWidget(title)

        speed_row = QHBoxLayout()
        for speed in [1, 3, 5, 6]:
            b = QPushButton(str(speed))
            b.setFixedSize(80, 40)
            speed_row.addWidget(b)

        layout.addLayout(speed_row)
        frame.setLayout(layout)
        return frame


    # --------------------------------------------------------
    # APPLY BASIC DARK THEME STYLING
    # --------------------------------------------------------
    def apply_styles(self):
        self.setStyleSheet("""
            QWidget {
                background-color: #1e1e1e;
                color: white;
                font-family: 'Segoe UI';
                font-size: 12pt;
            }
            QPushButton {
                background: #2c2c2c;
                border: 1px solid #555;
                padding: 6px;
                border-radius: 4px;
            }
            QPushButton:hover {
                background: #3c3c3c;
            }
            QLabel {
                color: white;
            }
            QFrame {
                border: none;
            }
        """)


def main():
    app = QApplication(sys.argv)
    window = MouseConfigApp()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()

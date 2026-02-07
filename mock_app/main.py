import sys
from PyQt6.QtWidgets import (
    QApplication, QWidget, QPushButton, QLabel, QLineEdit,
    QVBoxLayout, QHBoxLayout, QGridLayout, QFrame, QComboBox
)
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QFont


# -----------------------------
# MAIN APPLICATION
# -----------------------------
class MouseConfigApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Mouse Config")
        self.setMinimumSize(900, 600)

        # -----------------------------
        # DATA MODEL
        # -----------------------------
        self.current_profile = 0

        self.profiles = [
            ["Left Click", "Right Click", "Ctrl + C", "Ctrl + V"],
            ["b1", "b2", "b3", "B4"],
            ["b1", "b2", "b3", "B4"],
            ["b1", "b2", "b3", "B4"],
        ]

        self.scroll_stage_count = 4
        self.scroll_speeds = [1, 3, 5, 6, 8]

        # -----------------------------
        # UI LAYOUT
        # -----------------------------
        main_layout = QVBoxLayout()
        main_layout.setSpacing(20)

        # -------- PROFILE BAR --------
        profile_bar = QHBoxLayout()
        profile_bar.addWidget(QLabel("Profile"))

        self.profile_selector = QComboBox()
        self.profile_selector.addItems(
            ["Profile 1", "Profile 2", "Profile 3", "Profile 4"]
        )
        self.profile_selector.currentIndexChanged.connect(self.select_profile)
        profile_bar.addWidget(self.profile_selector)
        profile_bar.addStretch()

        main_layout.addLayout(profile_bar)

        # -------- CONTENT --------
        content_layout = QHBoxLayout()
        content_layout.setSpacing(30)

        content_layout.addWidget(self.build_left_panel())
        content_layout.addWidget(self.build_preview_panel())
        content_layout.addWidget(self.build_scroll_panel())

        main_layout.addLayout(content_layout)

        # -------- BOTTOM BUTTONS --------
        bottom = QHBoxLayout()
        bottom.addStretch()

        reset_btn = QPushButton("Reset")
        save_btn = QPushButton("Save")

        save_btn.clicked.connect(self.save_config)

        bottom.addWidget(reset_btn)
        bottom.addWidget(save_btn)

        main_layout.addLayout(bottom)

        self.setLayout(main_layout)
        self.apply_styles()

        self.update_button_labels()

    # -----------------------------
    # LEFT PANEL (BUTTON MACROS)
    # -----------------------------
    def build_left_panel(self):
        frame = QFrame()
        frame.setObjectName("card")
        layout = QVBoxLayout()

        self.button_widgets = []

        for i in range(4):
            row = QHBoxLayout()

            num = QLabel(str(i + 1))
            num.setFixedSize(30, 30)
            num.setAlignment(Qt.AlignmentFlag.AlignCenter)

            btn = QPushButton("")
            btn.setFixedHeight(36)

            self.button_widgets.append(btn)

            row.addWidget(num)
            row.addWidget(btn)
            layout.addLayout(row)

        frame.setLayout(layout)
        return frame

    # -----------------------------
    # CENTER PREVIEW (VISUAL ONLY)
    # -----------------------------
    def build_preview_panel(self):
        frame = QFrame()
        frame.setObjectName("card")
        frame.setFixedSize(360, 260)
        layout = QGridLayout()

        for i in range(4):
            b = QPushButton(str(i + 1))
            b.setFixedSize(80, 60)
            layout.addWidget(b, i // 2, i % 2)

        frame.setLayout(layout)
        return frame

    # -----------------------------
    # RIGHT PANEL (SCROLL STAGES)
    # -----------------------------
    def build_scroll_panel(self):
        frame = QFrame()
        frame.setObjectName("card")
        layout = QVBoxLayout()

        title = QLabel("Scrolling Speed Stages")
        title.setFont(QFont("Segoe UI", 12, QFont.Weight.Bold))
        layout.addWidget(title)

        # Stage count selector
        stage_row = QHBoxLayout()
        stage_row.addWidget(QLabel("Stages"))

        self.stage_selector = QComboBox()
        self.stage_selector.addItems(["2", "3", "4", "5"])
        self.stage_selector.setCurrentText(str(self.scroll_stage_count))
        self.stage_selector.currentTextChanged.connect(self.change_stage_count)

        stage_row.addWidget(self.stage_selector)
        stage_row.addStretch()
        layout.addLayout(stage_row)

        # Speed inputs
        self.scroll_inputs_layout = QHBoxLayout()
        self.scroll_inputs = []

        layout.addLayout(self.scroll_inputs_layout)
        frame.setLayout(layout)

        self.update_scroll_stage_ui()
        return frame

    # -----------------------------
    # SCROLL STAGE LOGIC
    # -----------------------------
    def update_scroll_stage_ui(self):
        while self.scroll_inputs_layout.count():
            item = self.scroll_inputs_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        self.scroll_inputs.clear()

        for i in range(self.scroll_stage_count):
            edit = QLineEdit(str(self.scroll_speeds[i]))
            edit.setFixedWidth(70)
            edit.setAlignment(Qt.AlignmentFlag.AlignCenter)

            edit.editingFinished.connect(
                lambda idx=i, e=edit: self.set_scroll_stage_value(idx, e.text())
            )

            self.scroll_inputs.append(edit)
            self.scroll_inputs_layout.addWidget(edit)

        self.scroll_inputs_layout.addStretch()

    def change_stage_count(self, value):
        self.scroll_stage_count = int(value)
        self.update_scroll_stage_ui()
        print(f"SET_SCROLL_STAGE_COUNT {value}")

    def set_scroll_stage_value(self, index, value):
        try:
            v = int(value)
            if v <= 0:
                raise ValueError
        except ValueError:
            self.scroll_inputs[index].setText(str(self.scroll_speeds[index]))
            return

        self.scroll_speeds[index] = v
        print(f"SET_SCROLL_STAGE {index} {v}")

    # -----------------------------
    # PROFILE LOGIC
    # -----------------------------
    def select_profile(self, index):
        self.current_profile = index
        self.update_button_labels()
        print(f"SET_PROFILE {index}")

    def update_button_labels(self):
        for i, btn in enumerate(self.button_widgets):
            btn.setText(self.profiles[self.current_profile][i])

    # -----------------------------
    # SAVE
    # -----------------------------
    def save_config(self):
        print("SAVE")

    # -----------------------------
    # STYLES
    # -----------------------------
    def apply_styles(self):
        self.setStyleSheet("""
            QWidget {
                background-color: #181818;
                color: #e6e6e6;
                font-family: Segoe UI;
                font-size: 11pt;
            }

            QFrame#card {
                background-color: #202020;
                border: 1px solid #2f2f2f;
                border-radius: 10px;
                padding: 12px;
            }

            QLabel {
                color: #dcdcdc;
            }

            QLabel#sectionTitle {
                font-size: 13pt;
                font-weight: bold;
            }

            QPushButton {
                background-color: #2a2a2a;
                border: 1px solid #3a3a3a;
                border-radius: 6px;
                padding: 8px 12px;
            }

            QPushButton:hover {
                background-color: #333333;
                border-color: #ff8c1a;
            }

            QPushButton:pressed {
                background-color: #1f1f1f;
            }

            QLineEdit {
                background-color: #1c1c1c;
                border: 1px solid #3a3a3a;
                border-radius: 6px;
                padding: 6px;
                color: white;
            }

            QLineEdit:focus {
                border-color: #ff8c1a;
            }

            QComboBox {
                background-color: #1c1c1c;
                border: 1px solid #3a3a3a;
                border-radius: 6px;
                padding: 6px;
            }

            QComboBox::drop-down {
                border: none;
            }
        """)



# -----------------------------
# ENTRY POINT
# -----------------------------
def main():
    app = QApplication(sys.argv)
    window = MouseConfigApp()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()

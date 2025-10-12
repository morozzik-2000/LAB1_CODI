#include "themestyles.h"

QString ThemeStyles::darkButtonStyle() {
    return R"(
        QPushButton {
            color: white;
            background-color: #424242;
            border: 1px solid #777;
            border-radius: 15px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #555555;
        }
        QPushButton:pressed {
            background-color: #2e2e2e;
            padding-top: 6px;
            padding-bottom: 4px;
        }
    )";
}

QString ThemeStyles::darkGroupBoxStyle() {
    return R"(
        QGroupBox {
            color: white;
            border: 1px solid #aaa;
            border-radius: 25px;
            margin-top: 13px;
            padding-top: 5px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            background-color: #353535;
        }
    )";
}

QString ThemeStyles::darkSpinBoxStyle() {
    return R"(
        QSpinBox {
            color: white;
            background-color: #424242;
            border: 1px solid #aaa;
            padding: 2px;
            border-radius: 10px;
        }
        QSpinBox::up-arrow, QSpinBox::down-arrow {
            width: 0;
            height: 0;
            border: none;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            width: 0;
            height: 0;
        }
    )";
}

QString ThemeStyles::darkComboBoxStyle() {
    return "QComboBox { color: white; background-color:#424242; }";
}

QString ThemeStyles::lightButtonStyle() {
    return R"(
        QPushButton {
            color: black;
            background-color: #ffffff;
            border: 1px solid #aaa;
            border-radius: 15px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        QPushButton:pressed {
            background-color: #d0d0d0;
            padding-top: 6px;
            padding-bottom: 4px;
        }
    )";
}

QString ThemeStyles::OctaveButtonStyle() {
    return R"(
        QPushButton {
            color: black;
            background-color: #27ae60;
            border: 1px solid #aaa;
            border-radius: 15px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        QPushButton:pressed {
            background-color: #d0d0d0;
            padding-top: 6px;
            padding-bottom: 4px;
        }
    )";
}

QString ThemeStyles::lightGroupBoxStyle() {
    return R"(
        QGroupBox {
            color: black;
            border: 1px solid #555;
            border-radius: 10px;
            margin-top: 13px;
            padding-top: 5px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            background-color: #f0f0f0;
        }
    )";
}

QString ThemeStyles::lightSpinBoxStyle() {
    return R"(
        QSpinBox {
            color: black;
            background-color: #ffffff;
            border: 1px solid #555;
            border-radius: 10px;
            padding: 2px;
        }
        QSpinBox::up-arrow, QSpinBox::down-arrow {
            width: 0;
            height: 0;
            border: none;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            width: 0;
            height: 0;
        }
    )";
}

QString ThemeStyles::lightComboBoxStyle() {
    return "QComboBox { color: black; background-color:#ffffff; }";
}

QString ThemeStyles::logTextEdit() {
    return  R"(
            QTextEdit {
                border: 1px solid palette(mid);
                border-radius: 8px;
                padding: 5px;
                background-color: palette(base);
                color: palette(text);
            }
     )";
}

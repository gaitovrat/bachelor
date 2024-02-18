#include "Utils.h"

void Utils::setComboBox(QComboBox& comboBox, const QString& value) {
    int index = comboBox.findText(value);
    if (index == -1)
        return;

    comboBox.setCurrentIndex(index);
}

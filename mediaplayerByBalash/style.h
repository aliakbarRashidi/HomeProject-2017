#ifndef STYLE_H
#define STYLE_H

#include <QString>

class Style
{
public:
    static QString getWindowStyleSheet();
    static QString getLabelStyleSheet();
    static QString getCloseStyleSheet();
    static QString getMaximizeStyleSheet();
    static QString getRestoreStyleSheet();
    static QString getMinimizeStyleSheet();
    static QString getNextStyleSheet();
    static QString getPreviousStyleSheet();
    static QString getStopStyleSheet();
    static QString getPlayStyleSheet();
    static QString getRandomStyleSheet();
    static QString getSequentialStyleSheet();
    static QString getPauseStyleSheet();
    static QString getMenuStyleSheet();
    static QString getTableViewStyleSheet();
    static QString getAddPushStyleSheet();
    static QString getAddStyleSheet();
    static QString getRemoveStyleSheet();
    static QString getSliderStyleSheet();
    static QString getSliderStyleSheet2();
    static QString getVerticalScrollBarStyleSheet();
    static QString getHorizontalScrollBarStyleSheet();
    static QString getBtnToolStyleSheet();
    static QString getFullStyleSheet();
    static QString getSettingsStyleSheet();
    static QString getBtnPushToolStyleSheet();
    static QString getComboBoxStyleSheet();
};

#endif // STYLE_H

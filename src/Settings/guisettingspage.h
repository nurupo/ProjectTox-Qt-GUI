#ifndef GUISETTINGSPAGE_H
#define GUISETTINGSPAGE_H

#include "settings.hpp"
#include "abstractsettingspage.hpp"

class QComboBox;
class QLabel;

class GuiSettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    explicit GuiSettingsPage(QWidget *parent = 0);

    void applyChanges();
    void buildGui();
    void setGui();

private slots:
    void showRestartInfo(int i);
    
private:
    QComboBox *langCombo;
    QLabel    *restartLabel;
};

#endif // GUISETTINGSPAGE_H

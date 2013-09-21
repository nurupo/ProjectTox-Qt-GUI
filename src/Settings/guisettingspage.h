#ifndef GUISETTINGSPAGE_H
#define GUISETTINGSPAGE_H

#include "abstractsettingspage.hpp"

class QGroupBox;
class QCheckBox;
class QComboBox;
class QLabel;

class GuiSettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    GuiSettingsPage(QWidget *parent);

    void buildGui();
    void setGui();
    void applyChanges();
    
private slots:
    void updateSmileypackDetails(int index);

private:
    QGroupBox* buildAnimationGroup();
    QGroupBox* buildSmileypackGroup();



    QCheckBox* enableAnimationCheckbox;

    QComboBox* smileypackCombobox;
    QLabel*    smileypackNameLabel;
    QLabel*    smileypackDescLabel;
    QLabel*    smileypackWebLabel;
};

#endif // GUISETTINGSPAGE_H

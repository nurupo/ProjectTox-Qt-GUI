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

    void searchSmileyPacks();



    QCheckBox* enableAnimationCheckbox;

    QComboBox* smileypackCombobox;
    QLabel*    smileypackDescLabel;
    QCheckBox* useDejavuFontCheckbox;
};


#endif // GUISETTINGSPAGE_H

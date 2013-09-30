#ifndef GUISETTINGSPAGE_H
#define GUISETTINGSPAGE_H

#include "abstractsettingspage.hpp"

class QGroupBox;
class QCheckBox;
class QComboBox;
class QLabel;
class QToolButton;
class EmoijFontSettings;

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


    EmoijFontSettings *emoijSettings;
    QCheckBox* enableAnimationCheckbox;

    QComboBox* smileypackCombobox;
    QToolButton *emoijButton;
    QLabel*    smileypackDescLabel;
};


#endif // GUISETTINGSPAGE_H

#ifndef GUISETTINGSPAGE_H
#define GUISETTINGSPAGE_H

#include "abstractsettingspage.hpp"

class QGroupBox;
class QCheckBox;

class GuiSettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    GuiSettingsPage(QWidget *parent);

    void buildGui();
    void setGui();
    void applyChanges();
    
signals:
    
public slots:
    
private:
    QGroupBox* buildAnimationGroup();

    QCheckBox* enableAnimationCheckbox;

};

#endif // GUISETTINGSPAGE_H

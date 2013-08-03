#ifndef GUISETTINGSPAGE_H
#define GUISETTINGSPAGE_H

#include "abstractsettingspage.hpp"

class GuiSettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    explicit GuiSettingsPage(QWidget *parent = 0);

    void applyChanges();
    void buildGui();
    void setGui();
    
signals:
    
public slots:
    
};

#endif // GUISETTINGSPAGE_H

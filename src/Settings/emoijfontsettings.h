#ifndef EMOIJFONTSETTINGS_H
#define EMOIJFONTSETTINGS_H

#include <QDialog>

class QGroupBox;
class QFontComboBox;
class QComboBox;
class QLabel;

class EmoijFontSettings : public QDialog
{
    Q_OBJECT
public:
    explicit EmoijFontSettings(QWidget *parent = 0);

    bool    isCustomEmoijFont();
    void    setCustomEmoijFont(bool x);
    QString getEmoijFont();
    void    setEmoijFont(const QString &x);
    int     getEmoijSize();
    void    setEmoijSize(int x);

private slots:
    void updatePreview();
    void resetToDefault();
    
private:
    QGroupBox *selectGroup;
    QFontComboBox *fontCombobox;
    QComboBox *sizeComboBox;
    QLabel    *previewLabel;
};

#endif // EMOIJFONTSETTINGS_H

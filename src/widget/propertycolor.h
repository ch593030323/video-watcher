#ifndef PROPERTYCOLOR_H
#define PROPERTYCOLOR_H

#include <QWidget>

class PropertyColor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor highlightColor READ gethighlightColor WRITE sethighlightColor DESIGNABLE true)
    Q_PROPERTY(QColor textColor      READ gettextColor      WRITE settextColor      DESIGNABLE true)
    Q_PROPERTY(QColor borderColor READ getborderColor WRITE setborderColor DESIGNABLE true)
    Q_PROPERTY(QColor subToolColor READ getsubToolColor WRITE setsubToolColor DESIGNABLE true)
    Q_PROPERTY(QColor buttonColor READ getbuttonColor WRITE setbuttonColor DESIGNABLE true)
    Q_PROPERTY(QColor buttonTextColor READ getbuttonTextColor WRITE setbuttonTextColor DESIGNABLE true)
    Q_PROPERTY(QColor viewColor READ getviewColor WRITE setviewColor DESIGNABLE true)
    Q_PROPERTY(QColor buttonHoverColor READ getbuttonHoverColor WRITE setbuttonHoverColor DESIGNABLE true)

public:
    void sethighlightColor(const QColor &color);
    QColor gethighlightColor() const;
    static QColor highlightColor;

    void settextColor(const QColor &color);
    QColor gettextColor() const;
    static QColor textColor;

    void setborderColor(const QColor &color);
    QColor getborderColor() const;
    static QColor borderColor;

    void setsubToolColor(const QColor &color);
    QColor getsubToolColor() const;
    static QColor subToolColor;

    void setbuttonColor(const QColor &color);
    QColor getbuttonColor() const;
    static QColor buttonColor;

    void setbuttonTextColor(const QColor &color);
    QColor getbuttonTextColor() const;
    static QColor buttonTextColor;

    void setviewColor(const QColor &color);
    QColor getviewColor() const;
    static QColor viewColor;

    void setbuttonHoverColor(const QColor &color);
    QColor getbuttonHoverColor() const;
    static QColor buttonHoverColor;

    static void init(const QString &iconFontFamily, const QString &styleSheetString);
    static QPixmap getFontPixmap(QChar ch, const QColor &color = QColor("#f6f5fb"), const QSize &size = QSize(40, 40), qreal rotate = 0);

private:
    explicit PropertyColor(QWidget *parent = 0);
    static QString iconFontFamily;
};

#endif // PROPERTYCOLOR_H

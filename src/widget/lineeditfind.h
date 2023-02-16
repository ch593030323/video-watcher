#ifndef LINEEDITFIND_H
#define LINEEDITFIND_H

#include <QFrame>
#include <QLineEdit>
#include <QPushButton>

class LineEditFind : public QFrame
{
    Q_OBJECT

public:
    explicit LineEditFind(QWidget *parent = 0);

    QLineEdit *lineEdit();

    void hideFindButton();

    void setText(const QString &text);
    QString text();

private slots:
    void slotShowCloseButton(const QString &text);

private:
    QLineEdit *edit;
    QPushButton *buttonFind;
    QPushButton *buttonClose;
};

#endif // LINEEDITFIND_H

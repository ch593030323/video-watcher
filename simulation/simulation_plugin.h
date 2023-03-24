#ifndef SIMULATION_PLUGIN_H
#define SIMULATION_PLUGIN_H


#include <QObject>
#include <QtGui>
#include <sche/customPlugin.h>
#include "simulation_widget.h"



class controlButtonSettingDialog : public QDialog{
    Q_OBJECT
public:
    controlButtonSettingDialog( SimulationWidget* pWidget);
    ~controlButtonSettingDialog();
    void setData(QString);
    void showError(QWidget*,QString);
    bool attrbuteCheck(Database*,QString);
    void setLineEditeState(QLineEdit*,bool);


public slots:
    void doSave();
    void selectObject();
    void onUserDefault(int);

private:
    QLineEdit* text;
    QLineEdit* textsize;
    QCheckBox* usedefault;
    QCheckBox* needconfirm;
    QPushButton* objectId;
    QLineEdit* attribute;
    QLineEdit* attrvalue;
    QPushButton* permitId;
    QPushButton* clearPermitBtn;
    QLineEdit* logmsg;
    QPushButton* okButton;
    SimulationWidget *m_widget;
};

////////////////////////////////////////////////////////////////////////////////

class SimulationPlugin : public QObject,public ScheCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(ScheCustomWidgetInterface)
public:
            // ctor
            SimulationPlugin(QObject *parent = 0);
    ~SimulationPlugin();

    // UI widget creation
    virtual QWidget *createWidget(QWidget *parent, Database *database = NULL, HistoricalClient *histClient = NULL, void *data = NULL, int environ = 0);

    // Property dialog invocation
    virtual bool popupWidget(QWidget *parent, void *data = NULL);

    // Database handle setting
    virtual void setDataBase(Database *database);
    virtual void setInterfaces(UserMSInterface* userIface,OperationLog* olog);

    // Interfacing chores
    virtual bool isContainer() const;
    virtual void* getData();
    virtual void setParent(QWidget *parent);
    virtual void resize(int w, int h);
    virtual QIcon icon() const;
    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QString domSvg() const;
    virtual QString domG() const;

private:
    SimulationWidget *m_widget;
    controlButtonSettingDialog *m_dialog;

private slots:
    void startLog();
};


#endif //SIMULATION_PLUGIN_H

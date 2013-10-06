#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTimer>

namespace Ui {
class Configurator;
}

class Configurator : public QMainWindow
{
    Q_OBJECT

public:
    explicit Configurator(QWidget *parent = 0);
    ~Configurator();

public slots:
    void s_save();

private:
    Ui::Configurator *ui;
    QString m_configFile;
};

#endif // CONFIGURATOR_H

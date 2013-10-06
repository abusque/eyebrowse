#include "configurator.h"
#include "ui_configurator.h"

#include <libconfig.h++>
#include <iostream>

#define DEFAULT_LOCKSCREEN_ACTIVATION true
#define DEFAULT_LOCKSCREEN_DELAY      2

#define DEFAULT_SCROLLDOWN_ACTIVATION true
#define DEFAULT_SCROLLDOWN_DELAY      2
#define DEFAULT_SCROLLDOWN_TICKS      8

using namespace std;
using namespace libconfig;

Configurator::Configurator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Configurator)
{
    ui->setupUi(this);

    m_configFile = QFileDialog::getOpenFileName(this, tr("Open Config File"), "",  tr("Files (*.cfg)"));

    if(m_configFile.length() == 0)
        QTimer::singleShot(0, this, SLOT(close()));

    Config cfg;

    // Read the file. If there is an error, report it and exit.
    try
    {
        cfg.readFile(m_configFile.toStdString().c_str());
    }
    catch(const FileIOException &fioex)
    {
        //File do not exist or error!
        //std::cerr << "I/O error while reading file." << std::endl;
        //return(EXIT_FAILURE);
    }
    catch(const ParseException &pex)
    {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return;
    }

    // Find the 'lockscreen' setting. Add intermediate settings if they don't yet
    // exist.
    Setting &root = cfg.getRoot();
    if(!root.exists("lockScreen") || !root.exists("scrollDown"))
        return;

    Setting &lockScreen = root["lockScreen"];
    bool lockActivated = DEFAULT_LOCKSCREEN_ACTIVATION;
    lockScreen.lookupValue("activated", lockActivated);
    ui->ckbLockScreen->setChecked(lockActivated);

    int delay = DEFAULT_LOCKSCREEN_DELAY;
    lockScreen.lookupValue("delay", delay);
    ui->updDelayLock->setValue(delay);

    Setting &scrollDown = root["scrollDown"];

    bool scrollActivated = DEFAULT_SCROLLDOWN_ACTIVATION;
    scrollDown.lookupValue("activated", scrollActivated);
    ui->ckbScrollDown->setChecked(scrollActivated);

    delay = DEFAULT_SCROLLDOWN_DELAY;
    scrollDown.lookupValue("delay", delay);
    ui->updScrollDownDelay->setValue(delay);

    int ticks = DEFAULT_SCROLLDOWN_TICKS;
    scrollDown.lookupValue("ticks", ticks);
    ui->updScrollTicks->setValue(ticks);
}

Configurator::~Configurator()
{
    delete ui;
}

void Configurator::s_save()
{
    Config cfg;

    // Find the 'lockscreen' setting. Add intermediate settings if they don't yet
    // exist.
    Setting &root = cfg.getRoot();

    Setting &lockScreen = root.add("lockScreen", Setting::TypeGroup);
    lockScreen.add("activated", Setting::TypeBoolean) = ui->ckbLockScreen->isChecked();
    lockScreen.add("delay", Setting::TypeInt) = ui->updDelayLock->value();

    Setting &scrollDown = root.add("scrollDown", Setting::TypeGroup);
    scrollDown.add("activated", Setting::TypeBoolean) = ui->ckbScrollDown->isChecked();
    scrollDown.add("delay", Setting::TypeInt) = ui->updScrollDownDelay->value();
    scrollDown.add("ticks", Setting::TypeInt) = ui->updScrollTicks->value();

    // Write out the updated configuration.
    try
    {
        cfg.writeFile(m_configFile.toStdString().c_str());

    }
    catch(const FileIOException &fioex)
    {
      cerr << "I/O error while writing file: " << m_configFile.toStdString() << endl;
      return;
    }


    close();
}

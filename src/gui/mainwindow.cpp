#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "adapters/joystickadapter.h"
#include "adapters/mapadapter.h"
#include "adapters/gpsadapter.h"
#include "adapters/joystickadapter.h"
#include "adapters/cameraadapter.h"
#include "adapters/imuadapter.h"

#include <hardware/sensors/gps/simulatedgps.h>
#include <hardware/sensors/gps/HemisphereA100GPS.h>
#include <hardware/sensors/camera/StereoPlayback.h>
#include <hardware/sensors/IMU/Ardupilot.h>
#include <hardware/sensors/lidar/SimulatedLidar.h>

#include <QMdiSubWindow>
#include <QTextEdit>
#include "adapters/joystickadapter.h"
#include "adapters/lidaradapter.h"
#include <QDebug>
#include <QFileDialog>

#include <iostream>

using namespace IGVC::Sensors;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configTreeModel.populateModel();
    ui->configTree->setModel(configTreeModel.model());
    mdiArea = ui->mainDisplayArea;
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    checkIcon = QIcon(QString(":/images/Checkmark"));
    xIcon = QIcon(QString(":/images/Close"));

    connect(ui->hardwareStatusList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openHardwareView(QModelIndex)));

    Logger::setSatusBar(ui->statusBar);

    setupMenus();
    updateWindowMenu();

    _motorController = new MotorEncoderDriver2013;
    ui->hardwareStatusList->addItem("Motor Board");

    _joystick = new Joystick;
    ui->joystickButton->setEnabled(_joystick->isOpen());
    ui->hardwareStatusList->addItem("Joystick");

    ui->hardwareStatusList->addItem("Map");

    _joystickDriver = new JoystickDriver(&_joystick->onNewData);

    _lidar = new SimulatedLidar();
    ui->hardwareStatusList->addItem("LIDAR");

    _stereoSource = new StereoPlayback((QDir::currentPath() + "/../../test_data/video/CompCourse_left0.mpeg").toStdString(),(QDir::currentPath() + "/../../test_data/video/CompCourse_right0.mpeg").toStdString(),20,"",false);
    ui->hardwareStatusList->addItem("Camera");

    _GPS = new SimulatedGPS((QDir::currentPath() + "/GPSData.txt").toStdString());
    ui->actionSimulatedGPS->setChecked(true);
    ui->hardwareStatusList->addItem("GPS");

    _IMU = new Ardupilot();
    ui->hardwareStatusList->addItem("IMU");

    updateHardwareStatusIcons();

    isRunning = false;
    isPaused = false;
    ui->stopButton->setVisible(false);
}

void MainWindow::setupMenus()
{
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->menuWindow, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
    connect(ui->actionClose_2, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));
    connect(ui->actionClose_All, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));
    connect(ui->actionCascade, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionTile, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));
}

MainWindow::~MainWindow()
{
    delete _joystick;
    delete _GPS;
    delete ui;
}

void MainWindow::openHardwareView(QModelIndex index)
{
    QString labelText = ui->hardwareStatusList->item(index.row())->text();
    if(MDIWindow* window = findWindowWithTitle(labelText))
    {
        if(!window->isVisible())
            window->show();
    }
    else
    {
        using namespace std;
        MDIWindow *newWindow = new MDIWindow;
        newWindow->setWindowTitle(labelText);
        newWindow->setLayout(new QGridLayout);

        QWidget* adapter;

        if(labelText == "Joystick")
        {
            adapter = new JoystickAdapter(_joystick);
        }
        else if(labelText == "LIDAR")
        {
            adapter = new LidarAdapter(_lidar);
	}
        else if(labelText == "Map")
        {
            adapter = new MapAdapter();
        }
        else if(labelText == "GPS")
        {
            adapter = new GPSAdapter(_GPS);
        }
        else if(labelText == "Camera")
        {
            adapter = new CameraAdapter(_stereoSource);
        }
        else if(labelText == "IMU")
        {
            adapter = new IMUAdapter(_IMU);
        }
        else
        {
            adapter = new QWidget();
        }

        newWindow->layout()->addWidget(adapter);

        mdiArea->addSubWindow(newWindow);
        newWindow->show();
    }

    updateWindowMenu();
}

void MainWindow::on_actionFullscreen_triggered()
{
    if(ui->actionFullscreen->isChecked())
        this->showFullScreen();
    else
        this->showNormal();
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);

    ui->actionTile->setEnabled(hasMdiChild);
    ui->actionCascade->setEnabled(hasMdiChild);
    ui->actionClose_2->setEnabled(hasMdiChild);
    ui->actionClose_All->setEnabled(hasMdiChild);
}

void MainWindow::updateWindowMenu()
{
    for(int i = ui->menuWindow->actions().size()-1; i > 5; i--)
    {
        ui->menuWindow->removeAction(ui->menuWindow->actions().at(i));
    }

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i) {
        MDIWindow *child = qobject_cast<MDIWindow *>(windows.at(i)->widget());

        QString text = windows.at(i)->windowTitle();
        QAction *action  = ui->menuWindow->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MDIWindow* MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MDIWindow *>(activeSubWindow->widget());
    return 0;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

MDIWindow* MainWindow::findWindowWithTitle(QString title)
{
    foreach(QMdiSubWindow *window, mdiArea->subWindowList())
    {
        MDIWindow *mdiChild = qobject_cast<MDIWindow*>(window->widget());
        if(mdiChild && mdiChild->windowTitle() == title)
            return mdiChild;
    }
    return 0;
}

void MainWindow::on_joystickButton_toggled(bool checked)
{
    this->setFocus();
    if(checked)
    {
        _motorController->setControlEvent(&_joystickDriver->controlEvent);
    }
    else
    {
        //TODO : Set motor controller to listen to intelligence events
        _motorController->setControlEvent(0);
    }
}

void MainWindow::on_playButton_clicked()
{
    if(isRunning)
    {
        if(isPaused)
        {
            ui->playButton->setIcon(QIcon(":/images/Pause"));
        }
        else
        {
            ui->playButton->setIcon(QIcon(":/images/Play"));
        }
        isPaused = !isPaused;
    }
    else
    {
        ui->playButton->setIcon(QIcon(":/images/Pause"));
        ui->stopButton->setVisible(true);
        isRunning = true;
    }
}

void MainWindow::on_stopButton_clicked()
{
    if(isRunning)
    {
        ui->playButton->setIcon(QIcon(":/images/Play"));
        ui->stopButton->setVisible(false);
        isRunning = false;
        isPaused = false;
    }
}

void MainWindow::on_actionStatus_Bar_toggled(bool checked)
{
    if(checked)
    {
        ui->statusBar->show();
        Logger::setSatusBar(ui->statusBar);
    }
    else
    {
        ui->statusBar->hide();
        Logger::setSatusBar(0);
    }
}

void MainWindow::on_saveConfigButton_clicked()
{
    ConfigManager::Instance().save();
}

void MainWindow::on_loadConfigButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Configuration File"), "", tr("XML Files(*.xml)"));
    ConfigManager::Instance().load(fileName.toStdString());
}

void MainWindow::on_actionHemisphere_A100_triggered()
{
    ui->actionSimulatedGPS->setChecked(!ui->actionHemisphere_A100->isChecked());
    _GPS = new HemisphereA100GPS();
    updateHardwareStatusIcons();
}

void MainWindow::on_actionSimulatedGPS_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Simulated GPS Data File"), "", tr("Text Files(*.txt)"));
    if(fileName.length() > 0)
    {
        ui->actionHemisphere_A100->setChecked(!ui->actionSimulatedGPS->isChecked());
        _GPS = new SimulatedGPS(fileName.toStdString());
        updateHardwareStatusIcons();
    }
}

void MainWindow::updateHardwareStatusIcons()
{
    ui->hardwareStatusList->findItems("GPS", Qt::MatchExactly).at(0)->setIcon(_GPS->isOpen() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("Joystick", Qt::MatchExactly).at(0)->setIcon(_joystick->isOpen() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("Motor Board", Qt::MatchExactly).at(0)->setIcon(_motorController->isOpen() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("IMU", Qt::MatchExactly).at(0)->setIcon(_IMU->isWorking() ? checkIcon : xIcon);
}

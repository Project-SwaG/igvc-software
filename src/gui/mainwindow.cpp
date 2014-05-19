#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "adapters/joystickadapter.h"
#include "adapters/mapadapter.h"
#include "adapters/gpsadapter.h"
#include "adapters/joystickadapter.h"
#include "adapters/cameraadapter.h"
#include "adapters/imuadapter.h"
#include "adapters/pathadapter.h"
#include "adapters/lidaradapter.h"
#include "adapters/positiontrackeradapter.h"
#include "adapters/lightshieldadapter.h"
#include "adapters/motorboardadapter.h"

#include <hardware/sensors/gps/simulatedgps.h>
#include <hardware/sensors/gps/nmeacompatiblegps.h>
#include <hardware/sensors/camera/StereoPlayback.h>
#include <hardware/sensors/camera/Bumblebee2.h>
#include <hardware/sensors/IMU/Ardupilot.h>
#include <hardware/sensors/lidar/SimulatedLidar.h>
#include <hardware/sensors/lidar/lms200.h>

#include <QMdiSubWindow>
#include <QTextEdit>
#include <QDebug>
#include <QFileDialog>

#include <iostream>

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

    _lights = new LightController();
    connect(_lights, SIGNAL(onBatteryLevelChanged(int)), ui->batteryIndicator, SLOT(onBatteryLevelChanged(int)));
    connect(_lights, SIGNAL(onEStopStatusChanged(bool)), ui->statusImage, SLOT(onEStopStatusChanged(bool)));
    connect(_lights, SIGNAL(onEStopStatusChanged(bool)), _motorController, SLOT(onEStopStatusChanged(bool)));
    ui->hardwareStatusList->addItem("Light Controller");

    _joystick = new Joystick;
    ui->joystickButton->setEnabled(_joystick->isOpen());
    ui->hardwareStatusList->addItem("Joystick");

//    _lidar = new SimulatedLidar();
    _lidar = new LMS200();
    ui->hardwareStatusList->addItem("LIDAR");
    ui->actionLMS_200->setChecked(true);

    //_stereoSource = new StereoPlayback((QDir::currentPath() + "/../../test_data/video/CompCourse_left0.mpeg").toStdString(),(QDir::currentPath() + "/../../test_data/video/CompCourse_right0.mpeg").toStdString(),20,"",false);
    _stereoSource = new Bumblebee2("/home/robojackets/igvc/software/src/hardware/sensors/camera/calib/out_camera_data.xml");
    ui->hardwareStatusList->addItem("Camera");

//    _GPS = new SimulatedGPS("");
    _GPS = new NMEACompatibleGPS("/dev/ttyGPS", 19200);
    ui->actionOutback_A321->setChecked(true);
    ui->hardwareStatusList->addItem("GPS");

    _IMU = new Ardupilot();
    ui->hardwareStatusList->addItem("IMU");

    _posTracker = new BasicPositionTracker(_GPS, _IMU);
    ui->hardwareStatusList->addItem("Position Tracker");

    _lineDetector = new LineDetector();
    connect(_stereoSource, SIGNAL(onNewLeftImage(ImageData)), _lineDetector, SLOT(onImageEvent(ImageData)));

    _mapper = new MapBuilder(_lidar, _posTracker);
    connect(_lidar, SIGNAL(onNewData(LidarState)), _mapper, SLOT(onLidarData(LidarState)));
    // TOOO - Add slot in MapBuilder for point cloud data.
    //connect(_lineDetector, SIGNAL(onNewCloud(pcl::PointCloud<pcl::PointXYZ>)), _mapper, SLOT());

    _planner = new AStarPlanner();

    ui->hardwareStatusList->addItem("Map");

    ui->hardwareStatusList->addItem("Path Planner");

    _joystickDriver = new JoystickDriver(_joystick);

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
    delete ui;
    delete _joystick;
    delete _posTracker;
    delete _GPS;
    delete _IMU;
    delete _stereoSource;
    delete _lidar;
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

        if(labelText == "Motor Board")
        {
            adapter = new MotorBoardAdapter(_motorController);
        }
        else if(labelText == "Joystick")
        {
            adapter = new JoystickAdapter(_joystick);
        }
        else if(labelText == "LIDAR")
        {
            adapter = new LidarAdapter(_lidar);
	}
        else if(labelText == "Map")
        {
            adapter = new MapAdapter(_mapper, _posTracker);
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
        else if(labelText == "Position Tracker")
        {
            adapter = new PositionTrackerAdapter(_posTracker);
        }
        else if(labelText == "Path Planner")
        {
            adapter = new PathAdapter(_planner);
        }
        else if(labelText == "Light Controller")
        {
            adapter = new LightShieldAdapter(_lights);
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
    return nullptr;
}

void MainWindow::on_joystickButton_toggled(bool checked)
{
    this->setFocus();
    if(checked)
    {
        // TODO : disconnect from intelilgence signals
        connect(_joystickDriver, SIGNAL(onNewMotorCommand(MotorCommand)), _motorController, SLOT(setMotorCommand(MotorCommand)));
    }
    else
    {
        disconnect(_joystickDriver, SIGNAL(onNewMotorCommand(MotorCommand)), _motorController, SLOT(setMotorCommand(MotorCommand)));
        // TODO : connect to intelligence signals
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
    _lights->setSafetyLight(isRunning);
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
    _lights->setSafetyLight(isRunning);
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
    ui->actionSimulatedGPS->setChecked(false);
    ui->actionHemisphere_A100->setChecked(true);
    ui->actionOutback_A321->setChecked(false);
    _GPS = new NMEACompatibleGPS("/dev/ttyGPS", 4800);
    _posTracker->ChangeGPS(_GPS);
    updateHardwareStatusIcons();
}

void MainWindow::on_actionSimulatedGPS_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Simulated GPS Data File"), "", tr("Text Files(*.txt)"));
    if(fileName.length() > 0)
    {
        ui->actionSimulatedGPS->setChecked(true);
        ui->actionHemisphere_A100->setChecked(false);
        ui->actionOutback_A321->setChecked(false);
//        MDIWindow *window = findWindowWithTitle("GPS");
//        if( window != nullptr)
//        {
//            QWidget* p = (QWidget*)window->parent();
//            if(p != nullptr)
//                p->close();
//        }
        _GPS = new SimulatedGPS(fileName.toStdString());
        _posTracker->ChangeGPS(_GPS);
        updateHardwareStatusIcons();
    }
}

void MainWindow::updateHardwareStatusIcons()
{
    ui->hardwareStatusList->findItems("GPS", Qt::MatchExactly).at(0)->setIcon(_GPS->isOpen() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("Joystick", Qt::MatchExactly).at(0)->setIcon(_joystick->isOpen() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("Motor Board", Qt::MatchExactly).at(0)->setIcon(_motorController->isOpen() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("IMU", Qt::MatchExactly).at(0)->setIcon(_IMU->isWorking() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("LIDAR", Qt::MatchExactly).at(0)->setIcon(_lidar->IsWorking() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("Light Controller", Qt::MatchExactly).at(0)->setIcon(_lights->isConnected() ? checkIcon : xIcon);
    ui->hardwareStatusList->findItems("Camera", Qt::MatchExactly).at(0)->setIcon(_stereoSource->IsConnected() ? checkIcon : xIcon);
}

void MainWindow::on_actionClearLogs_triggered()
{
    Logger::Clear();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    mdiArea->closeAllSubWindows();
    QMainWindow::closeEvent(e);
}

void MainWindow::on_actionOutback_A321_triggered()
{
    ui->actionSimulatedGPS->setChecked(false);
    ui->actionHemisphere_A100->setChecked(false);
    ui->actionOutback_A321->setChecked(true);
    _GPS = new NMEACompatibleGPS("/dev/ttyGPS", 19200);
    _posTracker->ChangeGPS(_GPS);
    updateHardwareStatusIcons();
}

void MainWindow::on_actionSimulatedLidar_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Simulated Lidar Data File"), "", tr("CSV Files(*.csv)"));
    if(fileName.length() > 0)
    {
        ui->actionLMS_200->setChecked(false);
        ui->actionSimulatedLidar->setChecked(true);
        SimulatedLidar *newDevice = new SimulatedLidar;
        newDevice->loadFile(fileName.toStdString().c_str());
        _mapper->ChangeLidar(newDevice);
        _mapper->Clear();
        _lidar = newDevice;
        updateHardwareStatusIcons();
    }
}

void MainWindow::on_actionLMS_200_triggered()
{
    ui->actionLMS_200->setChecked(true);
    ui->actionSimulatedLidar->setChecked(false);
    _lidar = new LMS200;
    _mapper->ChangeLidar(_lidar);
    _mapper->Clear();
    updateHardwareStatusIcons();
}

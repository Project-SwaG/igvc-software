#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "batteryindicator.h"
#include "configtreemodel.h"
#include "mdiwindow.h"
#include <QMdiArea>
#include <QSignalMapper>
#include "hardware/sensors/joystick/Joystick.h"
#include "hardware/actuators/motors/MotorEncoderDriver2013.h"
#include "intelligence/JoystickDriver.hpp"
#include <hardware/sensors/gps/GPS.hpp>
#include <hardware/sensors/camera/StereoSource.hpp>
#include <hardware/sensors/IMU/IMU.h>
#include <hardware/sensors/lidar/Lidar.h>
#include <hardware/actuators/lights/lightcontroller.h>
#include <intelligence/posetracking/basicpositiontracker.h>
#include <intelligence/mapping/mapbuilder.h>
#include <intelligence/pathplanning/astarplanner.h>
#include <intelligence/linedetection/linedetector.h>
#include <intelligence/controller/controller.h>
#include <common/utils/GPSWaypointSource.h>

namespace Ui {
class MainWindow;
}

/*!
 * \brief The main window of the control application.
 * \author Matthew Barulic
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionFullscreen_triggered();

    void updateWindowMenu();
    void setActiveSubWindow(QWidget *window);

    void updateMenus();

    void openHardwareView(QModelIndex index);

    void on_joystickButton_toggled(bool checked);

    void on_playButton_clicked();

    void on_stopButton_clicked();

    void on_actionStatus_Bar_toggled(bool arg1);

    void on_saveConfigButton_clicked();

    void on_loadConfigButton_clicked();

    void on_actionHemisphere_A100_triggered();

    void on_actionSimulatedGPS_triggered();

    void on_actionClearLogs_triggered();

    void on_actionOutback_A321_triggered();

    void on_actionSimulatedLidar_triggered();

    void on_actionLMS_200_triggered();

    void on_actionLoad_Waypoint_File_triggered();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    ConfigTreeModel configTreeModel;
    QMdiArea* mdiArea;
    QSignalMapper *windowMapper;

    QIcon checkIcon;
    QIcon xIcon;

    JoystickDriver *_joystickDriver;
    std::shared_ptr<Joystick> _joystick;
    //Joystick *_joystick;

    MotorDriver *_motorController;

    GPS *_GPS;

    StereoSource *_stereoSource;

    IMU *_IMU;

    Lidar *_lidar;

    BasicPositionTracker *_posTracker;

    MapBuilder *_mapper;

    PathPlanner *_planner;

    LightController *_lights;

    LineDetector *_lineDetector;

    GPSWaypointSource *_waypointSource;

    Controller *_compController;

    bool isRunning, isPaused;

    MDIWindow *activeMdiChild();
    void setupMenus();
    MDIWindow* findWindowWithTitle(QString title);
    void updateHardwareStatusIcons();
};

#endif // MAINWINDOW_H

#ifndef POSITIONTRACKERADAPTER_H
#define POSITIONTRACKERADAPTER_H

#include <QWidget>
#include <intelligence/posetracking/positiontracker.hpp>
#include <vector>

namespace Ui {
class PositionTrackerAdapter;
}

class PositionTrackerAdapter : public QWidget
{
    Q_OBJECT

public:
    explicit PositionTrackerAdapter(std::shared_ptr<PositionTracker> src, QWidget *parent = 0);
    ~PositionTrackerAdapter();

protected:
    void paintEvent(QPaintEvent*);

Q_SIGNALS:
    void updateBecauseOfData();
    void setProgress(int);

private slots:

    void on_resetButton_clicked();

    void on_clearButton_clicked();

    void onNewPosition(RobotPosition pos);

    void onOriginPercentage(int percent);

private:
    Ui::PositionTrackerAdapter *ui;

    std::shared_ptr<PositionTracker> posTracker;

    std::vector<RobotPosition> positions;

    double minx, maxx, miny, maxy;
};

#endif // POSITIONTRACKERADAPTER_H

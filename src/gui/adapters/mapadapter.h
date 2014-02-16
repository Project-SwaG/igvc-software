#ifndef MAPADAPTER_H
#define MAPADAPTER_H

#include <QWidget>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <common/events/Event.hpp>
#include <intelligence/mapping/mapbuilder.h>

namespace Ui {
class MapAdapter;
}

/*!
 * \brief Widget for displaying Map data.
 * \author Matthew Barulic
 */
class MapAdapter : public QWidget
{
    Q_OBJECT
    
public:
    explicit MapAdapter(MapBuilder *mapper, QWidget *parent = 0);
    ~MapAdapter();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    
private slots:
    void on_scaleSlider_sliderMoved(int position);

    void on_homeButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::MapAdapter *ui;

    // pixels per meter
    double _scale;

    // on-screen displacement of map's origin from widget's center
    QPoint _origin;

    bool _isDragging;
    QPoint _previousMousePosition;

    pcl::PointCloud<pcl::PointXYZ>::Ptr _map;

    void setScale(int scale);

    void onNewMap(pcl::PointCloud<pcl::PointXYZ>::Ptr map);
    LISTENER(MapAdapter, onNewMap, pcl::PointCloud<pcl::PointXYZ>::Ptr)

    void paintEvent(QPaintEvent *);

    MapBuilder *_mapper;
};

#endif // MAPADAPTER_H

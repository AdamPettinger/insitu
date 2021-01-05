#ifndef insitu_INSITU_UTILS_HPP
#define insitu_INSITU_UTILS_HPP

// QT includes
#include <QWidget>
#include <QSet>
#include <QString>

// ROS includes
#include <ros/ros.h>
#include <ros/master.h>
#include <image_transport/image_transport.h>

// C++ includes
#include <vector>
#include <unordered_map>

namespace insitu {

QList<QString> getModeList();

QList<QString> getTopicList();

void addNamedWidget(std::string name, QWidget * widget);

QWidget * getNamedWidget(std::string name);

} // namespace insitu
#endif
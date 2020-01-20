#include "nav_msgs/Odometry.h"
#include "ros/ros.h"

#include "Poco/Net/Socket.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerParams.h"

#include <iostream>
#include <memory>

struct Bridge : public Poco::Net::TCPServerConnection {
  Bridge(const Poco::Net::StreamSocket &s);

  void run() override;

private:
  float x{}, y{}, theta{};

  ros::NodeHandle n;
  ros::Subscriber odometry_sub;
  void odometry_callback(const nav_msgs::Odometry::ConstPtr &msg);
  
  void receive(Poco::Net::StreamSocket& ss);
};
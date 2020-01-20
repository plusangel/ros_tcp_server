#include "bridge.h"
#include <thread>

void Bridge::receive(Poco::Net::StreamSocket& ss) {
  char buffer[256];
  int n{1};
  while (n > 0) {
    ss.sendBytes(buffer, n);
    n = ss.receiveBytes(buffer, sizeof(buffer));
    std::cout << "The server received: " << buffer << std::endl;
  }
}

void Bridge::run() {
  Poco::Net::StreamSocket &ss = socket();
  ros::Rate spin_rate(2);

  try {
    char ibuffer[256];

    // std::thread my_thread(&Bridge::receive, this, ss);
    std::thread receive_thread(&Bridge::receive, this, std::ref(ss));

    while (ros::ok()) {
      ss.sendBytes("buffer", sizeof("buffer"));
      // std::cout << b << std::endl;
      ros::spinOnce();
      spin_rate.sleep();
    }

  } catch (Poco::Exception &exc) {
    std::cerr << "NautilusServer: " << exc.displayText() << std::endl;
  }
}

Bridge::Bridge(const Poco::Net::StreamSocket &s)
    : Poco::Net::TCPServerConnection(s) {
  odometry_sub = n.subscribe("/odometry/filtered_odom", 10,
                             &Bridge::odometry_callback, this);
}

void Bridge::odometry_callback(const nav_msgs::Odometry::ConstPtr &msg) {

  x = msg->pose.pose.position.x;
  y = msg->pose.pose.position.y;
  theta = msg->pose.pose.position.x;

  ROS_INFO("listening odometry: (%f, %f, %f)", x, y, theta);
}

int main(int argc, char **argv) {

  ros::init(argc, argv, "bridge_node");

  Poco::Net::ServerSocket socket(1234);

  Poco::Net::TCPServerParams *pParams = new Poco::Net::TCPServerParams();
  pParams->setMaxThreads(4);
  pParams->setMaxQueued(4);
  pParams->setThreadIdleTime(100);

  Poco::Net::TCPServer myserver(
      new Poco::Net::TCPServerConnectionFactoryImpl<Bridge>(), socket, pParams);
  myserver.start();

  while (ros::ok())
    ;

  return 0;
}
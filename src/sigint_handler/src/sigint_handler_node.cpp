#include <ros/ros.h>
#include <signal.h>

void sigint_handler(int sig)
{
	ROS_INFO("If this printed you got your piece of code");
	ros::shutdown();
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "sigint_handler_name", ros::init_options::NoSigintHandler);
	ros::NodeHandle nh;
	signal(SIGINT, sigint_handler);
	ros::spin();
	return 0;
}

#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <laser_geometry/laser_geometry.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/Vector3.h>
#include <std_msgs/Float64.h>
#include <visualization_msgs/Marker.h>
#include <signal.h>

#define pi 3.141592653589793

void signint_handler(int sig)
{
	ROS_INFO("shutting down...!!!");	
	ros::shutdown();
}

//class magnetometer_heading direction
class direction_follower_test
{
	private:
		ros::NodeHandle n_sub;
		ros::NodeHandle n_pub;
		ros::Subscriber vector_sub;
		ros::Publisher compass_hdg_pub;
		double time;
		std_msgs::Float64 compass_hd;
	public:
		direction_follower_test()
		{
			time=ros::Time::now().toSec();
			compass_hd.data = 180.0;
			vector_sub = n_sub.subscribe("vector_pwm_arduino", 1000, &direction_follower_test::scan_callback, this);
			compass_hdg_pub = n_pub.advertise<std_msgs::Float64>("/mavros/global_position/compass_hdg", 1000);
			signal(SIGINT, signint_handler);
		}
		void scan_callback(const geometry_msgs::Vector3::ConstPtr& pwm_vector_in);
		void run();
};

void direction_follower_test::scan_callback(const geometry_msgs::Vector3::ConstPtr& pwm_vector_in)
{
	const double couple_distance = 0.15;	//in metres
	const double max_speed = 1;		//in metres/sec
	double omega =((pwm_vector_in->y-pwm_vector_in->x)*max_speed)/(255.0*couple_distance/2.0);
	double delt=ros::Time::now().toSec()-time;
	double time=delt+time;
	compass_hd.data += (omega*delt*180.0)/pi;
	std_msgs::Float64 compass_hd;
	compass_hd.data = compass_hd.data - 360.0*((int)compass_hd.data/360);
}

void direction_follower_test::run()
{
	ros::Rate r(10);
	while(ros::ok())
	{
		compass_hdg_pub.publish(compass_hd);
		ros::spinOnce();
		r.sleep();
	}
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "direction_follower_test_node");
	direction_follower_test obj;
	obj.run();
	return 0;
}

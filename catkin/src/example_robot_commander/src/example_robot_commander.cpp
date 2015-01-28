#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

ros::Publisher cmd_publisher;
//create a variable of type "Twist", as defined in: /opt/ros/hydro/share/geometry_msgs
// any message published on a ROS topic must have a pre-defined format, so subscribers know how to
// interpret the serialized data transmission
geometry_msgs::Twist twist_cmd;

void move(float distance, float speed, ros::Rate& sleep_timer) {
	twist_cmd.linear.x = speed;
    int niters = distance / speed * 100.0; //1200 iters at 100Hz is 12 seconds;
    //iteration counter; at 10ms/iteration, and 0.4m/sec, expect 4mm/iter
    // should move by 4m over 10 sec
    for (int i = 0; i < niters; i++)
    {
        cmd_publisher.publish(twist_cmd); // really, should only need to publish this once, but no hard done
        sleep_timer.sleep(); // sleep for (remainder of) 10ms
    }
    twist_cmd.linear.x = 0;
}

void turn(int niters, float speed, ros::Rate& sleep_timer) {
	twist_cmd.angular.z = speed;
    for (int i = 0; i < niters; i++)
    {
        cmd_publisher.publish(twist_cmd); // really, should only need to publish this once, but no hard done
        sleep_timer.sleep(); // sleep for (remainder of) 10ms
    }
    twist_cmd.angular.z = 0;
}

void turn_90_deg_right(ros::Rate& sleep_timer) {
	turn(500, -0.314, sleep_timer);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "robot0_commander"); // name of this node
    ros::NodeHandle nh; // two lines to create a publisher object that can talk to ROS
    //stdr "robot0" is expecting to receive commands on topic: /robot0/cmd_vel
    // commands are of type geometry_msgs/Twist, but they use only velocity in x dir and
    //  yaw rate in z-dir; other 4 fields will be ignored
    cmd_publisher = nh.advertise<geometry_msgs::Twist>("/robot0/cmd_vel", 1);

    //let's make a 100Hz timer
    ros::Rate sleep_timer(100);

    // look at the components of a message of type geometry_msgs::Twist by typing:
    // rosmsg show geometry_msgs/Twist
    // It has 6 fields.  Let's fill them all in with some data:
    twist_cmd.linear.x = 0.0;
    twist_cmd.linear.y = 0.0;
    twist_cmd.linear.z = 0.0;
    twist_cmd.angular.x = 0.0;
    twist_cmd.angular.y = 0.0;
    twist_cmd.angular.z = 0.0;

    // timer test...print out a message every 1 second
    ROS_INFO("count-down");
    for (int j = 3; j > 0; j--)
    {
        ROS_INFO("%d", j);
        for (int i = 0; i < 100; i++)
            sleep_timer.sleep();
    }

    // Setup for moving forward
    move(5, 0.4, sleep_timer);

    // Setup for turning right
    ROS_INFO("Time to rotate negative");
    turn_90_deg_right(sleep_timer);

    ROS_INFO("Let's move down the hallway a little bit");
    move(12.5, 0.4, sleep_timer);

    ROS_INFO("Let's look down the big long hallway");
    turn_90_deg_right(sleep_timer);

    ROS_INFO("my work here is done");
    while (ros::ok())
    {
        twist_cmd.linear.x = 0.0;
        twist_cmd.angular.z = 0;
        cmd_publisher.publish(twist_cmd); // and halt
    }


    return 0;
}
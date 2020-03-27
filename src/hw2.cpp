// include ros library
#include "ros/ros.h"

// include msg library
#include <turtlesim/Pose.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Point.h>

// include math 
#include <math.h>

using namespace std;

turtlesim::Pose pose;
geometry_msgs::Twist vel_msg;
geometry_msgs::Point goal_point;
//Define a data structure to 3D
struct XYZ{
  float x;
  float y;
  float z;
};
//Declare a variable.Its name is pos_err with XYZ data type
struct XYZ pos_err;

// declare call back function(call back the pose of robot)
void pos_cb(const turtlesim::Pose::ConstPtr& msg)
{
  pose = *msg;
} 

int main(int argc, char **argv)
{
  ros::init(argc, argv, "tutorial_1");
  ros::NodeHandle n;

  // declare publisher & subscriber
  ros::Subscriber pos_sub = n.subscribe<turtlesim::Pose>("turtle1/pose", 10, pos_cb);
  ros::Publisher turtlesim_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);
  //input your desired position
  ROS_INFO("Please input (x,y). x>0,y>0");
  cout<<"desired_X:";
  cin>>goal_point.x;
  cout<<"desired_Y:";
  cin>>goal_point.y;
  // setting frequency as 10 Hz
  ros::Rate loop_rate(10);

  int count = 0;
  while (ros::ok()){

    printf("\ncount : %d\n",count);
    printf("goal x : %f \t y : %f\n",goal_point.x,goal_point.y);
    printf("pose x : %f \t y : %f\n",pose.x,pose.y);

    // Calculate position error(feedback term)
    pos_err.x = goal_point.x - pose.x;
    pos_err.y = goal_point.y - pose.y;
    
    printf("pose_err x : %f \t y : %f\n",pos_err.x,pos_err.y);
    /*Your error-driven controller design*/
    /*float Eg, Ag;
     goal_point.z = goal_point.y - pose.y / goal_point.x - pose.x ;
     Eg = goal_point.z;
     Ag = atan (Eg) * 180 / 3.14159265;
     printf("angle =: %f \n",Ag);*/
      float D;
      D = sqrt(pos_err.x * pos_err.x + pos_err.y * pos_err.y);
      printf("distance = : %f \n",D);
     
      float T;
     T = pose.theta * 180 / 3.11;


     float E, A, C;
     pos_err.z = pos_err.y / pos_err.x ;
     E = pos_err.z;
     A = atan (E) * 180 / 3.14159265;
     if(pos_err.x > 0 && pos_err.y > 0){
       A = A;
     }
     if(pos_err.x < 0 && pos_err.y > 0){
       A = A + 180;
     }
     if(pos_err.x < 0 && pos_err.y < 0){
       A = A - 180;
     }
     if(pos_err.x > 0 && pos_err.y < 0){
       A = A ;
     }

     C = abs(A - T);

     printf("theta = : %f \t",T);
     printf("angle = : %f \t",A);
     printf("compare = : %f \n",C);

     int KL, KA;
     KL = 1.8;
     KA = 1.3;
     vel_msg.linear.x =  D / 3;
     vel_msg.angular.z =  C / 40 ;
     if(C <= 20 && D >= 1.5){
       vel_msg.linear.x = KL * vel_msg.linear.x;
       vel_msg.angular.z = 0.000001;
     }

     else if(C >= 20 && D <= 1.5){
       vel_msg.linear.x  =  KL * 0.000001;
       vel_msg.angular.z = KA * vel_msg.angular.z;
     }


       else if(C >= 280 && D >= 1.5){
       vel_msg.linear.x  =  KL * vel_msg.linear.x ;
       vel_msg.angular.z = KA * vel_msg.angular.z / 20;
     }

     /*else if(C <= 10){
       vel_msg.linear.x =  vel_msg.linear.x / 20000;
     }*/

     else if(C <= 10 && D <= 1.5){
       vel_msg.linear.x  =  KL * vel_msg.linear.x / 20;
       vel_msg.angular.z =   KA * vel_msg.angular.z / 20;
     }
    
     /*else if(D <= 0.7){
       vel_msg.angular.z =  vel_msg.angular.z / 20000;
     } */
     else{
     vel_msg.linear.x = KL * vel_msg.linear.x;
     vel_msg.angular.z = KA * vel_msg.angular.z;
     }
     

 

     printf("vel_linear :  %f\n",vel_msg.linear.x);
     printf("vel_angular :  %f\n",vel_msg.angular.z);

    turtlesim_pub.publish(vel_msg);

    if(D <= 0.08 )

      break;

    count ++;
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}




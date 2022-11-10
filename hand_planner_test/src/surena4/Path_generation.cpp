#include "S5mod_right_hand.h"
#include "../Eigen/Dense"
#include "../Eigen/Geometry"
#include "../Eigen/QuadProg.h"
#include "../Eigen/testQPsolvers.hpp"
#include "../Eigen/eiquadprog.hpp"
#include "../Eigen/Core"
#include "../Eigen/Cholesky"
#include "../Eigen/LU"
#include <iostream>
#include <vector>
#include "fstream"
#include <string>
//#include "ros/ros.h"
//#include<std_msgs/Float32MultiArray.h>
//#include<std_msgs/Int32MultiArray.h>
#include"MinimumJerkInterpolation.h"

using namespace  std;
using namespace  Eigen;


    int main() {
    MinimumJerkInterpolation coef_generator;
    double a=0.02 ;
    double b=2*a ;


    MatrixXd P_x_r(1,3);      
    MatrixXd V_x_r(1,3);      
    MatrixXd A_x_r(1,3); 
    MatrixXd P_y_r(1,3);   
    MatrixXd V_y_r(1,3);      
    MatrixXd A_y_r(1,3);      
    MatrixXd P_z_r(1,3);      
    MatrixXd V_z_r(1,3);      
    MatrixXd A_z_r(1,3);

    MatrixXd X_coef_r;        
    MatrixXd Y_coef_r;        
    MatrixXd Z_coef_r; 
    VectorXd P_r(3); VectorXd V_r(3);
    
    VectorXd q_ra(7);
    q_ra<<0,0,0,-10*M_PI/180,0,0,0;
    VectorXd q;
    q.resize(7,1);
    VectorXd q_end;
    q_end.resize(7,1);

    right_hand hand_funcs;
    right_hand hand_r;

    VectorXd r_right_palm(3);
    MatrixXd R_target_r;
    R_target_r.resize(3,3);
    //R_target_r=hand_funcs.rot(2,-45*M_PI/180,3);

    
    //R_target_r.fill(0);

    MatrixXd qref;
    qref.resize(7,1800);

    VectorXd V_ra(3);

    double time_r;
    MatrixXd t_r(1,3);
    int count = 0; 
    time_r=count*.005;

    t_r<<0,2,4;
    double T=4;

    VectorXd r_target_r(3); 
    VectorXd r_middle_r(3); 
    VectorXd r_start_r(3); 


    right_hand hand0_r( q_ra, r_target_r,  R_target_r,0,0);
 

    r_start_r=hand0_r.r_right_palm;

    r_middle_r<<-0.005,-0.058,-0.344  ; ///lift up
    r_target_r<<0.226,-0.017,-0.31;
    R_target_r=hand_funcs.rot(3,-90*M_PI/180,3)*hand_funcs.rot(1,90*M_PI/180,3);

    while (time_r<9)
    {
    
    P_x_r<< r_start_r(0),r_middle_r(0),r_target_r(0);
    P_y_r<< r_start_r(1),r_middle_r(1),r_target_r(1);
    P_z_r<< r_start_r(2),r_middle_r(2),r_target_r(2);


    V_x_r<<0,INFINITY,0;
    V_y_r<<0,INFINITY,0;
    V_z_r<<0,INFINITY,0;
    A_x_r<<0,INFINITY,0;
    A_y_r<<0,INFINITY,0;
    A_z_r<<0,INFINITY,0;

    X_coef_r=coef_generator.Coefficient(t_r,P_x_r,V_x_r,A_x_r);
    Y_coef_r=coef_generator.Coefficient(t_r,P_y_r,V_y_r,A_y_r);
    Z_coef_r=coef_generator.Coefficient(t_r,P_z_r,V_z_r,A_z_r);

   if(time_r<t_r(1)&& time_r>=t_r(0)){
    P_r<<   coef_generator.GetAccVelPos(X_coef_r.row(0),time_r,t_r(0),5)(0,0),
            coef_generator.GetAccVelPos(Y_coef_r.row(0),time_r,t_r(0),5)(0,0),
            coef_generator.GetAccVelPos(Z_coef_r.row(0),time_r,t_r(0),5)(0,0);
    V_r<<   coef_generator.GetAccVelPos(X_coef_r.row(0),time_r,t_r(0),5)(0,1),
            coef_generator.GetAccVelPos(Y_coef_r.row(0),time_r,t_r(0),5)(0,1),
            coef_generator.GetAccVelPos(Z_coef_r.row(0),time_r,t_r(0),5)(0,1);

    hand_r.update_right_hand(q_ra,V_r,r_middle_r,R_target_r);
    r_right_palm=hand_r.r_right_palm;
    hand_r.doQP(q_ra);
    q_ra=hand_r.q_next;
   }
    else if(time_r<t_r(2)&& time_r>=t_r(1)){
    P_r<<   coef_generator.GetAccVelPos(X_coef_r.row(1),time_r,t_r(1),5)(0,0),
            coef_generator.GetAccVelPos(Y_coef_r.row(1),time_r,t_r(1),5)(0,0),
            coef_generator.GetAccVelPos(Z_coef_r.row(1),time_r,t_r(1),5)(0,0);
    V_r<<   coef_generator.GetAccVelPos(X_coef_r.row(1),time_r,t_r(1),5)(0,1),
            coef_generator.GetAccVelPos(Y_coef_r.row(1),time_r,t_r(1),5)(0,1),
            coef_generator.GetAccVelPos(Z_coef_r.row(1),time_r,t_r(1),5)(0,1);

    hand_r.update_right_hand(q_ra,V_r,r_target_r,R_target_r);
    r_right_palm=hand_r.r_right_palm;
    hand_r.doQP(q_ra);
    q_ra=hand_r.q_next;

   }
   
   else if (time_r>=5 &&time_r<=9)
   {
   // double L=1.5*M_PI*(a+b/2)/2;
    //double L=2*M_PI*(a+b/2)/3;
    double L=0.628;
    //cout<< L <<endl;
    double time_w=time_r -5;
    double v=30*L/pow(T,3)*pow(time_w,2)-60*L/pow(T,4)*pow(time_w,3)+30*L/pow(T,5)*pow(time_w,4);
    double P=10*L/pow(T,3)*pow(time_w,3)-15*L/pow(T,4)*pow(time_w,4)+6*L/pow(T,5)*pow(time_w,5);
/*
      if (time_r<=7){
     V_ra<<0,v*sin(3*P/L*M_PI),v*cos(3*P/L*M_PI);
      }

      else{
     V_ra<<0,-v*cos(-3*(P-L/2)/L*M_PI),v*sin(-3*(P-L/2)/L*M_PI);

      }
      */
    V_ra<<0,-v*sin(2*P/L*M_PI),v*cos(2*P/L*M_PI);
    hand_r.update_right_hand(q_ra,V_ra,r_target_r,R_target_r);
    hand_r.doQP(q_ra);
    q_ra=hand_r.q_next;

   }
 
    q_end=q_ra; 
    qref.block(0,count,7,1)=q_end;
  

    count++;
    time_r=(count)*.005;


    
    q[0]=q_ra(0);
    q[1]=q_ra(1);
    q[2]=q_ra(2);
    q[3]=q_ra(3);
    q[4]=q_ra(4);
    q[5]=q_ra(5);
    q[6]=q_ra(6);

     ofstream fw("/home/cast/Projects/pybullet/drawcircle5.txt", std::ofstream::out);
    //check if file was successfully opened for writing
    if (fw.is_open())
    {
    //store array contents to text file
    for (int i = 0; i < 1800; i++) {
        for (int j = 0; j <7; j++){
        fw << qref(j,i) << "\n";
    }
    }
    fw.close();
    }

  };   
     return 0 ;

    }
  /*
    trajectory_data.data.clear();
        for (int i = 0; i <= 7; ++i) {
            trajectory_data.data.push_back(q[i]);

        }
        trajectory_data_pub.publish(trajectory_data);

        ros::spinOnce();
        loop_rate.sleep();
        };
        };
        

   return 0 ;

  };   
*/
//g++ Path_generation.cpp S5mod_right_hand.cpp MinimumJerkInterpolation.cpp
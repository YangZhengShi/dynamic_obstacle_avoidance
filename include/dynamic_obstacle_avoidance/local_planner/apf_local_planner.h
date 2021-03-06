#pragma once

#include "dynamic_obstacle_avoidance/local_planner/local_planner_interface.h"
#include "dynamic_obstacle_avoidance/local_planner/robot_jacobian.h"
#include <list>
#include <sensor_msgs/JointState.h>


struct ApfParam
{
    int max_iter;
    int iter;
    double safety_distance;
    // 目标
    double target_zeta;
    double target_alfa;
    double target_alfa_rot;
    // 工作场的影响因子
    double dist_att;
    double dist_att_config;
    // 误差
    double target_err;
    double target_err_ori;

    // 轨迹
    double trajectory_zeta;
    double tra_alfa;
    double tra_alfa_rot;
    double tra_dist_att;
    double tra_dist_att_config;

    double global_trajectory_att_outer;
    double global_trajectory_att_inner;

    // 障碍物
    // link
    std::vector<double> obs_eta;
    // std::vector<double> obs_rep;
    std::vector<double> obs_alfa;

};


class ApfLocalPlanner : public ILocalPlanner
{
public:
    ApfLocalPlanner(ros::NodeHandle* nh, moveit::planning_interface::MoveGroupInterface* group);
    ~ApfLocalPlanner();

    int execute() override;

private:
    //各种计算距离的函数
    double getDistance(const geometry_msgs::PoseStamped& pose1, const geometry_msgs::PoseStamped& pose2);
    double getDistance(const geometry_msgs::PoseStamped& pose1, const Eigen::Isometry3d& pose2);
    double getDistance(const geometry_msgs::PoseStamped& pose1, const std::vector<double>& pose2);
    double getDistance(const Eigen::Isometry3d& pose1, const std::vector<double>& pose2);

    // 获取障碍物距离的函数
    void getObstacleDistance();

    // 是否有符合轨迹跟随器的点
    bool getNearbyPoint();

    void fill(const geometry_msgs::PoseStamped& pose, std::vector<double>& positin);

    // 执行全规划的轨迹
    int executeGlobalTra();

    // 执行局部规划的轨迹
    int executeApf();

    // 获取所有的作用力
    int getJointForce();

    // 获取全局轨迹跟随器的力
    int getTrajectoryForce();

    // 获取目标跟随器的力
    int getTargetPoseForce();

    // 获取障碍物的斥力
    int getObstacleForce();

    // 获取所有link的Pose
    int getLinkPose();

    // 获取姿态误差
    void getOrienError(const geometry_msgs::PoseStamped& p, const Eigen::Isometry3d& p2, Eigen::Vector3d& euler_angle);
    Eigen::Vector3d getOrienError(const geometry_msgs::PoseStamped& p, const Eigen::Isometry3d& p2);

    // 获取位置的总误差
    double finishPosition(const geometry_msgs::PoseStamped& s, const Eigen::Isometry3d& t);
    // 获取姿态的总误差
    double finishOrien(const geometry_msgs::PoseStamped& s, const Eigen::Isometry3d& t);
    // 获取角度的总误差
    double finishJoint(std::vector<double> current_joint, std::vector<double> target_joint);

    // std::vector<double> getVelocity(std::vector<double> current_joint, const std::vector<double>& target_joint, std::vector<double> current_joint_velocity);

    // 获取设置参数
    void setParam();

    void JointStateCB(const sensor_msgs::JointStateConstPtr& msg);

    void setTrajectoryIndex(int index, std::list<Eigen::Isometry3d>::iterator& iter);

    // 速度转移矩阵
    void tr2jac(const Eigen::Isometry3d& T);

    // 获取两个link的齐次矩阵
    Eigen::Isometry3d getTransform(const std::string& target_link, const std::string& source_link);


    double getRandon(double a, double b);

    std::vector<double> m_current_joint_value;
    std::vector<double> m_current_joint_velocity;

private:
    ApfParam* m_apf;

    // 迭代器
    std::list<Eigen::Isometry3d>::iterator m_tcp_trajectory_iter;
    int m_trajectory_point_index;

    // 所有的力矩
    Eigen::Matrix<double, 6, 1> m_joint_att_force_p;
    Eigen::Matrix<double, 6, 1> m_joint_att_force_w;
    Eigen::Matrix<double, 6, 1> m_joint_att_trajectory_p;
    Eigen::Matrix<double, 6, 1> m_joint_att_trajectory_w;
    Eigen::Matrix<double, 6, 1> m_joint_rep_force;
    Eigen::Matrix<double, 6, 1> m_all_force;
    
    Eigen::Matrix<double, 6, 6> m_volecity_transform;

    std::vector<std::string> m_link_name;
    std::vector<geometry_msgs::PoseStamped> m_link_pose_vec;
    geometry_msgs::PoseStamped m_current_target_link_pose;

    Eigen::Matrix<double, 3, 1> m_current_p;
    std::vector<double> m_current_w;

    RobotJacobian* m_robot_jacobian;
    
    std::vector<Eigen::MatrixXd> m_jacobian_p;
    Eigen::MatrixXd m_target_jacobian_w;
    Eigen::MatrixXd m_target_jacobian_p;
    std::vector<Eigen::MatrixXd> m_all_jacobian;

    ros::Subscriber m_joint_state_sub;

    bool m_is_stop;
};
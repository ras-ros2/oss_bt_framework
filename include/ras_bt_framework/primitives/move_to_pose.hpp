#pragma once

#include "ras_bt_framework/PrimitiveBehavior.hpp"
#include "rclcpp/rclcpp.hpp"
#include "ras_interfaces/srv/pose_req.hpp" 
#include "geometry_msgs/msg/pose.hpp"
#include "rclcpp/logging.hpp"  // For ROS_INFO


namespace BT
{
template <>
inline geometry_msgs::msg::Pose convertFromString(StringView str)
{
    // Expect a string like: "0.2,0.25,0.1,0.0,0.0,0.0,1.0"
    auto parts = BT::splitString(str, ',');
    if (parts.size() != 7)
    {
        throw BT::RuntimeError("invalid input string for geometry_msgs::msg::Pose: ", str);
    }

    geometry_msgs::msg::Pose pose;
    pose.position.x = convertFromString<double>(parts[0]);
    pose.position.y = convertFromString<double>(parts[1]);
    pose.position.z = convertFromString<double>(parts[2]);
    pose.orientation.x = convertFromString<double>(parts[3]);
    pose.orientation.y = convertFromString<double>(parts[4]);
    pose.orientation.z = convertFromString<double>(parts[5]);
    pose.orientation.w = convertFromString<double>(parts[6]);
    
    return pose;
}
}


namespace ras_bt_framework
{

class MoveToPose : public PrimitiveBehavior {
public:
    MoveToPose(const std::string& name, const BT::NodeConfig& config)
    : PrimitiveBehavior(name, config)
    {
        // Initialize other members here, like the ROS node
        node_ = rclcpp::Node::make_shared("move_to_pose_node");
        move_to_pose = node_->create_client<ras_interfaces::srv::PoseReq>("/create_traj");
    }

    ~MoveToPose() {}
    
    static BT::PortsList providedPorts()
    {
        return { BT::InputPort<geometry_msgs::msg::Pose>("pose") };
    }
    
   virtual BT::NodeStatus tick() override {
    std::cout << ("MoveToPose") << std::endl;

    auto msg = getInput<geometry_msgs::msg::Pose>("pose");

    auto request = std::make_shared<ras_interfaces::srv::PoseReq::Request>();

    // Direct assignment without using 'expected'
    request->object_pose = msg.value();

    request->type = "beaker";  // Corrected semicolon

    auto result_future = move_to_pose->async_send_request(
            request, std::bind(&MoveToPose::move_to_pose_response, this,
                                std::placeholders::_1));  

    if (rclcpp::spin_until_future_complete(node_, result_future) ==
    rclcpp::FutureReturnCode::SUCCESS)
    {
    return BT::NodeStatus::SUCCESS;
    }
    
   }



    void move_to_pose_response(rclcpp::Client<ras_interfaces::srv::PoseReq>::SharedFuture future) {
        // Handle the response if needed
    }

private:
    rclcpp::Node::SharedPtr node_;  // Node shared pointer to create clients
    rclcpp::Client<ras_interfaces::srv::PoseReq>::SharedPtr move_to_pose;
};

}
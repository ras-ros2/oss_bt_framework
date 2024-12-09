#pragma once

#include "ras_bt_framework/PrimitiveBehavior.hpp"
#include "ras_interfaces/srv/read_black.hpp"

namespace ras_bt_framework
{

class ReadInputPort : public PrimitiveBehavior
{
public:
  ReadInputPort(const std::string& name, const BT::NodeConfig& config)
    : PrimitiveBehavior(name, config)
  {
    node_ = rclcpp::Node::make_shared("input_port_node");
    blackboard_client = node_->create_client<ras_interfaces:srv:ReadBlack>("/read_blackboard");
  }


  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<std::string>("blackboard") };
  }

  virtual BT::NodeStatus tick() override
  {
    auto msg = getInput<std::string>("blackboard");

    auto request = std::make_shared<ras_interfaces::srv::ReadBlack::Request>();

    request->blackboard = msg;

    auto result_future = blackboard_client->async_send_request(
        request, std::bind(&ReadInputPort::blackboard_response, this,
                            std::placeholders::_1));  

    return BT::NodeStatus::SUCCESS;
  }
   
    void blackboard_response(rclcpp::Client<ras_interfaces::srv::ReadBlack>::SharedFuture future) {}

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Client<ras_interfaces::srv::ReadBlack>::SharedPtr blackboard_client;
};
    
}
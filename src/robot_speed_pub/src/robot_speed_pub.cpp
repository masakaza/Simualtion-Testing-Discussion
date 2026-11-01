#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include<queue>

class RobotSpeedPublisher : public rclcpp::Node
{
public:
  RobotSpeedPublisher(): Node("robot_speed_publisher"),speed_publisher_(create_publisher<std_msgs::msg::Int32>("robot_speed", 10))
  {
    allspeed();
    publish_timer_ = create_wall_timer(std::chrono::milliseconds(1000),std::bind(&RobotSpeedPublisher::publish_speed, this));
  }

private:
  void allspeed(){
    speed_.push(2);
    speed_.push(5);
    speed_.push(4);
    speed_.push(8);
    speed_.push(3);
    speed_.push(6);
  }
  void publish_speed()
  {
    int temp=speed_.front();
    std_msgs::msg::Int32 message;
    message.data = temp;
    speed_.pop();
    speed_.push(temp);
    speed_publisher_->publish(message);
    RCLCPP_INFO(get_logger(), "Publishing speed: %d", message.data);
  }

  std::queue<int> speed_;
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr speed_publisher_;
  rclcpp::TimerBase::SharedPtr publish_timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RobotSpeedPublisher>());
  rclcpp::shutdown();
  return 0;
}

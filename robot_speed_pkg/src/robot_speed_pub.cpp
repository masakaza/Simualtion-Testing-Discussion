#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <queue>

class SpeedPublisher : public rclcpp::Node
{
public:
    SpeedPublisher(std::string name) : Node(name)
    {
        RCLCPP_INFO(this->get_logger(), "%s已启动", name.c_str());
        speedpublisher_ = this->create_publisher<std_msgs::msg::Int32>("robot_speed", 10);
        timer_ = this->create_wall_timer(std::chrono::seconds(1), 
            std::bind(&SpeedPublisher::send_speed_, this));
    }
    void init_all_speed()
    {
        all_speed_.push(2);
        all_speed_.push(5);
        all_speed_.push(4);
        all_speed_.push(8);
        all_speed_.push(3);
        all_speed_.push(6);
    }
private:
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr speedpublisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::queue<int> all_speed_;
    void send_speed_()
    {
        int temp = all_speed_.front();
        all_speed_.push(temp);
        all_speed_.pop();
        auto msg = std_msgs::msg::Int32();
        msg.data = temp;
        RCLCPP_INFO(this->get_logger(), "%i", msg.data);
        speedpublisher_->publish(msg);
    }
};
int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SpeedPublisher>("speed_publisher");
    node->init_all_speed();
    rclcpp::spin(node);
    rclcpp::shutdown();
}

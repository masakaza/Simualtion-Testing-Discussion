#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <queue>
#include <vector>

template<typename T>
T absoluteDifference(T a, T b)
{
    return (a > b) ? (a - b) : (b - a);
}

class SpeedMonitor : public rclcpp::Node
{
public:
    SpeedMonitor(std::string name): Node(name)
    {
        RCLCPP_INFO(this->get_logger(), "%s已启动", name.c_str());
        speedsubsriber_ = this->create_subscription<std_msgs::msg::Int32>("robot_speed", 10, 
            std::bind(&SpeedMonitor::speed_callback, this, std::placeholders::_1));
    }
private:
    void speed_callback(const std_msgs::msg::Int32 &msg)
    { 
        std::cout << std::endl;
        RCLCPP_INFO(this->get_logger(), "Receive Speed: %i", msg.data);
        auto temp = msg.data;
        speed_.push(temp);
        show_speed_();
        check_difference_(process_speed_, previous_speed_);
        previous_speed_ = process_speed_;
    }

    void show_speed_()
    {
        if(speed_.size() > 1)
        {
            process_speed_ = speed_.front();
            speed_.pop();
            std::cout << "Processing Speed: " << process_speed_ << std::endl;
            std::cout << "Previous Speed: " << previous_speed_ << std::endl;
        }
        else
        {
            std::cout << "waitting for next command..." << std::endl;
        }
    }

    void check_difference_(int pro_speed, int pre_speed)
    {
        int result = absoluteDifference<int>(pro_speed, pre_speed);
        difference_.push_back(result);
        if(difference_.size() > 4)
            difference_.erase(difference_.begin());
        std::cout << "Speed Diffrence: " << result << std::endl; 
        if(result < 3)
        {
            std::cout << "State: NORMAL" << std::endl;
        }
        else
        {
            std::cout << "State: WARNING" << std::endl;
        }

        warning_num_ = 0;
        for(std::vector<int>::iterator it = difference_.begin();it < difference_.end();it++)
        {
            if(*it >= 3)
                warning_num_++;
        }
        if(warning_num_ > 1)
            std::cout << "System State: UNSTABLE" << std::endl;
        else    
            std::cout << "System State: STABLE" << std::endl;
    }

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr speedsubsriber_;
    std::queue<int> speed_;
    std::vector<int> difference_;
    int warning_num_;
    int process_speed_;
    int previous_speed_ = 0;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SpeedMonitor>("speed_monitor");
    rclcpp::spin(node);
    rclcpp::shutdown();
}

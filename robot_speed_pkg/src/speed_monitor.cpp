#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
using namespace std;

vector<int> sub_v;

template <typename T>
T absoluteDifference(T a, T b)
{
    return (a > b) ? (a - b) : (b - a);
}

void checkSpeedChange(int currentSpeed, int previousSpeed)
{
    bool warn = 0;
    int difference_speed = absoluteDifference(currentSpeed, previousSpeed);
    warn = difference_speed >= 3;

    sub_v.push_back(difference_speed);
    if (sub_v.size() > 4)
    {
        sub_v.erase(sub_v.begin());
    }

    cout << "Speed Difference: " << difference_speed << endl;
    cout << "State: " << (warn ? "WARNING" : "NORMAL") << endl;

    warn = 0;
}

class speed_monitor : public rclcpp::Node
{
private:
    queue<int> sub_q;
    queue<int> speed_q;
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
    int warn_count = 0;
    int front = 0;
    int end = 0;
    bool warn = 0;
    bool sys_warn = 0;

    void sub_cb(const std_msgs::msg::Int32 &msg)
    {
        RCLCPP_INFO(this->get_logger(), "Receive Speed: %d", msg.data);
        sub_q.push(msg.data);
        if (sub_q.size() > 1)
        {
            RCLCPP_INFO(this->get_logger(), "Processing Speed: %d", sub_q.front());
            sub_q.pop();
        }
        else
        {
            speed_q.push(0);
            RCLCPP_INFO(this->get_logger(), "Waiting for next command...");
        }
        speed_q.push(msg.data);

        if (speed_q.size() >= 3)
        {
            auto temp = speed_q.front();
            RCLCPP_INFO(this->get_logger(), "Previous Speed: %d", speed_q.front());
            speed_q.pop();

            thread t(checkSpeedChange, temp, speed_q.front());

            t.join();
            warn_count = 0;
            for (auto i = sub_v.begin(); i != sub_v.end(); i++)
            {
                if (*i >= 3)
                {
                    warn_count++;
                }
            }

            sys_warn = warn_count >= 2;

            RCLCPP_INFO(this->get_logger(), "System State: %s", (sys_warn ? "UNSTABLE" : "STABLE"));
        }
    }

public:
    speed_monitor(string name) : Node(name)
    {
        RCLCPP_INFO(this->get_logger(), "接收节点创建");
        subscription_ = this->create_subscription<std_msgs::msg::Int32>("robot_speed", 10, bind(&speed_monitor::sub_cb, this, placeholders::_1));
    }
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(make_shared<speed_monitor>("speed_monitor_node"));
    rclcpp::shutdown();
    return 0;
}
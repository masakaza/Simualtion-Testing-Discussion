#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
using namespace std;

queue<int> sub_q;
vector<int> sub_v;
queue<int> speed_q;

template <typename T>
T absoluteDifference(T a, T b)
{
    T delta_v = a - b;
    if (delta_v < 0)
    {
        delta_v = -delta_v;
    }

    return delta_v;
}

// vector<int> v(5);

void checkSpeedChange(int currentSpeed, int previousSpeed)
{
    bool warn = 0;
    int difference_speed = absoluteDifference(currentSpeed, previousSpeed);

    if (difference_speed >= 3)
    {
        warn = 1;
    }

    sub_v.push_back(difference_speed);
    if (sub_v.size() > 4)
    {
        sub_v.erase(sub_v.begin());
    }

    // RCLCPP_INFO(this->get_logger(),"Speed Difference: %d",difference_speed);
    // RCLCPP_INFO(this->get_logger(),"State: %s",(warn ? "WARNING":"NORMAL"));
    cout << "Speed Difference: " << difference_speed << endl;
    cout << "State: " << (warn ? "WARNING" : "NORMAL") << endl;

    warn = 0;
    // warn_count = 0;
}

class speed_monitor : public rclcpp::Node
{
private:
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

            // int difference_speed = absoluteDifference(temp,speed_q.front());

            // if (difference_speed >= 3)
            // {
            //     warn = 1;
            // }

            // sub_v.push_back(difference_speed);
            // if (sub_v.size() > 4)
            // {
            //     sub_v.erase(sub_v.begin());
            // }

            t.join();
            for (auto i = sub_v.begin(); i != sub_v.end(); i++)
            {
                if (*i >= 3)
                {
                    warn_count++;
                }

                if (warn_count >= 2)
                {
                    sys_warn = 1;
                }
                else
                {
                    sys_warn = 0;
                }
            }

            // RCLCPP_INFO(this->get_logger(),"Speed Difference: %d",difference_speed);
            // RCLCPP_INFO(this->get_logger(),"State: %s",(warn ? "WARNING":"NORMAL"));
            RCLCPP_INFO(this->get_logger(), "System State: %s", (sys_warn ? "UNSTABLE" : "STABLE"));
            // warn = 0;
            warn_count = 0;
        }
    }

public:
    speed_monitor() : Node("speed_monitor_node")
    {
        // speed_q.push(0);
        RCLCPP_INFO(this->get_logger(), "接收节点创建");
        subscription_ = this->create_subscription<std_msgs::msg::Int32>("robot_speed", 10, bind(&speed_monitor::sub_cb, this, placeholders::_1));
    }
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(make_shared<speed_monitor>());
    rclcpp::shutdown();
    return 0;
}
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include <thread> 

using namespace std;
vector<int> v1;
queue<int> q1;
queue<int> q2;

template<typename T>
T absoluteDifference(T a, T b)
{
    T delta_v = a - b;
    if(delta_v < 0)
    {
        delta_v = b - a;
    }
    return delta_v;
}

void changeSpeed(int currentspeed, int previousspeed)
{
    int diff = absoluteDifference(currentspeed, previousspeed);
    if(diff >= 3){
        cout << "WARNING" << endl;
    }
    else{
        cout << "NORMAL" << endl;
    }
}

class Speed_monitor:public rclcpp::Node
{
public:
    Speed_monitor():Node("speed_monitor_node")
    {
        RCLCPP_INFO(this->get_logger(),"创建节点");
        Subscription_ = this->create_subscription<std_msgs::msg::Int32>("robot_speed", 10, std::bind(&Speed_monitor::ab_cb, this));
    }
private:
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr Subscription_;
    int bool_choice = 0;
    
    void ab_cb(const std_msgs::msg::Int32 &msg)
    {
        RCLCPP_INFO(this->get_logger(),"接受速度为：%d",msg.data);
        v1.push_back(msg.data);
        q1.push(msg.data); 
        if(q1.size() >= 2)
        {
            int current_speed = q1.back();
            int previous_speed = q1.front(); 
            RCLCPP_INFO(this->get_logger(),"Processing Speed:%d, Previous speed: %d", current_speed, previous_speed);
            q1.pop(); 
            thread t(changeSpeed, current_speed, previous_speed);
            t.join(); // 主线程等待子线程结束
            // 统计警告次数逻辑
            for(auto i = v1.begin(); i != v1.end(); i++)
            {
                if(*i >= 3)
                {
                    bool_choice++;
                }
            }
            if(bool_choice >= 2)
            {
                cout << "System State: UNSTABLE" << endl;
            }
            else
            {
                cout << "System State: STABLE" << endl;
            }
        }
    }
};
int main(int argc, char const *argv[])
{
    //初始化
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Speed_monitor>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
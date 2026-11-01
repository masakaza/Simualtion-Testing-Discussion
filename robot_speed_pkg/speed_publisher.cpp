// 1.包含头文件
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <queue> 

// 3.自定义节点类
class Talker:public rclcpp::Node
{
public:
    Talker():Node("talkerstu_node_cpp")
    {
        speedpublisher_ = this->create_publisher<std_msgs::msg::Int32>("robot_speed",10);
        timer_ = this->create_wall_timer(std::chrono::seconds(1),std::bind(&Talker::give_speed,this));
    }
    void give_u_speed()
    {
        pick_speed.push(2);
        pick_speed.push(5);
        pick_speed.push(4);
        pick_speed.push(8);
        pick_speed.push(3);
        pick_speed.push(6);
    }
private:
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr speedpublisher_;  //创建对象接受指针
    rclcpp::TimerBase::SharedPtr timer_;
    std::queue<int>pick_speed;
    void give_speed()
    {
        if (!pick_speed.empty()) 
        {
            int temp = pick_speed.front();
            pick_speed.push(temp);
            pick_speed.pop();
            auto msg = std_msgs::msg::Int32();
            msg.data = temp;
            RCLCPP_INFO(this->get_logger(),"Publishing: %d",msg.data);
            speedpublisher_->publish(msg);
        }
    }
};

int main(int argc,char const *argv[])
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<Talker>(); 
    node->give_u_speed();
    rclcpp::spin(node);
    //
    rclcpp::shutdown();
    return 0;
}
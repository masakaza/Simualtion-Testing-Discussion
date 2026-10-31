#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <queue>
using namespace std;

void init_queue(queue<int>& q)
{
    q.push(2);
    q.push(5);
    q.push(4);
    q.push(8);
    q.push(3);
    q.push(6);
}

class speed_publish : public rclcpp::Node
{
private:
    queue<int> speed_q;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    void pub_msg_cb()
    {
        auto message = std_msgs::msg::Int32();
        int temp = speed_q.front();
        message.data = temp;
        RCLCPP_INFO(this->get_logger(),"server sent: %d",message.data);
        //循环
        speed_q.pop();
        speed_q.push(temp);
        publisher_->publish(message);
    }

public:
    speed_publish():Node("speed_pub_node")
    {
        RCLCPP_INFO(this->get_logger(),"速度发布节点创建");
        init_queue(speed_q);
        publisher_ = this->create_publisher<std_msgs::msg::Int32>("robot_speed",10);
        timer_ = this->create_wall_timer(1s,bind(&speed_publish::pub_msg_cb,this));
    }
};




int main(int argc,char const* argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(make_shared<speed_publish>());
    rclcpp::shutdown();
    return 0;
}
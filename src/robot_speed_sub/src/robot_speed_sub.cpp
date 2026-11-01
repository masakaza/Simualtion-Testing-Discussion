#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>
#include<thread>
#include<queue>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

template<typename T>
T SpeedDifference(T a,T b){
  return (a>b)?(a-b):(b-a);
}

static std::vector<int> difference_;

void Check_Speed_Difference(int v_0,int v_t){
  int result = SpeedDifference<int>(v_0,v_t);
  difference_.push_back(result);
  if(difference_.size()>4){
    difference_.erase(difference_.begin());
  }
  std::cout<<"Speed Difference:  "<<result<<std::endl;
  if(result>=3){
    std::cout<<"State: WARNING"<<std::endl;
  }
  else std::cout<<"State: NORMAL"<<std::endl;
}
class RobotSpeedSubscriber: public rclcpp::Node
{
public:
  RobotSpeedSubscriber():Node("robot_speed_sublisher")
  {
    speedsubsriber_ = this->create_subscription<std_msgs::msg::Int32>("robot_speed",10,std::bind(&RobotSpeedSubscriber::speed_callback, this, std::placeholders::_1));
  }
private:
void speed_callback(const std_msgs::msg::Int32 &msg){
  std::cout<<std::endl;
  RCLCPP_INFO(this->get_logger(),"Receive Speed:  %i",msg.data);
  speed_.push(msg.data);
  show_speed();
  std::thread th(Check_Speed_Difference,previous_speed_,process_speed_);
  th.join();
  SystemStateCheck();
  previous_speed_=process_speed_;
}
  void show_speed(){
    if(speed_.size()>1){
      process_speed_=speed_.front();
      speed_.pop();
      std::cout<<"Process Speed:  "<<process_speed_<<std::endl;
      std::cout<<"Previous Speed:  "<<previous_speed_<<std::endl;
    }
    else{
      std::cout<<"Wait for next speed!"<<std::endl;
    }
  }
  void SystemStateCheck(){
    warning_num_=0;
    for(std::vector<int>::iterator it=difference_.begin();it<difference_.end();it++){
      if(*it>=3) warning_num_++;
    }
    if(warning_num_>=2) std::cout<<"System State:  UNSTABLE"<<std::endl;
    else std::cout<<"System State:  STABLE"<<std::endl;
  }
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr speedsubsriber_;
  std::queue<int> speed_;
  int warning_num_ = 0;
  int process_speed_ = 0;
  int previous_speed_ = 0;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RobotSpeedSubscriber>());
  rclcpp::shutdown();
  return 0;
}

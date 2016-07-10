#include <managers/tasks_manager.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace tyd;

using namespace std::chrono_literals;

int main()
{
  for(unsigned int i = 0; i < 10; i++)
  {
    auto task = managers::add_task("task@project", 8h, models::task_size::M);
    std::cout << "Adding task: " << task << std::endl;
    std::this_thread::sleep_for(1s);
  }
}

#include <managers/tasks_manager.hpp>

#include <iostream>
#include <sstream>
#include <queue>
#include <functional>
#include <algorithm>
#include <map>

using namespace tyd;

using namespace std::chrono_literals;

using arguments = std::queue<std::string>;
using callback = std::function<bool(arguments&)>;

static bool keep_running { true };
std::experimental::optional<tyd::models::task_record> current_task_record {};

auto help = [](arguments& args) -> bool
{
  #pragma unused(args)

  std::cout << "Available commands\n";
  std::cout << "------------------\n\n";
  std::cout << "\thelp: print this help\n";
  std::cout << "\tquit: quits\n";

  std::cout << "\n";
  std::cout << "Tasks management\n";
  std::cout << "----------------\n\n";
  std::cout << "\tadd name estimate size: adds a task with these args\n";
  std::cout << "\t\tname: a string without spaces.\n";
  std::cout << "\t\testimate: the initial estimate in hours.\n";
  std::cout << "\t\tsize: the task size: XS, S, M, L, XL or XXL.\n";
  std::cout << "\tprint id: prints the informationof the task with that id\n";

  std::cout << "\n";
  std::cout << "Task recording\n";
  std::cout << "----------------\n\n";
  std::cout << "\tstart id [timestamp]: starts a new task record. It will\n";
  std::cout << "\t                      close the current task record if it\n";
  std::cout << "\t                      is already open\n";
  std::cout << "\t\tid: the id of the associated task.\n";
  std::cout << "\t\ttimestamp: YYYY-MM-DDTHH:MM:SS to set as start, if it is\n";
  std::cout << "\t\t           not defined, current timestamp will be used.\n";
  std::cout << "\tend restimate [timestamp]: ends the current opened task";
  std::cout << "\t\t                         record.\n";
  std::cout << "\t\trestimate: the remaining estimate in hours.\n";
  std::cout << "\t\ttimestamp: YYYY-MM-DDTHH:MM:SS to set as end, if it is\n";
  std::cout << "\t\t           not defined, current timestamp will be used.\n";

  std::cout << "\n";

  return true;
};

auto quit = [](arguments& args) -> bool
{
  #pragma unused(args)

  std::cout << "Bye...\n";
  keep_running = false;

  return true;
};

auto add = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto name = args.front();
    args.pop();

    auto estimate = std::chrono::hours(std::atoi(args.front().c_str()));
    args.pop();

    auto size = tyd::models::to_task_size(args.front());
    args.pop();

    tyd::managers::add_task(name, estimate, size);

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cerr << ex.what();
  }

  return retvalue;
};

auto print_task = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto& task = tyd::managers::find_task(std::atoi(args.front().c_str()));

    auto invested = tyd::models::get_invested_effort(task);
    auto remaining = task.records().back().estimate().value_or(
        task.estimate() - invested
      );

    std::cout << "id: " << task.id() << "\n";
    std::cout << "name: " << task.name() << "\n";
    std::cout << "size: " << tyd::models::to_string(task.size()) << "\n";
    std::cout << "original estimate: " << task.estimate().count() << " hours\n";
    std::cout << "invested effort: " << invested.count() << "hours\n";
    std::cout << "remaining effort: " << remaining.count() << "hours\n";
    std::cout << "deviation: " << "not implemented yet " << "hours\n";

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cout << "The task with id: " << args.front() << " does not exist.\n";
  }

  return retvalue;
};

auto start_record = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto& task = tyd::managers::find_task(std::atoi(args.front().c_str()));
    args.pop();

    auto time = std::chrono::system_clock::now();
    if(not args.empty())
    {
      std::cout << "Could not use timestamp (not implemented yet) using now.\n";
    }

    task.open_record(time);

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cout << "The task with id: " << args.front() << " does not exist.\n";
  }

  return retvalue;
};

auto end_record = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto& task = tyd::managers::find_task(std::atoi(args.front().c_str()));
    args.pop();

    auto reestimate = std::chrono::hours(std::atoi(args.front().c_str()));
    args.pop();

    auto time = std::chrono::system_clock::now();
    if(not args.empty())
    {
      std::cout << "Could not use timestamp (not implemented yet) using now.\n";
    }

    task.close_record(time, reestimate);

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cout << "The task with id: " << args.front() << " does not exist.\n";
  }

  return retvalue;
};

static std::map<std::string, callback> dispatcher = {
  { std::string("help"), help },
  { std::string("quit"), quit },
  { std::string("add"), add },
  { std::string("print"), print_task},
  { std::string("start"), start_record },
  { std::string("end"), end_record },
};

bool dispatch_command(
    const std::string& command,
    std::queue<std::string>& args
  )
{
  bool retvalue = false;

  try
  {
    retvalue = dispatcher.at(command)(args);
  }
  catch(const std::exception& ex)
  {
    std::cout << "Unknown command\n";
  }

  return retvalue;
};

int main()
{
  while(keep_running)
  {
    std::cout << "Command (or type help): ";

    std::string input;
    std::getline(std::cin, input);

    std::istringstream tokens(input);
    std::queue<std::string> args;

    for(std::string each; std::getline(tokens, each, ' '); args.push(each));

    auto command = args.front();
    args.pop();

    if(not dispatch_command(command, args))
    {
      std::cout << "Invalid arguments, see help";
    }
  }
}

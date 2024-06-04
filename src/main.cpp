#include "climenu.hpp"
#include "serial.hpp"

#include <string.h>

#include <boost/program_options.hpp>
#include <nlohmann/json.hpp>

#include <csignal>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using json = nlohmann::json;

static const std::string defaultDevice{"/dev/ttyAMA0"};

std::string extractDataFromSerial(std::shared_ptr<serial> serialIf,
                                  const std::string& cmd,
                                  bool getHearbeat = false)
{
    constexpr uint32_t maxsize = 1024;
    static const std::string heartbeattag{"{\"pa\":"};

    uint32_t retries = 3;
    while (retries--)
    {
        std::vector<uint8_t> tmpvec;
        serialIf->flushBuffer();
        serialIf->write({cmd.begin(), cmd.end()});
        serialIf->read(tmpvec, maxsize);

        std::string tmpstr{tmpvec.begin(), tmpvec.end()};
        if (!tmpstr.empty())
        {
            std::vector<std::string> tmplist;
            auto start = tmpstr.find("{"), end = tmpstr.find("}");
            while (start != std::string::npos && end != std::string::npos)
            {
                tmplist.push_back(tmpstr.substr(start, end + 1));
                tmpstr.erase(start, end + 1);
                start = tmpstr.find("{");
                end = tmpstr.find("}");
            }

            for (auto&& obj : tmplist)
            {
                if (obj.find(heartbeattag) == std::string::npos || getHearbeat)
                {
                    return obj;
                }
            }
        }
    }
    return {};
}

void displayJsonData(const json& json)
{
    for (const auto& [key, val] : json.items())
    {
        std::cout << key << ": " << val << '\n';
    }
}

void readwifiinfo(std::shared_ptr<serial> serialIf)
{
    auto jsoncmd = json::object({{"T", 65}});
    auto output = extractDataFromSerial(serialIf, jsoncmd.dump());
    displayJsonData(json::parse(output));
}

void readpowerinfo(std::shared_ptr<serial> serialIf)
{
    auto jsoncmd = json::object({{"T", 70}});
    auto output = extractDataFromSerial(serialIf, jsoncmd.dump());
    displayJsonData(json::parse(output));
}

void readimuinfo(std::shared_ptr<serial> serialIf)
{
    auto jsoncmd = json::object({{"T", 71}});
    auto output = extractDataFromSerial(serialIf, jsoncmd.dump());
    displayJsonData(json::parse(output));
}

void readdeviceinfo(std::shared_ptr<serial> serialIf)
{
    auto jsoncmd = json::object({{"T", 74}});
    auto output = extractDataFromSerial(serialIf, jsoncmd.dump());
    displayJsonData(json::parse(output));
}

void readhearbeatinfo(std::shared_ptr<serial> serialIf)
{
    auto output = extractDataFromSerial(serialIf, {}, true);
    displayJsonData(json::parse(output));
}

void sendusercmd(std::shared_ptr<serial> serialIf)
{
    static const auto exitTag = "q";

    std::cout << "Commands mode, to exit enter: " << exitTag << "\n";
    while (true)
    {
        std::cout << "CMD> ";
        std::string usercmd;
        std::cin >> usercmd;
        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');

        if (usercmd != exitTag)
        {
            try
            {
                auto jsoncmd = json::parse(usercmd);
                auto output = extractDataFromSerial(serialIf, jsoncmd.dump());
                if (!output.empty())
                {
                    displayJsonData(json::parse(output));
                }
            }
            catch (const json::parse_error& e)
            {
                std::cerr << "Cannot covert string to json\n";
            }
        }
        else
        {
            break;
        }
    }
}

std::string gettimestr(void)
{
    time_t rawtime{0};
    tm* timeinfo{0};

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    std::string timestr{asctime(timeinfo)};
    timestr.erase(std::remove(timestr.begin(), timestr.end(), '\n'),
                  timestr.end());
    return timestr;
}

void exitprogram()
{
    printf("Cleaning and closing\n");
    exit(0);
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        throw std::runtime_error("Safe app termiantion");
    }
}

int main(int argc, char* argv[])
{
    std::signal(SIGINT, signalHandler);
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")(
        "device,d", boost::program_options::value<std::string>(),
        "serial device node")("speed,s",
                              boost::program_options::value<std::string>(),
                              "speed of serial communication");

    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc;
        return 0;
    }

    const auto& device =
        vm.count("device") ? vm.at("device").as<std::string>() : defaultDevice;
    try
    {
        std::shared_ptr<serial> serialIf =
            std::make_shared<uart>(device, B115200);

        Menu menu{
            "[JSON commands " + gettimestr() + " @ " + device + "]",
            {{"to get wifi info", std::bind(readwifiinfo, serialIf)},
             {"to get power info", std::bind(readpowerinfo, serialIf)},
             {"to get imu info", std::bind(readimuinfo, serialIf)},
             {"to get device info", std::bind(readdeviceinfo, serialIf)},
             {"to get heartneat info", std::bind(readhearbeatinfo, serialIf)},
             {"to send user command", std::bind(sendusercmd, serialIf)},
             {"exit", exitprogram}}};

        menu.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception occured, aborting!\n";
    }
    return 0;
}

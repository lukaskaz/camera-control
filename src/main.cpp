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

#define UART_DEV "/dev/ttyAMA0"
#define SAMPLESPERSCAN 360
#define ANGLESTOCHK {0, 45, 90, 135, 180, 225, 270, 315};

#define SCANSTARTFLAG 0xA5
//#define SCANGETINFOCMD 0x50
#define SCANGETINFOCMD 65
#define SCANGETSTATCMD 0x52
#define SCANGETSRATECMD 0x59
#define SCANSTARTSCAN 0x20
#define SCANSTOPSCAN 0x25

using json = nlohmann::json;

std::string extractDataFromSerial(
    std::shared_ptr<serial> serialIf,
    std::tuple<std::string, std::string, uint32_t>&& params)
{
    const auto& [cmd, tag, maxsize] = params;
    while (true)
    {
        std::vector<uint8_t> tmpvec;
        serialIf->flushBuffer();
        serialIf->write({cmd.begin(), cmd.end()});
        serialIf->read(tmpvec, maxsize);

        std::string tmpstr{tmpvec.begin(), tmpvec.end()};
        // std::cout << tmpstr << std::endl;
        if (auto start = tmpstr.find("{\"" + tag + "\":");
            start != std::string::npos)
        {
            if (auto end = tmpstr.find("}", start); end != std::string::npos)
            {
                return tmpstr.substr(start, end + 1);
            }
        }
    }
    return {};
}

void readwifiinfo(std::shared_ptr<serial> serialIf)
{
    json jsoncmd = json::object({{"T", 65}});
    auto output = extractDataFromSerial(serialIf, {jsoncmd.dump(), "IP", 150});
    auto jsonData = json::parse(output);
    std::cout << jsonData << std::endl;
    std::cout << jsonData.at("AP_NAME") << std::endl;
}

void readpowerinfo(std::shared_ptr<serial> serialIf)
{
    json jsoncmd = json::object({{"T", 70}});
    auto output =
        extractDataFromSerial(serialIf, {jsoncmd.dump(), "shunt_mV", 150});
    auto jsonData = json::parse(output);
    std::cout << jsonData << std::endl;
    std::cout << jsonData.at("load_V") << std::endl;
}

void readimuinfo(std::shared_ptr<serial> serialIf)
{
    json jsoncmd = json::object({{"T", 71}});
    auto output =
        extractDataFromSerial(serialIf, {jsoncmd.dump(), "temp", 250});
    auto jsonData = json::parse(output);
    std::cout << jsonData << std::endl;
    std::cout << jsonData.at("temp") << std::endl;
}

void readdeviceinfo(std::shared_ptr<serial> serialIf)
{
    json jsoncmd = json::object({{"T", 74}});
    auto output = extractDataFromSerial(serialIf, {jsoncmd.dump(), "MAC", 30});
    auto jsonData = json::parse(output);
    std::cout << jsonData << std::endl;
    std::cout << jsonData.at("MAC") << std::endl;
}

const char* gettimestr(void)
{
    time_t rawtime = {0};
    struct tm* timeinfo = {0};

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return asctime(timeinfo);
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
        vm.count("device") ? vm.at("device").as<std::string>() : UART_DEV;
    try
    {
        std::shared_ptr<serial> serialIf =
            std::make_shared<uart>(device, B115200);

        Menu menu{"[JSON commands dispatcher on " + device + "]",
                  {{"to get wifi info", std::bind(readwifiinfo, serialIf)},
                   {"to get power info", std::bind(readpowerinfo, serialIf)},
                   {"to get imu info", std::bind(readimuinfo, serialIf)},
                   {"to get device info", std::bind(readdeviceinfo, serialIf)},
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

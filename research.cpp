
#include <iostream>
#include <string>
#include <vector>
#include <chrono>



struct Datetime() {
    std::chrono::sys_seconds datetime;
    Datetime(int year,
             int month,
             int day,
             int hour,
             int minute,
             int second)
        : timestamp_(
              // 1. Create the date part (sys_days) using C++20 calendar types
              std::chrono::sys_days{
                  std::chrono::year{year} / 
                  std::chrono::month{month} / 
                  std::chrono::day{day}
              } 
              // 2. Add time durations
              + std::chrono::hours{hour} 
              + std::chrono::minutes{minute} 
              + std::chrono::seconds{second}
          ) {

        }

    int year() const {
        // 1. Cast down to days (required for calendar conversions)
        auto days = std::chrono::floor<std::chrono::days>(datetime);
        
        // 2. Convert days to a year_month_day object
        std::chrono::year_month_day ymd{days};
        
        // 3. Cast the std::chrono::year to an integer
        return static_cast<int>(ymd.year());
    }

    int month() const {
        auto days = std::chrono::floor<std::chrono::days>(datetime);
        std::chrono::year_month_day ymd{days};
        return static_cast<int>(ymd.month());
    }


    int day() const {
        auto days = std::chrono::floor<std::chrono::days>(datetime);
        std::chrono::year_month_day ymd{days};
        return static_cast<int>(ymd.day());
    }

    int hour() const {
        // 1. Convert sys_seconds to sys_days to isolate the date portion
        auto day_point = std::chrono::floor<std::chrono::days>(datetime);
        
        // 2. Subtract the day portion to get the time passed since midnight
        auto time_of_day = std::chrono::hh_mm_ss{m_timestamp - day_point};
        
        // 3. Return the hours component
        return time_of_day.hours().count();
    } 


    int minutes() const {
        auto day_point = std::chrono::floor<std::chrono::days>(datetime);
        auto time_of_day = std::chrono::hh_mm_ss{m_timestamp - day_point};
        return time_of_day.minutes().count();
    } 


    int seconds() const {
        auto day_point = std::chrono::floor<std::chrono::days>(datetime);
        auto time_of_day = std::chrono::hh_mm_ss{m_timestamp - day_point};
        return time_of_day.seconds().count();
    } 

    int epoch() const {
        return datetime.time_since_epoch().count();
    }

}

struct Candle {
    double open;
    double close;
    double high;
    double low;
    int volume;
    Datetime datetime;
   
     
};

/*
void saveToBinary(const MyData& data, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) return;

    // 1. Extract the underlying representation (time_since_epoch)
    // .count() returns the raw integer number of seconds
    auto raw_time = data.timestamp.time_since_epoch().count();

    // 2. Write the components sequentially
    out.write(reinterpret_cast<const char*>(&raw_time), sizeof(raw_time));
    out.write(reinterpret_cast<const char*>(&data.id), sizeof(data.id));
}


MyData loadFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    MyData data{};
    if (!in) return data;

    // 1. Read the raw time integer type
    decltype(std::declval<std::chrono::sys_seconds>().time_since_epoch().count()) raw_time;
    in.read(reinterpret_cast<char*>(&raw_time), sizeof(raw_time));
    
    // 2. Read the integer ID
    in.read(reinterpret_cast<char*>(&data.id), sizeof(data.id));

    // 3. Reconstruct the sys_seconds object
    std::chrono::seconds duration(raw_time);
    data.timestamp = std::chrono::sys_seconds(duration);

    return data;
}

*/

void analyze_symbol() {
        
}


int main(int argc, char* argv[]) {
    
     std::vector<std::string> args(argv, argv + argc);

    // Default values
    std::string name = "Guest";
    bool verbose = false;
    int nyse_and_nasdaq_index = 0;

    // Manual loop starting at index 1 (index 0 is the program executable path)
    for (size_t i = 1; i < args.size(); ++i) {
        if ((args[i] == "-n" || args[i] == "--name") && i + 1 < args.size()) {
            name = args[++i]; // Get the next item as the value
        } else if (args[i] == "-v" || args[i] == "--verbose") {
            verbose = true;
        } else if (args[i] == "--nn_index") {
            nyse_and_nasdaq_index  = stoi(++i);
        }
   
    
     
     

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

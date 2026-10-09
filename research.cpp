
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <numeric>

using namespace std;

struct Datetime {
    std::chrono::sys_seconds datetime;
    Datetime(int year,
             int month,
             int day,
             int hour,
             int minute,
             int second)
        : datetime(
              // 1. Create the date part (sys_days) using C++20 calendar types
              std::chrono::sys_days{
                  std::chrono::year{year} / 
                  std::chrono::month{static_cast<unsigned int>(month)} / 
                  std::chrono::day{static_cast<unsigned int>(day)}
              } 
              // 2. Add time durations
              + std::chrono::hours{hour} 
              + std::chrono::minutes{minute} 
              + std::chrono::seconds{second}
          ) {

        }

    Datetime() {}

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
        return static_cast<unsigned>(ymd.month());
    }


    int day() const {
        auto days = std::chrono::floor<std::chrono::days>(datetime);
        std::chrono::year_month_day ymd{days};
        return static_cast<unsigned>(ymd.day());
    }

    int hour() const {
        // 1. Convert sys_seconds to sys_days to isolate the date portion
        auto day_point = std::chrono::floor<std::chrono::days>(datetime);
        
        // 2. Subtract the day portion to get the time passed since midnight
        auto time_of_day = std::chrono::hh_mm_ss{datetime - day_point};
        
        // 3. Return the hours component
        return time_of_day.hours().count();
    } 


    int minute() const {
        auto day_point = std::chrono::floor<std::chrono::days>(datetime);
        auto time_of_day = std::chrono::hh_mm_ss{datetime - day_point};
        return time_of_day.minutes().count();
    } 


    int second() const {
        auto day_point = std::chrono::floor<std::chrono::days>(datetime);
        auto time_of_day = std::chrono::hh_mm_ss{datetime - day_point};
        return time_of_day.seconds().count();
    } 

    int epoch() const {
        return datetime.time_since_epoch().count();
    }

};

struct Candle {
    double open;
    double close;
    double high;
    double low;
    int volume;
    Datetime datetime;
   
    Candle() {}    
 
    Candle(double open,
           double close,
           double high,
           double low,
           int volume,
           int year,
           int month,
           int day,
           int hour,
           int minute) {
        this->open = open;
        this->close = close;
        this->high = high;
        this->low = low;
        this->volume = volume;
        this->datetime = Datetime(year, month, day, hour, minute, 0);
    }  
     
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


std::vector<double> calculateEMA(const std::vector<double>& data, int period) {
    if (data.empty() || period <= 0) return {};
    
    std::vector<double> ema(data.size(), 0.0);
    
    // 1. Calculate the smoothing factor (alpha)
    double alpha = 2.0 / (period + 1);
    
    // 2. Establish the initial seed value (SMA of the first 'period' elements)
    if (data.size() < period) {
        // Not enough data for the specified period; fallback to initial value
        ema[0] = data[0];
        for (size_t i = 1; i < data.size(); ++i) {
            ema[i] = alpha * data[i] + (1 - alpha) * ema[i - 1];
        }
        return ema;
    }
    
    // Standard approach: Use SMA for the first valid data point
    double sum = std::accumulate(data.begin(), data.begin() + period, 0.0);
    double sma = sum / period;
    ema[period - 1] = sma;
    
    // 3. Calculate EMA for the remaining elements
    for (size_t i = period; i < data.size(); ++i) {
        ema[i] = (data[i] * alpha) + (ema[i - 1] * (1.0 - alpha));
    }
    
    return ema;
}

void update_technicals(const vector<Candle> & sorted_hour_candles,
                       int h,
                       vector<double> & two_hundred_hour_ema, 
                       vector<double> & twelve_hour_ema, 
                       vector<double> & twenty_six_hour_ema, 
                       vector<double> & twenty_hour_ema,
                       vector<double> & hour_macd,
                       vector<double> & hour_macd_signal,
                       vector<double> & hour_macd_histo) {
    double new_200_ema = (sorted_hour_candles[h].close - two_hundred_hour_ema.back()) * 2.0 / (200 + 1) + two_hundred_hour_ema.back();
    two_hundred_hour_ema.push_back(new_200_ema);

    double new_12_ema = (sorted_hour_candles[h].close - twelve_hour_ema.back()) * 2.0 / (12 + 1) + twelve_hour_ema.back();
    twelve_hour_ema.push_back(new_12_ema);

    double new_26_ema = (sorted_hour_candles[h].close - twenty_six_hour_ema.back()) * 2.0 / (26 + 1) + twenty_six_hour_ema.back();
    twenty_six_hour_ema.push_back(new_26_ema);
   
    double new_20_ema = (sorted_hour_candles[h].close - twenty_hour_ema.back()) * 2.0 / (20 + 1) + twenty_hour_ema.back();
    twenty_hour_ema.push_back(new_20_ema);

    double new_hour_macd = new_12_ema - new_26_ema;
    hour_macd.push_back(new_hour_macd);

    double new_hour_macd_signal = (new_hour_macd - hour_macd_signal[-1]) * 2.0 / (9 + 1) + hour_macd_signal[-1];
    hour_macd_signal.push_back(new_hour_macd_signal);

    hour_macd_histo.push_back(new_hour_macd - new_hour_macd_signal);


}


void update_new_candle_and_technicals(int h0, int h, Candle & new_hour_candle, const vector<Candle> & sorted_one_min_candles, int i, const vector<Candle> & sorted_hour_candles, double & new_200_hour_ema, const vector<double> & two_hundred_hour_ema, const vector<double> & twelve_hour_ema, const vector<double> & twenty_six_hour_ema, double & new_20_hour_ema, const vector<double> & twenty_hour_ema, double & new_hour_macd, const vector<double> & hour_macd_signal, double & new_hour_macd_histo, bool day_mode) {
    if (h0 != h) {
        new_hour_candle = sorted_one_min_candles[i];
        int start_hour_index = i - 1;
        int epoch = 0;
        if (day_mode) {
            epoch = sorted_hour_candles[h + 1].datetime.epoch();
        } else {
            epoch = sorted_hour_candles[h].datetime.epoch();
        }
        while (start_hour_index >= 0) {
            if (sorted_one_min_candles[start_hour_index].datetime.epoch() > epoch) {
                new_hour_candle.volume += sorted_one_min_candles[start_hour_index].volume;
                new_hour_candle.high = max(new_hour_candle.high, sorted_one_min_candles[start_hour_index].high);
                new_hour_candle.low = min(new_hour_candle.low, sorted_one_min_candles[start_hour_index].low);
            } else {
                break;
            }

            start_hour_index -= 1;
        }

        new_hour_candle.open += sorted_one_min_candles[start_hour_index + 1].open;
    } else {
        new_hour_candle.volume += sorted_one_min_candles[i].volume;
        new_hour_candle.high = max(new_hour_candle.high, sorted_one_min_candles[i].high);
        new_hour_candle.low = min(new_hour_candle.low, sorted_one_min_candles[i].low);
        new_hour_candle.close = sorted_one_min_candles[i].close;

    }


    new_200_hour_ema = (sorted_one_min_candles[i].close - two_hundred_hour_ema.back()) * 2.0 / (200 + 1) + two_hundred_hour_ema.back();
    double new_12_hour_ema = (sorted_one_min_candles[i].close - twelve_hour_ema.back()) * 2.0 / (12 + 1) + twelve_hour_ema.back();
    double new_26_hour_ema = (sorted_one_min_candles[i].close - twenty_six_hour_ema.back()) * 2.0 / (26 + 1) + twenty_six_hour_ema.back();
    new_20_hour_ema = (sorted_one_min_candles[i].close - twenty_hour_ema.back()) * 2.0 / (20 + 1) + twenty_hour_ema.back();
    new_hour_macd = new_12_hour_ema  = new_26_hour_ema;
    double new_hour_macd_signal = (new_hour_macd - hour_macd_signal.back()) * 2.0 / (9 + 1) + hour_macd_signal.back();
    new_hour_macd_histo = new_hour_macd - new_hour_macd_signal;
 }


void first_time_calculate_technicals(const vector<Candle> & sorted_hour_candles, int h, vector<double> & two_hundred_hour_ema, vector<double> & twelve_hour_ema, vector<double> & twenty_six_hour_ema, vector<double> & twenty_hour_ema, vector<double> & hour_macd, vector<double> & hour_macd_signal, vector<double> & hour_macd_histo) {
    vector<Candle> first_h_candles(sorted_hour_candles.begin(), sorted_hour_candles.begin() + h);
    vector<double> first_h_candles_close;
    for (size_t j = 0; j < first_h_candles.size(); ++j) {
        first_h_candles_close.push_back(first_h_candles[j].close);
    }
    two_hundred_hour_ema = calculateEMA(first_h_candles_close, 200);
    twelve_hour_ema = calculateEMA(first_h_candles_close, 12);
    twenty_six_hour_ema = calculateEMA(first_h_candles_close, 26);
    twenty_hour_ema = calculateEMA(first_h_candles_close, 20);

    for (size_t j = 26; j < twelve_hour_ema.size(); ++j) {
        hour_macd.push_back(twelve_hour_ema[j] - twenty_six_hour_ema[j]);
    }


    hour_macd_signal = calculateEMA(hour_macd, 9);
    for (size_t j = 9; j < hour_macd.size(); ++j) {
        hour_macd_histo.push_back(hour_macd[j] - hour_macd_signal[j]);
    }
}


bool is_daylight_savings1(const Datetime & datetime) {
    // todo
    return true;
}




void analyze_symbol(string calc_mode) {
    vector<Candle> sorted_day_candles;
    vector<Candle> sorted_hour_candles;
    vector<Candle> sorted_five_min_candles;
    vector<Candle> sorted_one_min_candles;
    
    vector<Candle> hour_candles_sub;
    Candle new_hour_candle;

    vector<double> two_hundred_hour_ema;
    vector<double> twelve_hour_ema;
    vector<double> twenty_hour_ema;
    vector<double> twenty_six_hour_ema;

    vector<double> hour_macd;
    vector<double> hour_macd_signal;
    vector<double> hour_macd_histo;

    double new_200_hour_ema; 
    double new_20_hour_ema; 
    double new_hour_macd;
    double new_hour_macd_histo;


    vector<Candle> day_candles_sub;
    Candle new_day_candle;

    vector<double> two_hundred_day_ema;
    vector<double> twelve_day_ema;
    vector<double> twenty_day_ema;
    vector<double> twenty_six_day_ema;

    vector<double> day_macd;
    vector<double> day_macd_signal;
    vector<double> day_macd_histo;

    double new_200_day_ema; 
    double new_20_day_ema; 
    double new_day_macd;
    double new_day_macd_histo;
       

    int i = 0;
    int h = 0;
    int d = 0;
    bool first_time_calculate_ema = true;

    while (i < sorted_one_min_candles.size()) {
        int h0 = h;
        while (h < sorted_hour_candles.size() && sorted_hour_candles[h].datetime.epoch() < sorted_one_min_candles[i].datetime.epoch()) {
            hour_candles_sub.push_back(sorted_hour_candles[h]);
            if (!first_time_calculate_ema) {
                update_technicals(sorted_hour_candles, 
                                  h,
                                  two_hundred_hour_ema, 
                                  twelve_hour_ema, 
                                  twenty_six_hour_ema, 
                                  twenty_hour_ema,
                                  hour_macd,
                                  hour_macd_signal,
                                  hour_macd_histo);

/*
                new_200_ema = (sorted_hour_candles[h].close - two_hundred_hour_ema.back()) * 2.0 / (200 + 1) + two_hundred_hour_ema.back();
                two_hundred_hour_ema.push_back(new_200_ema);

                double new_12_ema = (sorted_hour_candles[h].close - twelve_hour_ema.back()) * 2.0 / (12 + 1) + twelve_hour_ema.back();
                twelve_hour_ema.push_back(new_12_ema);

                double new_26_ema = (sorted_hour_candles[h].close - twenty_six_hour_ema.back()) * 2.0 / (26 + 1) + twenty_six_hour_ema.back();
                twenty_six_hour_ema.push_back(new_26_ema);
               
                new_20_ema = (sorted_hour_candles[h].close - twenty_hour_ema.back()) * 2.0 / (20 + 1) + twenty_hour_ema.back();
                twenty_hour_ema.push_back(new_20_ema);

                new_hour_macd = new_12_ema - new_26_ema;
                hour_macd.push_back(new_hour_macd);

                double new_hour_macd_signal = (new_hour_macd - hour_macd_signal[-1]) * 2.0 / (9 + 1) + hour_macd_signal[-1];
                hour_macd_signal.push_back(new_hour_macd_signal);

                hour_macd_histo.push_back(new_hour_macd - new_hour_macd_signal)
*/
            }
            h += 1;
        }

        int d0 = d;
        while (d < sorted_day_candles.size() && sorted_day_candles[d + 1].datetime.epoch() < sorted_one_min_candles[i].datetime.epoch()) {
            day_candles_sub.push_back(sorted_day_candles[d]);
            if (!first_time_calculate_ema) {
                update_technicals(sorted_day_candles, 
                                  d,
                                  two_hundred_day_ema, 
                                  twelve_day_ema, 
                                  twenty_six_day_ema, 
                                  twenty_day_ema,
                                  day_macd,
                                  day_macd_signal,
                                  day_macd_histo);

            }
            d += 1;
        }



        if (first_time_calculate_ema) {
            first_time_calculate_technicals(sorted_hour_candles, h, two_hundred_hour_ema, twelve_hour_ema, twenty_six_hour_ema, twenty_hour_ema, hour_macd, hour_macd_signal, hour_macd_histo);
            first_time_calculate_technicals(sorted_day_candles, d, two_hundred_day_ema, twelve_day_ema, twenty_six_day_ema, twenty_day_ema, day_macd, day_macd_signal, day_macd_histo);

/* 
            std::vector<Candle> first_h_candles(sorted_hour_candles.begin(), sorted_hour_candles.begin() + h);
            std::vector<double> first_h_candles_close;
            for (size_t j = 0; j < first_h_candles.size(); ++j) {
                first_h_candles_close.push_back(first_h_candles[j].close);
            }
            two_hundred_hour_ema = calculateEMA(first_h_candles_close, 200);
            twelve_hour_ema = calculateEMA(first_h_candles_close, 12);
            twenty_six_hour_ema = calculateEMA(first_h_candles_close, 26);
            twenty_hour_ema = calculateEMA(first_h_candles_close, 20);

            for (size_t j = 26; j < twelve_hour_ema.size(); ++j) {
                hour_macd.push_back(twelve_hour_ema[j] - twenty_six_hour_ema[j]);
            }


            hour_macd_signal = calculateEMA(hour_macd, 9);
            for (size_t j = 9; j < hour_macd.size(); ++j) {
                hour_macd_histo.push_back(hour_macd[j] - hour_macd_signal[j]);
            }
*/
        }


        update_new_candle_and_technicals(h0, h, new_hour_candle, sorted_one_min_candles, i, sorted_hour_candles, new_200_hour_ema, two_hundred_hour_ema, twelve_hour_ema, twenty_six_hour_ema, new_20_hour_ema, twenty_hour_ema, new_hour_macd, hour_macd_signal, new_hour_macd_histo, false);
        update_new_candle_and_technicals(d0, d, new_day_candle, sorted_one_min_candles, i, sorted_day_candles, new_200_day_ema, two_hundred_day_ema, twelve_day_ema, twenty_six_day_ema, new_20_day_ema, twenty_day_ema, new_day_macd, day_macd_signal, new_day_macd_histo, true); 

        const auto & current_time = sorted_one_min_candles[i].datetime;
        if (calc_mode == "hour_end") {
            if (current_time.minute() != 58) {
                i += 1;
                continue;
            }
        } else if (calc_mode == "day_end") {
            if (is_daylight_savings1(current_time)) {
                if (!(current_time.hour() == 19 && current_time.minute() == 58)) {
                    i += 1;
                    continue;
                }
            } else {
                if (!(current_time.hour() == 20 && current_time.minute() == 58)) {
                    i += 1;
                    continue;
                }
            }
        }

        i += 1;
    }

        
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
            nyse_and_nasdaq_index  = std::stoi(args[++i]);
        }
   
    }    
     
     

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

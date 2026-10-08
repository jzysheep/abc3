
#include <iostream>
#include <string>
#include <vector>


struct Datetime() {
    
}

struct Candle {
    double open;
    double close;
    double high;
    double low;
    int volume;
    Datetime datetime;
    
};

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

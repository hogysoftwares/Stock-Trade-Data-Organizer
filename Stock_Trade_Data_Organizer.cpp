#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits> 
#include <unordered_map>
#include <iomanip>  
 
/**
Stock_Trade_Data_Organizer v1.2

This program is designed to read stock trade data from a file,
and compute as well as organize the data needed to calculate the Volume Weighted Average Prices (VWAP), 
The program should do the following:

// Read file in with the following format:
// [Stock],[Interval],[Volume Traded],[High],[Low]

// Calculate the total volume traded per Stock
// Calculate the total volume traded per Stock&Interval

// Write the total volume traded per Stock&Interval as a percentage of the total volume traded per Stock to stdout:
// [Stock],[Interval],[%Volume Traded]

// Write the delimiter '#' to stdout

// Write the maximum High and minimum Low for each Stock to stdout:
// [Stock],[Day High],[Day Low]

// example input:
VOD.L 1 100 184 183.7
BT.LN 1 300 449.4 448.2
VOD.L 2 25 184.1 182.4
BT.LN 2 900 449.8 449.5

// example output:
VOD.L,1,80
BT.LN,1,25
VOD.L,2,20
BT.LN,2,75
#
VOD.L,184.1,182.4
BT.LN,449.8,448.2

**/

/* Stock traded in each interval as struct for organization. There aren't any needed member functions so using struct over class feels sufficient */
struct Trade_in_Interval {
    std::string stock_name;
    int interval;
    int volume;
    float high_price;
    float low_price;
};

/* Class to track minimum low price and maximum high price for a stock */
class StockMinMax {
public:
    StockMinMax() 
        : min_lowPrice(std::numeric_limits<float>::max()), max_highPrice(std::numeric_limits<float>::lowest()) {}

    void update_prices(float high_price, float low_price) {
        if (high_price > max_highPrice) {
            max_highPrice = high_price;
        }
        if (low_price < min_lowPrice) {
            min_lowPrice = low_price;
        }
    }

    /* getter functions so it should be const */
    float get_min_low() const {
     /* Although according to the program specification and examples it shouldn't happen, output 0.0f instead of limit values in case no prices were added for a stock.
        0.0f feels more appropriate for such a case. Limit values could also cause overflow somewhere. */
        return (min_lowPrice != std::numeric_limits<float>::max()) ? min_lowPrice : 0.0f;
    }

    float get_max_high() const {
        return (max_highPrice != std::numeric_limits<float>::lowest()) ? max_highPrice : 0.0f;
    }

private:
    float min_lowPrice;
    float max_highPrice;
};

const std::string DELIMITER = "#";
const std::string VERSION_NAME = "Stock Trade Data Organizer version 1.2";

/*  I am assuming that based on what the original code was trying to do, it meant to take in only one argument, either version or filename. Therefore the expected input is argc = 2. */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        /* Error cases where either no argument was provided or too many arguments were provided */
        if(argc == 1){
            std::cerr << "Error: No argument provided! Expected one argument (version or filename)" << std::endl;
        }
        else{
            std::cerr << "Error: Expected only one argument (version or filename)" << std::endl;
        }
        return -1;
    }

    /* If argument was version */
    std::string version_arg = argv[1];
    if (version_arg == "version") {
        std::cout << VERSION_NAME << std::endl;
        return 0;
    }

    /* Use argv[1] as filename if version is not the first argument */
    std::string filename = argv[1]; 
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << "Unable to access file " << filename << std::endl;
        return -1;
    }

    std::vector<Trade_in_Interval> trade_entries;
    /*unordered_map used instead of map since there was no specification that requires the ordering. 
    unordered_map is less expensive for accessing, inserting, deleting, which can be important for low-latency systems.
    The way map was used before, it would just be alphabetical order anyway, which the program specifications do not ask for. If that is important can be switched to map. */
    std::unordered_map<std::string, long long> TotalVolume_Per_Stock; 
    std::unordered_map<std::string, StockMinMax> HighLow_Per_Stock; 

    std::string line;
    std::cout << "Reading file " << filename << "..." << std::endl;

    while (std::getline(input_file, line)) {
        std::stringstream ss(line);
        Trade_in_Interval current_entry;

        if (ss >> current_entry.stock_name >> current_entry.interval >> current_entry.volume >> current_entry.high_price >> current_entry.low_price) {
            trade_entries.push_back(current_entry);
            TotalVolume_Per_Stock[current_entry.stock_name] += current_entry.volume;
            HighLow_Per_Stock[current_entry.stock_name].update_prices(current_entry.high_price, current_entry.low_price);
        }
        else {
            std::cerr << "Incomplete line skipped!" << line << std::endl;
        }
    }
    input_file.close();

    std::cout << "Writing output..." << std::endl;

    // First Output: [Stock],[Interval],[%Volume Traded]
    for (const auto& entry : trade_entries) {
        if (TotalVolume_Per_Stock[entry.stock_name] > 0) { /* Prevent dividing by zero */ 
            double percentage = (static_cast<double>(entry.volume) * 100.0) / TotalVolume_Per_Stock[entry.stock_name]; /* double instead of float in case the volume scale is too big before division */
            std::cout << entry.stock_name << "," << entry.interval << "," << std::fixed << std::setprecision(0) << percentage << std::endl; /* Round to whole number percentages, according to example*/
        }
        else {
            /* If the total trade volume was 0 for the stock, even though it shouldn't happen since that shouldn't be possible from the entries from file */
            std::cout << entry.stock_name << "," << entry.interval << "," << "0" << std::endl;
        }
    }

    std::cout << DELIMITER << std::endl;

    // Second Output: [Stock],[Day High],[Day Low]
    for (const auto& pair : HighLow_Per_Stock) {
        /* Prices are currently set to one decimal place so I used setprecision(1), but can be changed to two decimal places to fully represent cents as well. */
        std::cout << pair.first << "," << std::fixed << std::setprecision(1) << pair.second.get_max_high() << "," 
                                        << std::fixed << std::setprecision(1) << pair.second.get_min_low() << std::endl;
    }

    return 0;
}
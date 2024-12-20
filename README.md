# Velocity-Vortex
```
Welcome to the Velocity-Vortex: Where Every Millisecond Counts. 🚀
```
# Algo-Bot Process (V2.0.0) [On-going]
![diagram-export-11-18-2024-6_15_58-PM](https://github.com/user-attachments/assets/543d28cf-4080-4cc0-9308-3bc7747bd32d)
## Project Features

1. **High-Speed Trade Execution**  
   Capable of executing trades at lightning-fast speeds, leaving Python and heavyweight JavaScript.

2. **Comprehensive Indicator Library**  
   Includes a library of technical indicators to streamline strategy development.

3. **Custom Strategy Support**  
   Easily create and implement custom trading strategies.

4. **Scalable Architecture**  
   Designed with a scalable IO broker for handling high-throughput operations.

5. **Flexible Provider Integration**  
   Allows easy switching between different data and trading providers.

6. **Risk Analysis Module**  
   Built-in module for robust risk assessment and performance metrics.

7. **Backtesting Module**  
   Includes a backtesting environment with examples for validating strategies on historical data.

8. **FIFO-Based Order Book**  
   Implements an order book structure based on First-In-First-Out (FIFO) principles.

9. **WebSocket Integration**  
   Supports WebSocket connections for real-time market data and trade execution.

10. **Multi-Threaded Execution**  
    Leverages multi-threading for optimized, parallel processing.
    
11. **Statistical and Reinforcement Learning Models**  
    Includes basic statistical models and reinforcement learning algorithms for better decision-making and strategy improvement.




## File Descriptions

### Root Directory

- **.gitignore**: Specifies files and directories to be ignored by Git.
- **README.md**: Project documentation and instructions.

### Include Directory

- **AlgoEngine-Core/**: Core algorithms and indicators for the engine.
- **Data-Fetcher-Core/**: Modules for fetching data from various sources.
- **IO-Broker-Core/**: Input/Output broker modules.
- **matplotlib-cpp/**: Header-only C++ plotting library.
- **matplotlibcpp.h**: Header file for matplotlib-cpp.
- **Order-Manager-Core/**: Core modules for managing orders.
- **Orderbook/**: Modules for handling order books.
- **Risk-Analysis-Core/**: Core modules for risk analysis.
- **Utilities/**: Utility functions and helpers.
- **Velocity-Bot/**: Bot implementation for executing trades.

### IO-Server Directory

- **build/**: Build directory for the IO server.
- **CMakeLists.txt**: CMake configuration file for the IO server.
- **src/**: Source files for the IO server.

### Src Directory

- **AlgoEngine-Core/**: Source files for core algorithms and indicators.
- **Backtesting-Bot/**: Modules for backtesting trading strategies.
- **Data-Fetcher-Core/**: Source files for data fetching.
- **IO-Broker-Core/**: Source files for IO broker.
- **main.cpp**: Main entry point for the application.
- **Order-Manager-Core/**: Source files for order management.
- **Orderbook/**: Source files for order book handling.
- **Risk-Analysis-Core/**: Source files for risk analysis.
- **Utilities/**: Source files for utility functions.

## Building the Project

To build the project, ensure you have CMake installed and run the following commands:

```
mkdir build
cd build
cmake ..
cmake --build .
```

## Dependencies

The project depends on several libraries, including:

- Hiredis
- SQLite3
- Python3 (with NumPy)
- OpenSSL
- Boost
- CURL
- jsoncpp


# Main engine 
<img src="https://github.com/user-attachments/assets/c73fab4b-9e09-4caf-8f19-20bcc555c51c" alt="Main Engine" width="500" />

# Algo-Bot
<img src="https://github.com/user-attachments/assets/1f05e4b2-1ba0-4c13-93e8-faa1d95ac0f8" alt="Algo-Bot" width="500" />

# Algo-Bot Process (V1.8.0)
![diagram-export-11-5-2024-10_21_56-PM](https://github.com/user-attachments/assets/c1867a5f-e0b3-44b1-8cf2-4ba34a1f54fb)

# Class and Functions
![diagram-export-11-26-2024-12_23_06-AM](https://github.com/user-attachments/assets/ccc0983a-3f06-47c9-9773-fcde0328f109)







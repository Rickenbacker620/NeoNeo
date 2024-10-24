export module console;
import <iostream>;

// Singleton class for logging to console
export class ConsoleOut {
  public:
  static void log(const std::string &msg) {
      std::cout << msg << std::endl;
  }
};

export class LinesOut
{
  public:
    static void log(const std::string &msg) {
        std::cout << msg << std::endl;
    }
};

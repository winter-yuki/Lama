#include <cassert>
#include <fstream>
#include <stdexcept>
#include <variant>

#include "analyzer.hpp"
#include "interpreter.hpp"

struct InterpretConfig final {
    const std::string filename;
};

struct AnalysisConfig final {
    const std::string filename;
};

using Config = std::variant<InterpretConfig, AnalysisConfig>;

Config parseCliArgs(int argc, char * argv[]) {
    if (argc == 2) {
        return Config{InterpretConfig{argv[1]}};
    } else if (argc == 3 && std::string(argv[1]) == "-a") {
        return Config{AnalysisConfig{argv[2]}};
    } else {
        throw std::runtime_error("[-a for analysis] and .bc file");
    }
}

int main(int argc, char * argv[]) {
    const auto config = parseCliArgs(argc, argv);
    if (std::holds_alternative<InterpretConfig>(config)) {
        interpret(std::get<InterpretConfig>(config).filename);
    } else if (std::holds_alternative<AnalysisConfig>(config)) {
        analyze(std::get<AnalysisConfig>(config).filename);
    } else {
        assert(false && "Unknown execution mode");
    }
    return EXIT_SUCCESS;
}

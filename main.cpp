#include <pycom/lib/Pycom.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>

int main(int _argc, char *_argv[])
{
    if (_argc == 1)
    {
        std::cerr << "Usage: pycom <filename>[ <filename>[...]]\n"
                     "Options:\n"
                     "-o <filename>      Set output file name\n"
                     "-c                 Only compile the file, do not link\n"
                     "-emit-llvm         Emit LLVM IR code\n"
                     "-ON                Do optimizations, N can be 0, 1, 2, 3, s, z\n";
        return 0;
    }

    for (int i = 1; i < _argc; i++)
    {
        if (!std::filesystem::exists(_argv[i]))
        {
            std::cerr << "File " << _argv[i] << " does not exist!\n";
            return -1;
        }
        if (std::filesystem::is_directory(_argv[i]))
        {
            std::cerr << _argv[i] << " is a directory!\n";
            return -1;
        }
    }

    auto errmng = std::make_unique<ErrorManager>(std::cerr);
    Pycom pycom(errmng.get());

    std::ifstream file(_argv[1]);

    std::error_code EC;
    llvm::raw_fd_ostream output("output.o", EC, llvm::sys::fs::OF_None);

    pycom.open(file);
    if (pycom.checkSemantics())
    {
        pycom.generate();
        pycom.compile(output, llvm::OptimizationLevel::O0, llvm::PIELevel::Default, llvm::PICLevel::NotPIC);
    }

    return 0;
}

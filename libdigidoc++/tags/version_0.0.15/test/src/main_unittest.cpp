#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <BDoc.h>
#include <util/File.h>

std::string TEST_DATA_PATH = "../../test/data";
std::vector<std::string> ARGS;

int main(int argc, char* argv[])
{
    // Initialize digidoc library.
    digidoc::initialize();

    // Make command line arguments globally available.
    for(int i = 0; i < argc; i++)
    {
        ARGS.push_back(argv[i]);
    }

    // Set test data path.
    if(ARGS.size() > 1)
    {
        std::string path = ARGS[1];
        if(path[path.size() - 1] == '/'
        || path[path.size() - 1] == '\\')
        {
            path = path.substr(0, path.size() - 1);
        }
        TEST_DATA_PATH = path;
    }

    // Make sure that test data directory exists, otherwise some tests will fail.
    if(!digidoc::util::File::directoryExists(TEST_DATA_PATH))
    {
        printf("WARNING: Test data path '%s' does not exist, some tests, that need test files will fail.\n", TEST_DATA_PATH.c_str());
        if(ARGS.size() < 2)
        {
            printf("Please add test data path to command line (e.g. %s /path/to/test/data)", argv[0]);
        }
    }

    // Get the top level suite from the registry.
    CppUnit::Test* suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run.
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    // Change the default outputter to a compiler error format outputter.
    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));

    // Run the tests.
    bool wasSucessful = runner.run();

    // Terminate digidoc library.
    digidoc::terminate();

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;
}

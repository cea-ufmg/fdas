/**
 * FDAS device module for reading iio devices.
 */

#include <iostream>

#include <iio.h>

#include "common/common.hpp"

namespace po = boost::program_options;

using namespace fdas;
using std::cerr;
using std::cout;
using std::endl;
using std::string;


int main (int argc, char *argv[]) {
  // Define accepted command line arguments
  po::options_description desc("Read from iio device");
  desc.add_options()
      ("device,d", po::value<string>()->required(), "iio device to read from");
  desc.add(GeneralOptions()).add(DataSinkOptions());
  
  // Parse command line arguments
  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
  } catch (const std::exception& e) {
    cerr << "Error parsing command line arguments: " << e.what() << endl;
    return EXIT_FAILURE;
  }
  
  // Process help option
  if (vm.count("help")) {
    cout << desc << endl;
    return 0;
  }
  
  // Call notifiers
  try {
    po::notify(vm);
  } catch (const std::exception& e) {
    cerr << "Error processing command line arguments: " << e.what() << endl;
    return EXIT_FAILURE;
  }
  
  DataSinkPtrList data_sinks = BuildDataSinks(vm);
  
  return 0;
}

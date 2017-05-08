/**
 * FDAS device module for reading iio devices.
 */

#include <iostream>

#include <boost/log/trivial.hpp>
#include <iio.h>

#include "common/common.hpp"

namespace po = boost::program_options;

using namespace fdas;
using std::cerr;
using std::cout;
using std::endl;
using std::string;


void ReadBuffer(struct iio_device *dev) {
  
}

int main (int argc, char *argv[]) {
  // Command line arguments
  string device_name;
  
  // Define accepted command line arguments
  po::options_description desc("Read from iio device");
  desc.add_options()
      ("device,d", po::value<string>(&device_name)->required(),
       "iio device to read from");
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
  
  // Call notifiers and process arguments
  try {
    po::notify(vm);
  } catch (const std::exception& e) {
    cerr << "Error processing command line arguments: " << e.what() << endl;
    return EXIT_FAILURE;
  }
  DataSinkPtrList data_sinks = BuildDataSinks(vm);
  
  // Build libiio context
  struct iio_context *ctx = iio_create_local_context();
  if (!ctx) {
    BOOST_LOG_TRIVIAL(error) << "Could not create libiio context";
    return EXIT_FAILURE;
  }

  // Get the iio device
  struct iio_device *dev = iio_context_find_device(ctx, device_name.c_str());
  if (!dev) {
    BOOST_LOG_TRIVIAL(error) << "Could not find iio device `" << device_name
                             << '`';
    return EXIT_FAILURE;
  }

  // Read from the device
  ReadBuffer(dev);

  // Cleanup and exit
  iio_context_destroy(ctx);
  return EXIT_SUCCESS;
}

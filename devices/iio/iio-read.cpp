/**
 * FDAS device module for reading iio devices.
 */

#include <iostream>
#include <cstring>
#include <vector>

#include <boost/any.hpp>
#include <boost/log/trivial.hpp>
#include <iio.h>

#include "common/common.hpp"


namespace po = boost::program_options;

using namespace fdas;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

using boost::any;


any InterpretValue(const struct iio_data_format* fmt, void *data_ptr) {
  if (fmt->is_signed) {
    switch (fmt->length/8) {
      case 1: 
        return *reinterpret_cast<int8_t*>(data_ptr);
      case 2: 
        return *reinterpret_cast<int16_t*>(data_ptr);
      case 4: 
        return *reinterpret_cast<int32_t*>(data_ptr);
      case 8: 
        return *reinterpret_cast<int64_t*>(data_ptr);
      default:
        throw std::runtime_error("Unexpected data length");
    }
  } else {
    switch (fmt->length/8) {
      case 1: 
        return *reinterpret_cast<uint8_t*>(data_ptr);
      case 2: 
        return *reinterpret_cast<uint16_t*>(data_ptr);
      case 4: 
        return *reinterpret_cast<uint32_t*>(data_ptr);
      case 8: 
        return *reinterpret_cast<uint64_t*>(data_ptr);
      default:
        throw std::runtime_error("Unexpected data length");
    }
  }
}

void ReadBuffer(struct iio_device *dev, const DataSinkPtrList &data_sinks,
                size_t buffer_size) {
  struct iio_channel *timestamp_channel = 0;
  vector<struct iio_channel*> channels;
  vector<const struct iio_data_format*> data_formats;
  vector<void*> data_ptrs;
  vector<DataId> data_ids;
  
  for (int i=0; i<iio_device_get_channels_count(dev); i++) {
    struct iio_channel *channel = iio_device_get_channel(dev, i);
    const char* channel_name = iio_channel_get_name(channel);

    // Proceed to next channel if the current cannot be read in the buffer
    if (!iio_channel_is_scan_element(channel))
      continue;
    
    iio_channel_enable(channel);
    if (!std::strcmp(channel_name, "timestamp")) {
      timestamp_channel = channel;
    } else {
      channels.push_back(channel);
      data_formats.push_back(iio_channel_get_data_format(channel));
      data_ids.push_back(DataId(channel_name));
    }
  }
  
  struct iio_buffer *buffer = iio_device_create_buffer(dev, buffer_size, false);
  if (!buffer) {
    BOOST_LOG_TRIVIAL(error) << "Could create iio buffer.";
    return;
  }

  for (;;) {
    // Get data from the buffer
    ssize_t nread = iio_buffer_refill(buffer);
    if (nread < 0) {
      BOOST_LOG_TRIVIAL(error) << "Error refilling iio buffer: "
                               << std::strerror(errno);
      goto cleanup_and_exit;
    }

    ptrdiff_t buffer_step = iio_buffer_step(buffer);
    void *buffer_end = iio_buffer_end(buffer);
    for (int i=0; i<channels.size(); i++) {
      data_ptrs[i] = iio_buffer_first(buffer, channels[i]);
    }
    void *timestamp_data_ptr = iio_buffer_first(buffer, timestamp_channel);

    for (int i=0; i<buffer_size; i++) {
      int64_t timestamp = *reinterpret_cast<int64_t*>(timestamp_data_ptr);
      for (int j=0; i<channels.size(); j++) {
        any value = InterpretValue(data_formats[i], data_ptrs[i]);
        data_ptrs[j] += buffer_step;
        std::cout << boost::any_cast<double>(value) << " ";
      }
      timestamp_data_ptr += buffer_step;
    }

    std::cout << std::endl;
  }
  
cleanup_and_exit:  
  iio_buffer_destroy(buffer);
}

int main (int argc, char *argv[]) {
  // Command line arguments
  string device_name;
  unsigned buffer_size;
  
  // Define accepted command line arguments
  po::options_description desc("Read from iio device");
  desc.add_options()
      ("device,d", po::value<string>(&device_name)->required(),
       "iio device to read from")
      ("buffer-size", po::value<unsigned>(&buffer_size)->default_value(512),
       "iio buffer size (default 512)");
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
  ReadBuffer(dev, data_sinks, buffer_size);

  // Cleanup and exit
  iio_context_destroy(ctx);
  return EXIT_SUCCESS;
}

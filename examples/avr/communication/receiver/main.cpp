
#include <xpcc/architecture.hpp>
#include <xpcc/communication.hpp>
#include <xpcc/communication/xpcc/backend/can/can_connector.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc/driver/can/mcp2515.hpp>

using namespace xpcc::atmega;

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

#include "component_receiver/receiver.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

// ----------------------------------------------------------------------------
// Logging

Uart0 loggerUart(115200);
xpcc::IODeviceWrapper< Uart0 > loggerDevice(loggerUart);

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------
// CAN communication

GPIO__OUTPUT(Cs, B, 4);
GPIO__INPUT(Int, B, 2);

GPIO__OUTPUT(Sclk, B, 7);
GPIO__OUTPUT(Mosi, B, 5);
GPIO__INPUT(Miso, B, 6);

typedef xpcc::SoftwareSpiMaster< Sclk, Mosi, Miso > SpiM;
typedef xpcc::Mcp2515< SpiM, Cs, Int > CanDevice;

static CanDevice device;
static xpcc::CanConnector< CanDevice > connector(&device);

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

// Default filters to receive any extended CAN frame
FLASH_STORAGE(uint8_t canFilter[]) =
{
	MCP2515_FILTER_EXTENDED(0),	// Filter 0
	MCP2515_FILTER_EXTENDED(0),	// Filter 1

	MCP2515_FILTER_EXTENDED(0),	// Filter 2
	MCP2515_FILTER_EXTENDED(0),	// Filter 3
	MCP2515_FILTER_EXTENDED(0),	// Filter 4
	MCP2515_FILTER_EXTENDED(0),	// Filter 5

	MCP2515_FILTER_EXTENDED(0),	// Mask 0
	MCP2515_FILTER_EXTENDED(0),	// Mask 1
};

namespace component
{
	Receiver receiver(robot::component::RECEIVER, &dispatcher);
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	SpiM::initialize();
	Cs::setOutput();
	Int::setInput(Gpio::PullType::PullUp);
	
	// Configure MCP2515 and set the filters
	device.initialize(xpcc::can::BITRATE_125_KBPS);
	device.setFilter(xpcc::accessor::asFlash(canFilter));
	
	// Enable Interrupts
	sei();
	
	XPCC_LOG_INFO << "Welcome to the communication test!" << xpcc::endl; 
	
	while (1)
	{
		// deliver received messages
		dispatcher.update();
		
		component::receiver.update();
		
		xpcc::delay_us(100);
	}
}

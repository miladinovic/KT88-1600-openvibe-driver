#include "ovasCDriverContecKT88.h"
#include "ovasCConfigurationContecKT88.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>
#include <system/Memory.h>
#include <cmath>
#include <iostream>
#include <math.h>

#if defined OVAS_OS_Windows
 #include <windows.h>
 #include <winbase.h>
 #include <cstdio>
 #include <cstdlib>
 #include <commctrl.h>
 #define TERM_SPEED 921600
//   1++ => COM1++
 #define DEFAULT_DEVICE_ID 3
#else
 #include <cstdio>
 #include <unistd.h>
 #include <fcntl.h>
 #include <termios.h>
 #include <sys/time.h>
 #include <sys/select.h>
 #define TERM_SPEED B921600
//   0++ => ttyS0++
//   10++ => ttyUSB0++
 #define DEFAULT_DEVICE_ID 10
#endif

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//																	//

CDriverContecKT88::CDriverContecKT88(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32DeviceIdentifier(uint32(-1))
	,m_ui32SampleCountPerSentBlock(1)
	,m_pSample(NULL)
	,m_ui32TotalSampleCount(0)
	,m_ui32StartTime(0)
{
	m_ui32ChannelCount = 16; // 16  2 REF electrodes
	m_ui16SamplingFrequency = 100;

	m_oHeader.setSamplingFrequency(m_ui16SamplingFrequency);
	m_oHeader.setChannelCount(m_ui32ChannelCount);
	m_oHeader.setChannelName(0,  "Fp1");
	m_oHeader.setChannelName(1,  "Fp2");
	m_oHeader.setChannelName(2,  "F3");
	m_oHeader.setChannelName(3,  "F4");
	m_oHeader.setChannelName(4,  "C3");
	m_oHeader.setChannelName(5,  "C4");
	m_oHeader.setChannelName(6,  "P3");
	m_oHeader.setChannelName(7,  "P4");
	m_oHeader.setChannelName(8,  "O1");
	m_oHeader.setChannelName(9,  "O2");
	m_oHeader.setChannelName(10, "F7");
	m_oHeader.setChannelName(11, "F8");
	m_oHeader.setChannelName(12, "T3");
	m_oHeader.setChannelName(13, "T4");
	m_oHeader.setChannelName(14, "T5");
	m_oHeader.setChannelName(15, "T6");
}

CDriverContecKT88::~CDriverContecKT88(void)
{
}

const char* CDriverContecKT88::getName(void)
{
	return "Contec KT88 1016";
}

//___________________________________________________________________//
//																	//

OpenViBE::boolean CDriverContecKT88::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if (m_rDriverContext.isConnected()) return false;
	if (!m_oHeader.isChannelCountSet()||!m_oHeader.isSamplingFrequencySet()) return false;
	
	// Builds up a buffer to store acquired samples. This buffer will be sent 
	// to the acquisition server later...
	m_pSample = new float32[m_oHeader.getChannelCount() * ui32SampleCountPerSentBlock];
	m_pBuffer = new float32[m_oHeader.getChannelCount()];
	if (!m_pSample || !m_pBuffer)
	{
		delete [] m_pSample;
		delete [] m_pBuffer;
		m_pSample = NULL;
		m_pBuffer = NULL;
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] " << CString(this->getName()) <<": Samples allocation failed.\n";
		return false;
	}
	
	// Initialize hardware
	if (!this->openPort(&m_i32FileDescriptor, m_ui32DeviceIdentifier!=uint32(-1)?m_ui32DeviceIdentifier:DEFAULT_DEVICE_ID))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] " << CString(this->getName()) <<": Could not open port.\n";
		return false;
	}


	// Saves parameters
	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;

	m_rDriverContext.getLogManager() << LogLevel_Debug << CString(this->getName()) << " driver initialized.\n";

	return true;
}

OpenViBE::boolean CDriverContecKT88::start(void)
{
	if (!m_rDriverContext.isConnected()) return false;
	if (m_rDriverContext.isStarted()) return false;

	m_ui32TotalSampleCount = 0;
	m_ui32StartTime = System::Time::getTime();

	m_rDriverContext.getLogManager() << LogLevel_Debug << CString(this->getName()) \
		<< " driver started.\n";

#if defined OVAS_OS_Windows
	// Clear the port buffer: send everything to be sent and remove all read data from the port
	PurgeComm(m_i32FileDescriptor, 15);
	// TODO: Test the following purge
	// PurgeComm(your_handle,PURGE_RXABORT);
#else
	tcdrain(m_i32FileDescriptor); // Wait until everything written has been sent
	tcflush(m_i32FileDescriptor, TCIFLUSH); // Flush data received but not read
#endif

	// Sequence of bytes that tell the device to start recording eeg signals
    const uint8 l_pStartCommand[] = { 0x08, 0x83, 0x88 };

	// Tell the device to start sending data
#if defined OVAS_OS_Windows
	uint32 l_ui32WriteOk;
	WriteFile(m_i32FileDescriptor, &l_pStartCommand, 3, (LPDWORD)&l_ui32WriteOk, NULL);
#else
	ssize_t l_iWriteOk;
	l_iWriteOk = write(m_i32FileDescriptor, &l_pStartCommand, 3);
#endif

	return true;
}

OpenViBE::boolean CDriverContecKT88::loop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "[LOOP] " << \
		CString(this->getName()) <<": Entering loop\n";

	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return true;

	for (uint32 l_ui32SampleIndex = 0; l_ui32SampleIndex < m_ui32SampleCountPerSentBlock; l_ui32SampleIndex++)
	{
		// Get packet and check if it's a valid one
		if (!getPacket(m_i32FileDescriptor))
		{
			// Timeout! Let's abort
			m_rDriverContext.getLogManager() << LogLevel_Error << "[LOOP] " << \
				CString(this->getName()) <<": Timeout getting data from device!\n";
			std::cout << "Timeout in loop" << endl;
			return false;
		}

		// Valid packet! Let's put it into the "m_pSample" array
		for (uint8 l_ui8ChannelIndex = 0; l_ui8ChannelIndex < m_ui32ChannelCount; l_ui8ChannelIndex++)
		{
			uint32 l_ui32Index;
			l_ui32Index = l_ui8ChannelIndex * m_ui32SampleCountPerSentBlock + l_ui32SampleIndex;

			// TODO: Verify why the original Kt88-1016 driver divides the value by 10
			m_pSample[l_ui32Index] = (float32) (m_pBuffer[l_ui8ChannelIndex] / 10);
		}
	}

	m_ui32TotalSampleCount += m_ui32SampleCountPerSentBlock;
	m_pCallback->setSamples(m_pSample);
	m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

	return true;
}

OpenViBE::boolean CDriverContecKT88::stop(void)
{
	if (!m_rDriverContext.isConnected()) return false;
	if (!m_rDriverContext.isStarted()) return false;

	// Tell the device to stop acquiring data
    const uint8 l_pStopCommand[] = { 0xff };
#if defined OVAS_OS_Windows
	uint32 l_ui32WriteOk;
	WriteFile(m_i32FileDescriptor, l_pStopCommand, 1, (LPDWORD)&l_ui32WriteOk, NULL);
#else
	ssize_t l_iWriteOk;
	l_iWriteOk = write(m_i32FileDescriptor, l_pStopCommand, 1);
#endif

	return true;
}

OpenViBE::boolean CDriverContecKT88::uninitialize(void)
{
	if (!m_rDriverContext.isConnected()) return false;
	if (m_rDriverContext.isStarted()) return false;

	// uninitialize hardware here
	this->closePort(m_i32FileDescriptor);
	m_rDriverContext.getLogManager() << LogLevel_Debug << CString(this->getName()) << " driver closed.\n";

	delete [] m_pSample;
	delete [] m_pBuffer;
	m_pSample = NULL;
	m_pBuffer = NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//																	//
OpenViBE::boolean CDriverContecKT88::isConfigurable(void)
{
	return true; // change to false if your device is not configurable
}

OpenViBE::boolean CDriverContecKT88::configure(void)
{
	CConfigurationContecKT88 m_oConfiguration(m_rDriverContext,"../share/openvibe-applications/acquisition-server/interface-Contec-KT88.ui",m_ui32DeviceIdentifier);
	if (!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}
	return true;
}



//____________________________________________________________________________________
// Internal methods
//____________________________________________________________________________________

#if defined OVAS_OS_Windows
#else

ssize_t CDriverContecKT88::getBytes(::FD_TYPE i32FileDescriptor, uint8_t* pReadBuffer, size_t iNumberOfBytesToRead)
{
	struct timeval l_oYet;
	struct timeval l_oAbsoluteTimeout;

	gettimeofday(&l_oAbsoluteTimeout, NULL);
	l_oAbsoluteTimeout.tv_sec = 1; // absolute timeout is one second after start

	size_t l_iNumberOfBytesRead = 0;
	while (l_iNumberOfBytesRead < iNumberOfBytesToRead)
	{
		ssize_t l_iReturnValue;

		struct timeval l_oTimeout;
		l_oTimeout.tv_sec = 0;
		l_oTimeout.tv_usec = 100000; // per-char timeout is 100 ms

		fd_set l_inputFileDescriptorSet;
		FD_ZERO(&l_inputFileDescriptorSet);
		FD_SET(i32FileDescriptor, &l_inputFileDescriptorSet);

		l_iReturnValue = ::select(i32FileDescriptor+1, &l_inputFileDescriptorSet, NULL, NULL, &l_oTimeout);

		if (l_iReturnValue > 0)
		{
			if(FD_ISSET(i32FileDescriptor, &l_inputFileDescriptorSet))
			{
				l_iReturnValue = ::read(i32FileDescriptor, pReadBuffer, iNumberOfBytesToRead - l_iNumberOfBytesRead);
				if (l_iReturnValue > 0)
				{
					l_iNumberOfBytesRead += l_iReturnValue;
				}
			}
		}

		if (l_iReturnValue < 0)
		{
			// Abort due to error
			return l_iReturnValue;
		}

		gettimeofday(&l_oYet, NULL);
		if ((l_oAbsoluteTimeout.tv_sec < l_oYet.tv_sec) ||
			((l_oAbsoluteTimeout.tv_sec == l_oYet.tv_sec) && (l_oAbsoluteTimeout.tv_usec <= l_oYet.tv_usec)))
		{
			// Abort due to timeout
			break;
		}
	}

	return l_iNumberOfBytesRead;
}

#endif


OpenViBE::boolean CDriverContecKT88::getPacket(::FD_TYPE i32FileDescriptor)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "Enters getPacket\n";

	uint8 l_ui8Data;
	uint8 l_ui8LastData;

#if 0
	// TBD: Send 0x83 again to request more l_ui8Data?
	l_ui8Data = 0x83;
#if defined OVAS_OS_Windows
	uint32 l_ui32WriteOk;
	WriteFile(i32FileDescriptor, &l_ui8Data, 1, (LPDWORD)&l_ui32WriteOk, NULL);
#else
	ssize_t wrtval;
	retval = write(i32FileDescriptor, &l_ui8Data, 1);
#endif
#endif

	// Let's sync to the device: skip bytes until we get the two start bytes (255)
#if defined OVAS_OS_Windows
	uint32 l_ui32ReadOk;
	ReadFile(i32FileDescriptor, &l_ui8Data, 1, (LPDWORD)&l_ui32ReadOk, NULL);
#else
	ssize_t l_iReadOk;
	l_iReadOk = getBytes(i32FileDescriptor, &l_ui8Data, 1);
#endif

	uint8 l_ui8UnsynchronizedBytesAllowed = 100;
	while (--l_ui8UnsynchronizedBytesAllowed > 0)
	{
		l_ui8LastData = l_ui8Data;
#if defined OVAS_OS_Windows
		ReadFile(i32FileDescriptor, &l_ui8Data, 1, (LPDWORD)&l_ui32ReadOk, NULL);
#else
		l_iReadOk = getBytes(i32FileDescriptor, &l_ui8Data, 1);
#endif

		if (l_ui8Data == 255 && l_ui8LastData == 255)
		{
			// Start bytes found
			break;
		}
	}

	if (l_ui8UnsynchronizedBytesAllowed == 0)
	{
		return false;
	}

	// Each channel value is formed by two consecutive bytes, the first byte is
	// the least significant one and the second the most significant one. That means
	// we need to write both bytes into a 16 bit unsigned integer and swap its byte
	// order
	uint8 l_pBytesRead[2];
	for (uint32 channelNumber = 0; channelNumber < m_ui32ChannelCount; channelNumber)
	{
		uint16 l_ui16Value;

		// Read two bytes
#if defined OVAS_OS_Windows
		ReadFile(i32FileDescriptor, l_pBytesRead, 2, (LPDWORD)&l_ui32ReadOk, NULL);
#else
		l_iReadOk = getBytes(i32FileDescriptor, l_pBytesRead, 2);
#endif

		// Chinese way of arranging the eeg values into two bytes...
		// First byte represents the 12 most significant bits and the second one
		// represents the 4 least significant bits
		l_ui16Value = (((uint16)l_pBytesRead[0]) << 4) | (l_pBytesRead[1] & 0xF);

		// This device has a 12 bit resolution, which means it can output values from
		// 0 to 2^12-1, in other words, 0 to 4095.
		// A signal from this eeg device can vary from -N/2 to N/2 - 1, where N is 
		// device resolution, which means it can output signal values ranging from
		// -2048 to 2047. Therefore, we extract 2048 from the read value.
		m_pBuffer[channelNumber] = (float32) (l_ui16Value - 2048);
	}

	return true;
}



OpenViBE::boolean CDriverContecKT88::openPort(::FD_TYPE* pFileDescriptor, uint32 ui32TTYNumber)
{
	char l_sTTYName[1024];

#if defined OVAS_OS_Windows

	::sprintf(l_sTTYName, "\\\\.\\COM%d", ui32TTYNumber);
	*pFileDescriptor=::CreateFile(
		(LPCSTR)l_sTTYName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	// Check if port has been open
	if(*pFileDescriptor == INVALID_HANDLE_VALUE)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not open port [" << CString(l_sTTYName) << "]\n";
		return false;
	}

	// Variable to store COM port settings
	DCB l_oDCB = {0};

	if(!::GetCommState(*pFileDescriptor, &l_oDCB))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not get comm state on port [" << CString(l_sTTYName) << "]\n";
		return false;
	}

	// update DCB rate, byte size, parity, and stop bits size
	l_oDCB.DCBlength = sizeof(l_oDCB);
	l_oDCB.BaudRate  = TERM_SPEED;
	l_oDCB.ByteSize  = 8;
	l_oDCB.Parity    = NOPARITY;
	l_oDCB.StopBits  = ONESTOPBIT;
	l_oDCB.EvtChar   = '\0';

	// update flow control settings
	l_oDCB.fDtrControl		  = DTR_CONTROL_HANDSHAKE;
	l_oDCB.fRtsControl		  = RTS_CONTROL_HANDSHAKE;
	l_oDCB.fOutxCtsFlow	  = FALSE;
	l_oDCB.fOutxDsrFlow	  = FALSE;
	l_oDCB.fDsrSensitivity	  = FALSE;;
	l_oDCB.fOutX			  = FALSE;
	l_oDCB.fInX			  = FALSE;
	l_oDCB.fTXContinueOnXoff = FALSE;
	l_oDCB.XonChar			  = 17;
	l_oDCB.XoffChar		  = 19;
	l_oDCB.XonLim			  = 28672;
	l_oDCB.XoffLim			  = 7168;
	l_oDCB.fParity			  = FALSE;

	// Set port timeouts
	COMMTIMEOUTS l_oCommTimeouts;

	l_oCommTimeouts.ReadIntervalTimeout = 100;
	l_oCommTimeouts.ReadTotalTimeoutMultiplier = 1;
	l_oCommTimeouts.ReadTotalTimeoutConstant = 500;
	l_oCommTimeouts.WriteTotalTimeoutMultiplier = 1;
	l_oCommTimeouts.WriteTotalTimeoutConstant = 100;
	
	::SetCommState(*pFileDescriptor, &dcb);
	::SetCommTimeouts(*pFileDescriptor, &l_oCommTimeouts);
	::SetupComm(*pFileDescriptor, 4096, 512);
	::EscapeCommFunction(*pFileDescriptor, SETRTS);
	::EscapeCommFunction(*pFileDescriptor, SETDTR);
	::SetCommMask (*pFileDescriptor, EV_RXCHAR | EV_CTS | EV_DSR | EV_RLSD | EV_RING);

#else
	struct termios l_oTerminalAttributes;

	// open ttyS<i> for i < 10, else open ttyUSB<i-10>
	if(ui32TTYNumber<10)
	{
		::sprintf(l_sTTYName, "/dev/ttyS%d", ui32TTYNumber);
	}
	else
	{
		::sprintf(l_sTTYName, "/dev/ttyUSB%d", ui32TTYNumber-10);
	}

	if((*pFileDescriptor=::open(l_sTTYName, O_RDWR))==-1)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not open port [" << CString(l_sTTYName) << "]\n";
		return false;
	}

	if(::tcgetattr(*pFileDescriptor, &l_oTerminalAttributes)!=0)
	{
		::close(*pFileDescriptor);
		*pFileDescriptor=-1;
		m_rDriverContext.getLogManager() << LogLevel_Error << "terminal: tcgetattr() failed - did you use the right port [" << CString(l_sTTYName) << "] ?\n";
		return false;
	}

	// cfmakeraw
	l_oTerminalAttributes.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
		| INLCR | IGNCR | ICRNL | IXON);
	l_oTerminalAttributes.c_oflag &= ~OPOST;
	l_oTerminalAttributes.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	l_oTerminalAttributes.c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
	l_oTerminalAttributes.c_cflag |= CS8 | CLOCAL;

	// cfsetspeed 
	cfsetispeed(&l_oTerminalAttributes, TERM_SPEED);
	cfsetospeed(&l_oTerminalAttributes, TERM_SPEED);

	if(::tcsetattr(*pFileDescriptor, TCSAFLUSH, &l_oTerminalAttributes)!=0)
	{
		::close(*pFileDescriptor);
		*pFileDescriptor=-1;
		m_rDriverContext.getLogManager() << LogLevel_Error << "terminal: tcsetattr() failed - did you use the right port [" << CString(l_sTTYName) << "] ?\n";
		return false;
	}

#endif

	m_sTTYName = l_sTTYName;

	return true;
}

void CDriverContecKT88::closePort(::FD_TYPE i32FileDescriptor)
{
#if defined OVAS_OS_Windows
	::CloseHandle(i32FileDescriptor);
#else
	::close(i32FileDescriptor);
#endif
}
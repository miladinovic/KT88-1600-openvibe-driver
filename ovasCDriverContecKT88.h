#ifndef __OpenViBE_AcquisitionServer_CDriverContecKT88_H__
#define __OpenViBE_AcquisitionServer_CDriverContecKT88_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"
#include <openvibe/ov_all.h>

#if defined OVAS_OS_Windows
 typedef void * FD_TYPE;
#elif defined OVAS_OS_Linux
 typedef int FD_TYPE;
 #include <stdint.h>
 #include <unistd.h>
#else
#endif

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverContecKT88
	 * \author Emerson Lopes Machado (bmi.machado@gmail.com)
	 * \date Fri Mar 11 11:13:15 2011
	 * \erief The CDriverContecKT88 allows the acquisition server to acquire data from a Contec KT88 1016 device.
	 *
	 * This is a new driver for the Contec's KT88-1016 based on the explanations
	 * and code from Alexandra Elbakyan (mindwrapper@gmail.com). The explanations can be found at:
	 * http://engineuring.wordpress.com/2009/06/15/writing-your-own-soft-for-a-really-cheap-eeg-hardware-for-brain-computer-interfacing/
	 * and her code at: http://tech.groups.yahoo.com/group/conteckt88/files/
	 * 
	 * \sa CConfigurationContecKT88
	 */
	class CDriverContecKT88 : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverContecKT88(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverContecKT88(void);
		virtual const char* getName(void);

		virtual OpenViBE::boolean initialize(
			const OpenViBE::uint32 ui32SampleCountPerSentBlock,
			OpenViBEAcquisitionServer::IDriverCallback& rCallback);
		virtual OpenViBE::boolean uninitialize(void);

		virtual OpenViBE::boolean start(void);
		virtual OpenViBE::boolean stop(void);
		virtual OpenViBE::boolean loop(void);

		virtual OpenViBE::boolean isConfigurable(void);
		virtual OpenViBE::boolean configure(void);
		virtual const OpenViBEAcquisitionServer::IHeader* getHeader(void) { return &m_oHeader; }
		
		virtual OpenViBE::boolean isFlagSet(
			const OpenViBEAcquisitionServer::EDriverFlag eFlag) const
		{
			//return eFlag==DriverFlag_IsUnstable;
			return false;
		}

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32ChannelCount;
		OpenViBE::uint32 m_ui32DeviceIdentifier;
		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;
		OpenViBE::float32* m_pBuffer;

		::FD_TYPE  m_i32FileDescriptor;

		OpenViBE::uint32 m_ui32TotalSampleCount;
		OpenViBE::uint32 m_ui32StartTime;
	
	private:

		OpenViBE::uint16 m_ui16SamplingFrequency;
#if defined OVAS_OS_Linux
        ssize_t getBytes(::FD_TYPE i32FileDescriptor, uint8_t* buffer, size_t len);
#endif
		OpenViBE::boolean getPacket(::FD_TYPE i32FileDescriptor);
		OpenViBE::boolean openPort(::FD_TYPE* pFileDescriptor, OpenViBE::uint32 ui32TTYNumber);
		void closePort(::FD_TYPE i32FileDescriptor);


		OpenViBE::CString m_sTTYName;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverContecKT88_H__
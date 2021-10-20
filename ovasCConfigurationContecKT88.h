#ifndef __OpenViBE_AcquisitionServer_CConfigurationContecKT88_H__
#define __OpenViBE_AcquisitionServer_CConfigurationContecKT88_H__

#include "../ovasCConfigurationBuilder.h"
#include "../ovasIDriver.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CConfigurationContecKT88
	 * \author Emerson Lopes Machado ()
	 * \date Fri Mar 11 11:13:15 2011
	 * \erief The CConfigurationContecKT88 handles the configuration dialog specific to the Contec KT88 1016 device.
	 *
	 * TODO: details
	 *
	 * \sa CDriverContecKT88
	 */
	class CConfigurationContecKT88 : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationContecKT88(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName, OpenViBE::uint32& rUSBIndex);
        ~CConfigurationContecKT88();

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;
		OpenViBE::uint32& m_rUSBIndex;
		::GtkListStore* m_pListStore;

	private:

		/*
		 * Insert here all specific attributes, such as a connection ID.
		 * Example:
		 */
		// OpenViBE::uint32 m_ui32ConnectionID;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationContecKT88_H__

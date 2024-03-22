#pragma once
#include <driver_init.h>
namespace spi
{
	inline void begin(uint32_t baud)
	{
		const uint32_t SPI_CLK_FREQ = 48000000;
		uint16_t BAUD_REG = ((float)SPI_CLK_FREQ / (float)(2 * baud)) - 1;	//Calculate BAUD value
		SERCOM4->SPI.BAUD.reg = SERCOM_SPI_BAUD_BAUD(BAUD_REG);				//Set the SPI baud rate	
		SERCOM4->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;					//Enable the Sercom SPI
		spi_m_sync_enable(&SPI_0);
	}

	inline uint8_t transfer(uint8_t data)
	{
		while(SERCOM4->SPI.INTFLAG.bit.DRE == 0);
		SERCOM4->SPI.DATA.reg = data;
		while(SERCOM4->SPI.INTFLAG.bit.RXC == 0);
		return (uint8_t)SERCOM4->SPI.DATA.reg;
	}

	//#define SPI_SERCOM_PINS_PORT_GROUP	1
	//#define SPI_MISO_PORT_PMUX			PORT_PMUX_PMUXE_C_Val
	//#define SPI_SERCOM_MISO_PIN			PORT_PB16
	//#define SPI_SCK_MOSI_PORT_PMUX		PORT_PMUX_PMUXE_D_Val
	//#define SPI_SERCOM_MOSI_PIN			PORT_PB22
	//#define SPI_SERCOM_SCK_PIN			PORT_PB23
	//
	//#define SPI_SERCOM_CLK_GEN			0
	//#define SPI_SERCOM					SERCOM4
	//#define SPI_CLK_FREQ				8000000
	//#define SPI_BAUD					50000	
	//
	//	void begin(void)
	//	{
	//		/* Wait for synchronization */
	//		while (SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE);
	//
	//		/* Disable the SERCOM SPI module */
	//		SPI_SERCOM->SPI.CTRLA.bit.ENABLE = 0;
	//
	//		/* Wait for synchronization */
	//		while (SPI_SERCOM->SPI.SYNCBUSY.bit.SWRST);
	//
	//		/* Perform a software reset */
	//		SPI_SERCOM->SPI.CTRLA.bit.SWRST = 1;
	//
	//		/* Wait for synchronization */
	//		while (SPI_SERCOM->SPI.CTRLA.bit.SWRST);
	//
	//		/* Wait for synchronization */
	//		while (SPI_SERCOM->SPI.SYNCBUSY.bit.SWRST || SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE);
	//
	//		//Using the WRCONFIG register to bulk configure PB16 for being configured the SERCOM4 SPI MASTER MISO
	//		PORT->Group[SPI_SERCOM_PINS_PORT_GROUP].WRCONFIG.reg =
	//			PORT_WRCONFIG_WRPINCFG |											//Enables the configuration of PINCFG
	//			PORT_WRCONFIG_WRPMUX |												//Enables the configuration of the PMUX for the selected pins
	//			PORT_WRCONFIG_PMUXEN |												//Enables the PMUX for the pins
	//			PORT_WRCONFIG_PMUX(MUX_PB16C_SERCOM5_PAD0) |						//Bulk configuration for PMUX "C" for SERCOM4
	//			PORT_WRCONFIG_HWSEL |												//Select the correct pin configurations for 16-31
	//			PORT_WRCONFIG_INEN |												//Enable input on this pin MISO
	//			PORT_WRCONFIG_PINMASK((uint16_t)((PORT_PB16) >> 16));				//Selecting which pin is configured  PB16  This bit needs to shift to fit the 16 bit macro requirements
	//
	//		//Using the WRCONFIG register to bulk configure both PB22 and PB23 for being configured the SERCOM4 SPI MASTER MOSI and SCK pins
	//		PORT->Group[SPI_SERCOM_PINS_PORT_GROUP].WRCONFIG.reg =
	//			PORT_WRCONFIG_WRPINCFG |											//Enables the configuration of PINCFG
	//			PORT_WRCONFIG_WRPMUX |												//Enables the configuration of the PMUX for the selected pins
	//			PORT_WRCONFIG_PMUX(MUX_PB22D_SERCOM5_PAD2) |						//Bulk configuration for PMUX
	//			PORT_WRCONFIG_PMUXEN |												//Enables the PMUX for the pins
	//			PORT_WRCONFIG_HWSEL |												//Select the correct pin configurations for 16-31
	//			PORT_WRCONFIG_PINMASK((uint16_t)((SPI_SERCOM_MOSI_PIN | SPI_SERCOM_SCK_PIN) >> 16));	//Selecting which pin is configured
	//
	//		PM->APBCMASK.reg |= PM_APBCMASK_SERCOM5;								//Enable the SERCOM 5 under the PM
	//
	//		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM5_GCLK_ID_CORE) |				//Provide necessary clocks to the peripheral
	//			GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(SPI_SERCOM_CLK_GEN);
	//
	//		while (GCLK->STATUS.bit.SYNCBUSY);										//Wait for clock sync
	//
	//		SPI_SERCOM->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE_SPI_MASTER |			//Configure the Peripheral as SPI Master
	//			SERCOM_SPI_CTRLA_DOPO(1);												//DOPO is set to PAD[2,3]
	//
	//
	//		SPI_SERCOM->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;						//Enable receive on SPI
	//
	//		uint16_t BAUD_REG = ((float)SPI_CLK_FREQ / (float)(2 * SPI_BAUD)) - 1;	//Calculate BAUD value
	//		SPI_SERCOM->SPI.BAUD.reg = SERCOM_SPI_BAUD_BAUD(BAUD_REG);				//Set the SPI baud rate	
	//		SPI_SERCOM->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;					//Enable the Sercom SPI
	//		while (SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE);								//What for the enable to finish
	//
	//	}
	//
	//	uint8_t transfer(uint8_t data)
	//	{
	//		while (SPI_SERCOM->SPI.INTFLAG.bit.DRE == 0);
	//		SPI_SERCOM->SPI.DATA.reg = data;
	//		while (SPI_SERCOM->SPI.INTFLAG.bit.RXC == 0);
	//		return (uint8_t)SPI_SERCOM->SPI.DATA.reg;
	//	}

}
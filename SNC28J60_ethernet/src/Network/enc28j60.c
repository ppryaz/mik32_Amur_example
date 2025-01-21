//********************************************************************************************
//
// File : enc28j60.c Microchip ENC28J60 Ethernet Interface Driver
//
//********************************************************************************************
//
// Copyright (C) 2007
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
// This program is distributed in the hope that it will be useful, but
//
// WITHOUT ANY WARRANTY;
//
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin St, Fifth Floor, Boston, MA 02110, USA
//
// http://www.gnu.de/gpl-ger.html
//
//********************************************************************************************
#include "enc28j60.h"
#include "util.h"
//

//struct enc28j60_flag
//{
//	unsigned rx_buffer_is_free:1;
//	unsigned unuse:7;
//}enc28j60_flag;
static uint8_t Enc28j60Bank;
static uint16_t next_packet_ptr;

static uint8_t Enc28j60Bank;
static uint8_t erxfcon;


//*******************************************************************************************
//
// Function : enc28j60ReadOp
//
//*******************************************************************************************
uint8_t enc28j60ReadOp(uint8_t op, uint8_t address) {

	uint8_t result;
	CSACTIVE;											// Select SPI device

	snc28j60_SPI_SendByte(op|(address & ADDR_MASK)); 	// Issue read command
	snc28j60_SPI_SendByte(0x00); 						// Skip a false byte

	if(address & 0x80) snc28j60_SPI_ReceiveByte();	// do dummy read if needed (for mac and mii, see datasheet page 29)

	result = snc28j60_SPI_ReceiveByte();				// Take receive byte
	CSPASSIVE;											// Deselect SPI device

	return result;
}
//*******************************************************************************************
//
// Function : enc28j60WriteOp
// Description : Provided write operation
//
//*******************************************************************************************

void enc28j60WriteOp(uint8_t op, uint8_t address, uint8_t data) {
	CSACTIVE;
	snc28j60_SPI_SendByte(op|(address & ADDR_MASK));
	snc28j60_SPI_SendByte(data);

	CSPASSIVE;
}
//*******************************************************************************************
//
// Function : enc28j60SetBank
// Description : Set the bank from  which the work will be carried out.
// If the current bank and the target as a same, the function does nothing.
//
//*******************************************************************************************

void enc28j60SetBank(uint8_t address) {
	// Set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank) 	{
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		Enc28j60Bank = (address & BANK_MASK);

	}
	uint8_t bank = Enc28j60Bank;
}
//-------------------------------------------------------------------------------------------
//
//	Function : enc28j60_SPI_ReadWrite
//	Description : SPI read write data from SPI interface
//
//-------------------------------------------------------------------------------------------

uint8_t enc28j60_SPI_ReadWrite(uint8_t byte) {
	uint8_t receivedByte = 0;
	HAL_StatusTypeDef res;

	res = HAL_SPI_Exchange(&hspi1, &byte , &receivedByte, 1, SPI_TIMEOUT_DEFAULT);
	if (res != HAL_OK) {
		xprintf("Error exchange SPI, res: %d", res);
	}
	return receivedByte;
}
//-------------------------------------------------------------------------------------------
//
//	Function : snc28j60_SPI_SendByte
//	Description : Send byte from SPI
//
//-------------------------------------------------------------------------------------------

void snc28j60_SPI_SendByte(uint8_t bt) {
	enc28j60_SPI_ReadWrite(bt);
}
//-------------------------------------------------------------------------------------------
//
//	Function : snc28j60_SPI_ReceiveByte
//	Description : Receive byte from SPI
//
//-------------------------------------------------------------------------------------------

uint8_t snc28j60_SPI_ReceiveByte(void) {
	uint8_t bt = enc28j60_SPI_ReadWrite(0xFF);
	return  bt;
}
//-------------------------------------------------------------------------------------------
//
//	Function : enc28j60_readBuf
//	Description : Reading the received data from the memory of the Ethernet module into the
//  buffer for further processing
//
//-------------------------------------------------------------------------------------------

static void enc28j60_readBuf(uint16_t len, uint8_t* data) {
	CSACTIVE;

	snc28j60_SPI_SendByte(ENC28J60_READ_BUF_MEM);
	while(len--) {
		*data++ = enc28j60_SPI_ReadWrite(0x00);
	}
	CSPASSIVE;
}
//-------------------------------------------------------------------------------------------
//
//	Function : enc28j60_writeBuf
//	Description : Writing the transmitting data to the memory of the Ethernet module for the
//  it's internal buffer for transmit data packet.
//
//-------------------------------------------------------------------------------------------
static void enc28j60_writeBuf(uint16_t len, uint8_t *data) {
	CSACTIVE;

	snc28j60_SPI_SendByte(ENC28J60_WRITE_BUF_MEM);
	while (len--) {
		snc28j60_SPI_SendByte(*data++);
	}
	CSPASSIVE;
}
//------------------------------------------------------------------------------------------
//
//	Function : enc28j60_writeRegByte
//	Description : Universal function for write  control register
//
//------------------------------------------------------------------------------------------

static void enc28j60_writeRegByte(uint8_t address, uint8_t data) {
	enc28j60SetBank(address);
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}
//------------------------------------------------------------------------------------------
//
//	Function : enc28j60_readRegByte
//	Description : Universal function for read  control register
//
//------------------------------------------------------------------------------------------

static uint8_t enc28j60_readRegByte(uint8_t address) {
	enc28j60SetBank(address);
	uint8_t res = enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address); //????
	return res;
}
//------------------------------------------------------------------------------------------
//
//	Function : enc28j60_writeReg
//	Description : This function provided   the ability to write uint16_t data to  a two-byte register
//
//------------------------------------------------------------------------------------------

static void enc28j60_writeReg(uint8_t address, uint16_t data) {
	enc28j60_writeRegByte(address, data);
	enc28j60_writeRegByte(address + 1, (data >> 8));
}
//------------------------------------------------------------------------------------------
//
//	Function : enc28j60Read
//	Description : This function provided   the ability to read control register
//
//------------------------------------------------------------------------------------------

uint8_t enc28j60Read(uint8_t address) {

	enc28j60SetBank(address);								// Select bank to read
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address); // Do the read
}
//------------------------------------------------------------------------------------------
//
//	Function : enc28j60Write
//	Description : This function provided   the ability to write into control register
//
//------------------------------------------------------------------------------------------

void enc28j60Write(uint8_t address, uint8_t data) {

	enc28j60SetBank(address); 									// Select bank to write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);	// Do the write
}
//*******************************************************************************************
//
//	Function : enc28j60_read_phyreg
//	Description : Read PHY register
//
//*******************************************************************************************
uint16_t enc28j60_read_phyreg(uint8_t address) {
	uint16_t data;

	// Set the PHY register address
	enc28j60Write(MIREGADR, address);
	enc28j60Write(MICMD, MICMD_MIIRD);

	// Loop to wait until the PHY register has been read through the MII
	// This requires 10.24us
	while( (enc28j60Read(MISTAT) & MISTAT_BUSY) );

	// Stop reading
	enc28j60Write(MICMD, MICMD_MIIRD);

	// Obtain results and return
	data = enc28j60Read ( MIRDL );
	data |= enc28j60Read ( MIRDH );

	return data;
}
//*******************************************************************************************
//
//	Function : enc28j60PhyWrite
//	Description : Write into PHY register
//
//*******************************************************************************************
void enc28j60PhyWrite(uint8_t address, uint16_t data) {

	enc28j60_writeRegByte(MIREGADR, address);	// Set the PHY register address
	enc28j60_writeReg(MIWRL, data);				// Write the PHY data

	// wait until the PHY write completes

	while(enc28j60_readRegByte(MISTAT) & MISTAT_BUSY);  for(uint32_t i = 0 ; i < 2; ++i); // Need Delay here
}
//*******************************************************************************************
//
// Function : enc28j60PhyWrite
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************

void enc28j60_init() {
	enc28j60Read(ESTAT);		// It is neccssary to substract the case before starting initialization

	//uint16_t revision = enc28j60_readRegByte(EREVID);
	xprintf("\r\nRevision: %d", enc28j60getrev());

	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);	// Soft reset, need delay 50m after
	for (uint32_t i = 0; i < 10; ++i); //HAL_Delay(50);

	// check CLKRDY bit to see if reset is complete
	// The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
	while (!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));

	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// set receive buffer start address
	next_packet_ptr = RXSTART_INIT;
	// Rx start
	enc28j60_writeReg(ERXSTL, RXSTART_INIT);
	// set receive pointer address
	enc28j60_writeReg(ERXRDPTL, RXSTART_INIT);
	// RX end
	enc28j60_writeReg(ERXNDL, RXSTOP_INIT);
	// TX start
	enc28j60_writeReg(ETXSTL, TXSTART_INIT);
	// TX end
	enc28j60_writeReg(ETXNDL, TXSTOP_INIT);

	// do bank 1 stuff, packet filter:
	// For broadcast packets we allow only ARP packtets
	// All other packets should be unicast only for our mac (MAADR)
	//
	// The pattern to match on is therefore
	// Type     ETH.DST
	// ARP      BROADCAST
	// 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	// in binary these poitions are:11 0000 0011 1111
	// This is hex 303F->EPMM0=0x3f,EPMM1=0x30

	erxfcon = ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN;
	enc28j60_writeRegByte(ERXFCON, erxfcon);

	// do bank 2 stuff

	enc28j60_writeRegByte(MACON1,
			MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);	// Enable MAC receive
	enc28j60Write(MACON2, 0x00);					// Bring MAC out of reset
	// enable automatic padding to 60bytes and CRC operations
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3,
			MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
	// set inter-frame gap (non-back-to-back)
	enc28j60_writeReg(MAIPGL, 0x0C12);
	// set inter-frame gap (back-to-back)
	enc28j60_writeRegByte(MABBIPG, 0x12);
	// Set the maximum packet size which the controller will accept
	// Do not send packets longer than MAX_FRAMELEN:
	enc28j60_writeReg(MAMXFLL, MAX_FRAMELEN);
	// do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	enc28j60_writeRegByte(MAADR5, macaddr[0]);
	enc28j60_writeRegByte(MAADR4, macaddr[1]);
	enc28j60_writeRegByte(MAADR3, macaddr[2]);
	enc28j60_writeRegByte(MAADR2, macaddr[3]);
	enc28j60_writeRegByte(MAADR1, macaddr[4]);
	enc28j60_writeRegByte(MAADR0, macaddr[5]);

	//uint8_t test = enc28j60_readRegByte(MISTAT);

	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);	// Disable loopback of transmitted frames

	enc28j60PhyWrite(PHLCON, PHLCON_LACFG2_BIT
							| PHLCON_LBCFG2_BIT
							| PHLCON_LBCFG1_BIT
							| PHLCON_LBCFG0_BIT
							| PHLCON_LFRQ0_BIT
							| PHLCON_STRCH_BIT);


	enc28j60SetBank(ECON1);												// Switch to bank 0
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);	// Enable interrutps

	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);			// Enable packet reception}

}
//*******************************************************************************************
//
// Function : enc28j60getrev
// Description : read the revision of the chip.
//
//*******************************************************************************************

uint8_t enc28j60getrev(void) {
	return(enc28j60Read(EREVID));
}
//*******************************************************************************************
//
// Function : enc28j60_packet_send
// Description : Send packet to network.
//
//*******************************************************************************************

void enc28j60_packet_send ( uint8_t* buffer, uint16_t length ) {

	while(enc28j60ReadOp(ENC28J60_READ_CTRL_REG, ECON1)&ECON1_TXRTS) {
		if (enc28j60_readRegByte(EIR) & EIR_TXERIF) {
			enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1,  ECON1_TXRST);
			enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		}
	}
	enc28j60_writeReg(EWRPTL, TXSTART_INIT); // Set start pointer
	enc28j60_writeReg(ETXSTL, TXSTART_INIT);

	enc28j60_writeBuf(1, (uint8_t*)"0x00");	//
	enc28j60_writeBuf(length, buffer);

	enc28j60_writeReg(ETXNDL, TXSTART_INIT + length);
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1,  ECON1_TXRTS);

	uint8_t res = enc28j60_readRegByte(ESTAT);
	if (res & ESTAT_TXABRT) {
		xprintf("Transmit error< packet not send\r\n");
	}

	if( (enc28j60Read(EIR) & EIR_TXERIF) )
	{
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
	}


}
//*******************************************************************************************
//
// 	Function : enc28j60_mac_is_linked
// 	Description : return MAC link status.
//
//*******************************************************************************************

uint8_t enc28j60_mac_is_linked(void) {
	if ( (enc28j60_read_phyreg(PHSTAT1) & PHSTAT1_LLSTAT ) )
		return 1;
	else
		return 0;
}
//*******************************************************************************************
//
// Function : enc28j60_packet_receive
// Description : check received packet and return length of data
//
//*******************************************************************************************

unsigned short enc28j60_packet_receive ( unsigned char *rxtx_buffer, uint16_t max_length ) {
	unsigned short rx_status;
	uint16_t data_length;

	// check if a packet has been received and buffered
	// if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
	// The above does not work. See Rev. B4 Silicon Errata point 6.
	if( enc28j60_readRegByte(EPKTCNT) == 0 ) {
		return 0;
	}

	// Set the read pointer to the start of the received packet
	enc28j60_writeReg(ERDPTL, next_packet_ptr);

	struct {
		uint16_t nextPacket;
		uint16_t byteCount;
		uint16_t status;
	} header;

	enc28j60_readBuf(sizeof(header), (uint8_t*)&header);
	next_packet_ptr = header.nextPacket;

	data_length = header.byteCount -4; //	Remove the CRC count
	if (data_length > max_length) data_length = max_length;

	if ((header.status & 0x80) == 0) {
		data_length = 0;
	} else {
		enc28j60_readBuf(data_length, rxtx_buffer);
	}
	rxtx_buffer[data_length] = 0;

	if ((next_packet_ptr - 1) > RXSTOP_INIT) {
		enc28j60_writeReg(ERXRDPTL, RXSTOP_INIT);
	} else {
		enc28j60_writeReg(ERXRDPTL, (next_packet_ptr - 1));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	}

	return data_length;
}


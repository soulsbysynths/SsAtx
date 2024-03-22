/* 
* stk500.cpp
*
* Created: 13/01/2019 14:26:50
* Author: info
*/


#include "stk500.h"

// default constructor
stk500::stk500()
{
} //stk500

// default destructor
stk500::~stk500()
{
} //~stk500

/*
* avrdude interface for Atmel STK500 programmer
*
* Note: most commands use the "universal command" feature of the
* programmer in a "pass through" mode, exceptions are "program
* enable", "paged read", and "paged write".
*
*/
#include "stk500.h"

int stk500::send(byte *buf, unsigned int len)
{

	Serial1.write(buf,len);
}
int stk500::recv(byte * buf, unsigned int len)
{
	int rv;

	
	rv =Serial1.readBytes((char *)buf,len);
	if (rv < 0) 
	{
		stk500::error2(ERRORNOPGMR);
		return -1;
	}
	return 0;
}
int stk500::drain()
{
	while (Serial1.available()> 0)
	{
		//DEBUGP("draining: ");
		Serial1.read();
	}
	return 1;
}
int stk500::getsync()
{
	byte buf[32], resp[32];

	/*
	* get in sync */
	buf[0] = Cmnd_STK_GET_SYNC;
	buf[1] = Sync_CRC_EOP;
	
	/*
	* First send and drain a few times to get rid of line noise
	*/
	
	send(buf, 2);
	drain();
	send(buf, 2);
	drain();
	
	delay(100);		//soulsby add, not in avrdude, so not sure why needed
	send(buf, 2);
	if (recv(resp, 1) < 0) return -1;
	if (resp[0] != Resp_STK_INSYNC) 
	{
		stk500::error1(ERRORPROTOSYNC,resp[0]);
		drain();
		return -1;
	}

	if (recv(resp, 1) < 0) return -1;
	if (resp[0] != Resp_STK_OK) 
	{
		stk500::error1(ERRORNOTOK,resp[0]);
		return -1;
	}
	return 0;
}

int stk500::getparm(unsigned parm, unsigned * value)
{
	byte buf[16];
	unsigned v;
	int tries = 0;

	retry:
	tries++;
	buf[0] = Cmnd_STK_GET_PARAMETER;
	buf[1] = parm;
	buf[2] = Sync_CRC_EOP;

	send(buf, 3);

	if (recv(buf, 1) < 0) exit(1);
	if (buf[0] == Resp_STK_NOSYNC) 
	{
		if (tries > 33) 
		{
			stk500::error2(ERRORNOSYNC);
			return -1;
		}
		if (getsync() < 0)
		return -1;
		
		goto retry;
	}
	else if (buf[0] != Resp_STK_INSYNC) 
	{
		stk500::error1(ERRORPROTOSYNC,buf[0]);
		return -2;
	}

	if (recv(buf, 1) < 0) exit(1);
	v = buf[0];

	if (recv(buf, 1) < 0) exit(1);
	if (buf[0] == Resp_STK_FAILED) 
	{
		stk500::error1(ERRORPARMFAILED,v);
		return -3;
	}
	else if (buf[0] != Resp_STK_OK) 
	{
		stk500::error1(ERRORNOTOK,buf[0]);
		return -3;
	}

	*value = v;

	return 0;
}
/* read signature bytes - arduino version */
int stk500::arduino_read_sig_bytes(AVRMEM * m)
{
	unsigned char buf[32];

	/* Signature byte reads are always 3 bytes. */

	if (m->size < 3) 
	{
		//DEBUGPLN("memsize too small for sig byte read");
		return -1;
	}

	buf[0] = Cmnd_STK_READ_SIGN;
	buf[1] = Sync_CRC_EOP;

	send(buf, 2);

	if (recv(buf, 5) < 0)
	return -1;
	if (buf[0] == Resp_STK_NOSYNC) 
	{
		stk500::error2(ERRORNOSYNC);
		return -1;
	} 
	else if (buf[0] != Resp_STK_INSYNC) 
	{
		stk500::error1(ERRORPROTOSYNC,buf[0]);
		return -2;
	}
	if (buf[4] != Resp_STK_OK) 
	{
		stk500::error1(ERRORNOTOK,buf[4]);
		return -3;
	}

	m->buf[0] = buf[1];
	m->buf[1] = buf[2];
	m->buf[2] = buf[3];

	return 3;
}

int stk500::loadaddr(unsigned int addr)
{
	unsigned char buf[16];
	int tries;

	tries = 0;
	retry:
	tries++;
	buf[0] = Cmnd_STK_LOAD_ADDRESS;
	buf[1] = addr & 0xff;
	buf[2] = (addr >> 8) & 0xff;
	buf[3] = Sync_CRC_EOP;


	send(buf, 4);

	if (recv(buf, 1) < 0)
	exit(1);
	if (buf[0] == Resp_STK_NOSYNC) 
	{
		if (tries > 33) 
		{
			stk500::error2(ERRORNOSYNC);
			return -1;
		}
		if (getsync() < 0)
		return -1;
		goto retry;
	}
	else if (buf[0] != Resp_STK_INSYNC) 
	{
		stk500::error1(ERRORPROTOSYNC, buf[0]);
		return -1;
	}

	if (recv(buf, 1) < 0) exit(1);
	if (buf[0] == Resp_STK_OK) 
	{
		return 0;
	}

	stk500::error1(ERRORPROTOSYNC, buf[0]);
	return -1;
}
int stk500::paged_write(AVRMEM * m, int page_size, int n_bytes)
{
	// This code from avrdude has the luxury of living on a PC and copying buffers around.
	// not for us...
	// unsigned char buf[page_size + 16];
	unsigned char cmd_buf[16]; //just the header
	int memtype;
	// unsigned int addr;
	int block_size;
	int tries;
	unsigned int n;
	unsigned int i;
	int flash;

	// Fix page size to 128 because that's what arduino expects
	page_size = 128;
	//EEPROM isn't supported
	memtype = 'F';
	flash = 1;


	/* build command block and send data separeately on arduino*/
	
	i = 0;
	cmd_buf[i++] = Cmnd_STK_PROG_PAGE;
	cmd_buf[i++] = (page_size >> 8) & 0xff;
	cmd_buf[i++] = page_size & 0xff;
	cmd_buf[i++] = memtype;
	send(cmd_buf,4);
	send(&m->buf[0], page_size);
	cmd_buf[0] = Sync_CRC_EOP;
	send( cmd_buf, 1);

	if (recv(cmd_buf, 1) < 0) exit(1); // errr need to fix this...
	if (cmd_buf[0] == Resp_STK_NOSYNC) 
	{
		stk500::error2(ERRORNOSYNC);
		return -3;
	}
	else if (cmd_buf[0] != Resp_STK_INSYNC) 
	{

		stk500::error1(ERRORPROTOSYNC, cmd_buf[0]);
		return -4;
	}
	
	if (recv(cmd_buf, 1) < 0)
	exit(1);
	if (cmd_buf[0] != Resp_STK_OK) 
	{
		stk500::error1(ERRORNOTOK,cmd_buf[0]);

		return -5;
	}
	return n_bytes;
}
#ifdef LOADVERIFY //maybe sometime? note code needs to be re-written won't work as is
int stk500::paged_load(AVRMEM * m, int page_size, int n_bytes)
{
	unsigned char buf[16];
	int memtype;
	unsigned int addr;
	int a_div;
	int tries;
	unsigned int n;
	int block_size;

	memtype = 'F';


	a_div = 1;

	if (n_bytes > m->size) 
	{
		n_bytes = m->size;
		n = m->size;
	}
	else {
		if ((n_bytes % page_size) != 0) 
		{
			n = n_bytes + page_size - (n_bytes % page_size);
		}
		else 
		{
			n = n_bytes;
		}
	}

	for (addr = 0; addr < n; addr += page_size) 
	{
		//    report_progress (addr, n_bytes, NULL);

		if ((addr + page_size > n_bytes)) 
		{
			block_size = n_bytes % page_size;
		}
		else 
		{
			block_size = page_size;
		}
		
		tries = 0;
		retry:
		tries++;
		stk500_loadaddr(addr/a_div);
		buf[0] = Cmnd_STK_READ_PAGE;
		buf[1] = (block_size >> 8) & 0xff;
		buf[2] = block_size & 0xff;
		buf[3] = memtype;
		buf[4] = Sync_CRC_EOP;
		send(buf, 5);

		if (recv(buf, 1) < 0) exit(1);
		if (buf[0] == Resp_STK_NOSYNC) 
		{
			if (tries > 33) 
			{
				stk500::error2(ERRORNOSYNC);
				return -3;
			}
			if (getsync() < 0)
			return -1;
			goto retry;
		}
		else if (buf[0] != Resp_STK_INSYNC) 
		{
			stk500::error1(ERRORPROTOSYNC, buf[0]);
			return -4;
		}

		if (recv(&m->buf[addr], block_size) < 0) exit(1);

		if (recv(buf, 1) < 0) exit(1);

		if (buf[0] != Resp_STK_OK) 
		{
			stk500::error1(ERRORPROTOSYNC, buf[0]);
			return -5;
		}
	}

	return n_bytes;
}
#endif

/*
* issue the 'program enable' command to the AVR device
*/
int stk500::program_enable()
{
	unsigned char buf[16];
	int tries=0;

	retry:
	
	tries++;

	buf[0] = Cmnd_STK_ENTER_PROGMODE;
	buf[1] = Sync_CRC_EOP;

	send( buf, 2);
	if (recv( buf, 1) < 0) exit(1);
	if (buf[0] == Resp_STK_NOSYNC) 
	{
		if (tries > 33) 
		{
			stk500::error2(ERRORNOSYNC);
			return -1;
		}
		if (getsync()< 0)
		return -1;
		goto retry;
	}
	else if (buf[0] != Resp_STK_INSYNC) 
	{
		stk500::error1(ERRORPROTOSYNC,buf[0]);
		return -1;
	}

	if (recv( buf, 1) < 0) exit(1);
	if (buf[0] == Resp_STK_OK) 
	{
		return 0;
	}
	else if (buf[0] == Resp_STK_NODEVICE) 
	{
		stk500::error2(ERRORNODEVICE);
		return -1;
	}

	if(buf[0] == Resp_STK_FAILED)
	{
		stk500::error2(ERRORNOPROGMODE);
		return -1;
	}


	stk500::error1(ERRORUNKNOWNRESP,buf[0]);

	return -1;
}

void stk500::disable()
{
	unsigned char buf[16];
	int tries=0;

	retry:
	
	tries++;

	buf[0] = Cmnd_STK_LEAVE_PROGMODE;
	buf[1] = Sync_CRC_EOP;

	send( buf, 2);
	if (recv( buf, 1) < 0) exit(1);
	if (buf[0] == Resp_STK_NOSYNC) 
	{
		if (tries > 33) {
			stk500::error2(ERRORNOSYNC);
			return;
		}
		if (getsync() < 0)
		return;
		goto retry;
	}
	else if (buf[0] != Resp_STK_INSYNC) 
	{
		stk500::error1(ERRORPROTOSYNC,buf[0]);
		return;
	}

	if (recv( buf, 1) < 0) exit(1);
	if (buf[0] == Resp_STK_OK) 
	{
		return;
	}
	else if (buf[0] == Resp_STK_NODEVICE) 
	{
		stk500::error2(ERRORNODEVICE);
		return;
	}

	stk500::error1(ERRORUNKNOWNRESP,buf[0]);

	return;
}
//original avrdude error messages get copied to ram and overflow, wo use numeric codes.
void stk500::error1(int errno,unsigned char detail)
{
	//DEBUGP("error: ");
	//DEBUGP(errno);
	//DEBUGP(" detail: 0x");
	//DEBUGPLN(detail,HEX);
}


void stk500::error2(int errno)
{
	//DEBUGP("error" );
	//DEBUGPLN(errno);
}
void stk500::dumphex(unsigned char *buf,int len)
{
	//for (int i = 0; i < len; i++)
	//{
		//if (i%16 == 0)
		//DEBUGPLN();
		//DEBUGP(buf[i],HEX);DEBUGP(" ");
	//}
	//DEBUGPLN();
}
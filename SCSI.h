/*
 * SCSI.h
 *
 *  Created on: Jan 29, 2018
 *      Author: mark
 */

#include <stdint.h>
#include "my_debug.h"
#include "MSC.h"

#ifndef SRC_SCSI_H_
#define SRC_SCSI_H_

// SCSI commands ref: https://www.seagate.com/staticfiles/support/disc/manuals/scsi/100293068a.pdf

// opcodes required (by Windows) for USB card readers -> https://blogs.msdn.microsoft.com/usbcoreblog/2011/07/21/usb-mass-storage-and-compliance/
#define SCSI_FORMAT UNIT					0x04
#define SCSI_INQUIRY 						0x12
#define SCSI_MODE_SELECT_6 					0x15
#define SCSI_MODE_SENSE_6 					0x1A
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL	0x1E
#define SCSI_READ_CAPACITY_10 				0x25
#define SCSI_READ_CAPACITY_16 				0x9E // really ?
//#define SCSI_READ_FORMAT_CAPACITY 			0x23 // undocumented
#define SCSI_READ_10 						0x28
#define SCSI_RECEIVE_DIAGNOSTIC_RESULTS		0x1C // when ENCSERV=1 ???
#define SCSI_REPORT LUNS					0xA0
#define SCSI_REQUEST_SENSE 					0x03
#define SCSI_START_STOP 					0x1B
#define SCSI_TEST_UNIT_READY 				0x00
#define SCSI_WRITE_10 						0x2A
//#define SCSI_VERIFY 						0x2F

struct SCSI_CDB_CONTROL { // Generic just get opcode
    uint8_t	LINK:1, obsolete:1, NACA:1, reserv:3, vendorspcfc:2;
};

struct SCSI_CBD_GENERIC { // Generic just get opcode
    uint8_t	opcode;
	uint8_t array[15];
};

struct SCSI_CBD_INQUIRY {
	  uint8_t	opcode; //
	  uint8_t	EVPD:1, CMDDT:1, reserv:6;
	  uint8_t	pgcode;
	  uint16_t	length;
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[10];
};

//#define INQUIRY_RMB_NOTREMOVABLE 0<<7
//#define INQUIRY_RMB_REMOVABLE 1<<7

//PERIPHERAL DEVICE TYPE
#define SCSI_SBC2	0x00 // Direct access block device (e.g., magnetic disk)
#define SCSI_RBC	0x0E // Simplified direct-access device (e.g., magnetic disk) -- Windows does not support it

union SCSI_STANDARD_INQUIRY_DATA{
	struct INQUIRY {
	uint8_t peripheral_device_type:5, peripheral_qualifier;
	uint8_t reserv1:7, RMB:1; // = INQUIRY_RMB_REMOVABLE; // RMB(bit 7), resrved(0-6)
	uint8_t version; //=0;
	//uint8_t ACA_flags; //=0; // Obsolete(7-6), NORMACA(5), HISUP(4), Response Data Format(0-3)
	uint8_t response_data_format:4, HISUP:1, NORMACA:1, obsolete1:2;
	uint8_t additional_length; // N-4
	//uint8_t SCSI_flags; //=0; // SCCS(7) ACCS(6) TPGS(5-4) 3PC(3) resrved(1-2) PROTECT(0)
	uint8_t protect:1, reserv2:2, b3PC:1, TPGS:2, ACC:1, SCCS:1;
	//uint8_t BQue_flags; //=0; // BQUE(7) ENSERV(6) VS(5) MULTIP(4) MCHNGR(3) Obsolete(1-2) ADDR16(0)
	uint8_t adr16:1, obsolete2:2, MCHNGR:1, MULTIP:1, VS:1, ENSERV:1, BQUE:1;
	//uint8_t RelAdr_flags; //=0; // Obsolete(7-6) WUSB16(5) SYNC(4) LINKED(3) Obsolete(2) CMDQUE(1) VS(0)
	uint8_t VS2:1, CMDQUE:1, obsolete:3, linked:1, SYNC:1, WBUS16:1, obsolete4:2;
	uint8_t t10_vendor_id[8]; //={'M','M','7',' ',' ',' ',' ',' '};
	uint8_t product_id[16]; //={'S','V','M','L',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
	uint8_t product_revision_level[4]; //={'0','0','0','1'};
	uint8_t drive_serial_number[8]; //={'0','0','0','1','0','0','0','1'};
	uint8_t vendor_unique[12]; //={'0','0','0','1','0','0','0','1'};
	uint8_t IUS:1, QAS:1, clocking:2, reserv3:4;
	} inquiry;
	uint8_t array[36];
};
extern SCSI_STANDARD_INQUIRY_DATA standardInquiry;

/*
struct SCSI_CBD_READ_FORMAT_CAPACITY {
	  uint8_t	opcode;
	  uint8_t   rest[15];
};
*/

struct SCSI_CBD_READ_CAPACITY_10 {
	  uint8_t	opcode; // 0x25
	  uint8_t	reserv1;
	  uint32_t	LBA;
	  uint8_t	reserv2[2];
	  uint8_t	PMI:1, reserv3:7;
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[7];
};
/*
 * The LOGICAL BLOCK ADDRESS field shall be set to zero if the PMI bit is set to zero. If the PMI bit is set to zero and the
LOGICAL BLOCK ADDRESS field is not set to zero, the device server shall terminate the command with CHECK CONDI-
TION status with the sense key set to ILLEGAL REQUEST and the additional sense code set to INVALID FIELD IN CDB.

A partial medium indicator (PMI) bit set to zero specifies that the device server return information on the last logical
block on the direct-access block device

A PMI bit set to one specifies that the device server return information on the last logical block after that specified in
the LOGICAL BLOCK ADDRESS field before a substantial vendor-specific delay in data transfer may be encountered.
 */

union SCSI_CAPACITY_DATA_10 {
	struct FIELDS {
		  uint32_t	lastLBA;
		  uint32_t	block_sz; // in bytes
	} fields;
	uint8_t array[8];
};

struct SCSI_CBD_READ_CAPACITY_16 {
	  uint8_t	opcode; // 0x9E
	  uint8_t	srvact:5, reserv1:3;
	  uint64_t	lastLBA;
	  uint32_t	block_sz;
	  uint8_t	PMI:1, reserv2:7;
	  SCSI_CDB_CONTROL   control;
};

union SCSI_CAPACITY_DATA_16 {
	struct FIELDS {
		  uint64_t	lastLBA;
		  uint32_t	block_sz; // in bytes
		  uint8_t PROT_EN:1, P_TYPE:3, reserv1:4;
		  uint8_t reserv2[19];
	} fields;
	uint8_t array[32];
};


struct SCSI_CBD_READ_10 {
	  uint8_t	opcode; // 0x28
	  uint8_t	obsolete:1, FUA_NV:1, reserv1:1, FUA:1, DPO:1, RDPROTECT:3;
	  uint32_t	LBA;
	  uint8_t	GROUP_NO:5, reserv2:3;
	  uint16_t	length;
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[6];
};

struct SCSI_CBD_WRITE_10 {
	  uint8_t	opcode; // 0x2A
	  uint8_t	obsolete:1, FUA_NV:1, reserv1:1, FUA:1, DPO:1, RDPROTECT:3;
	  uint32_t	LBA;
	  uint8_t	GROUP_NO:5, reserv2:3;
	  uint16_t	length;
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[6];
};

struct SCSI_CBD_REQUEST_SENSE {
	  uint8_t	opcode; // 0x3
	  uint8_t	desc:1, reserv1:7;
	  uint8_t	reserv2[2];
	  uint8_t	length;
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[10];
};

struct SCSI_CBD_REQUEST_SENSE_DATA {
	  uint8_t	response_code:7, valid:1;
	  uint8_t	obolete;
	  uint8_t	sense_key:4, reserv1:1, ILI:1, EOM:1, filemark:1;
	  uint8_t	information[4];
	  uint8_t	additional_sense_len;
	  uint8_t	command_information[4];
	  uint8_t	additional_sense_code;
	  uint8_t	additional_sense_code_qualifier;
	  uint8_t	field_replaceable_unit_code;
	  uint16_t	sense_key_specific:15, SKSV:1;
	  uint8_t	additional_sense_bytes;
};

struct SCSI_CBD_MODE_SENSE_6 {
	  uint8_t	opcode; // 0x1A
	  uint8_t	reserv1:3, DBD:1, reserv2:4;
	  uint8_t	page_code:6, PC:2; //for Mode Sense. Page Code
	  uint8_t	subpage_code;
	  uint8_t	length;
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[10];
};

struct SCSI_CBD_MODE_SENSE_DATA_6 {
	  uint8_t	mode_data_length;
	  uint8_t	medium_type;
	  uint8_t	dev_specific_param;
	  uint8_t	block_descr_length;
};


struct SCSI_CBD_TEST_UNIT_READY {
	  uint8_t	opcode; // 0x00
	  uint8_t	reserv[4];
	  uint8_t	length; //alloc lenght or "Prevent Allow Flag" one bit 0 for Allow Media Removal opcode
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[10];
};


/*
struct SCSI_CBD_VERIFY {
	  uint8_t	opcode;
	  uint8_t	reserv1[4];
	  uint8_t	length; //alloc lenght or "Prevent Allow Flag" one bit 0 for Allow Media Removal opcode
	  uint8_t   rest[10];
};
*/

struct SCSI_CBD_START_STOP {
	  uint8_t	opcode; // 0x1B
	  uint8_t	immed:1, reserv1:7;
	  uint8_t	reserv2[2];
	  uint8_t	start:1, LOEJ:1, reserv3:2, pwr_condition:4;
	  SCSI_CDB_CONTROL   control;
	  uint8_t   rest[10];
};

struct SCSI_CBD_PREVENT_ALLOW_MEDIUM_REMOVAL {
	  uint8_t	opcode; // 0x1E
	  uint8_t	reserv1:5, LUN:3; // LUN is obsolete
	  uint8_t	reserv2[2];
	  uint8_t	prevent:2, reserv3:6; //
	  uint8_t	link:1, flag:1, reserv4:4, vndr_specif:1, lock_shared:1;
	  uint8_t   rest[10];
};


union SCSI_CBD {
	SCSI_CBD_GENERIC generic;
	SCSI_CBD_INQUIRY inquiry;
	SCSI_CBD_READ_CAPACITY_10 read_capacity10;
	SCSI_CBD_READ_CAPACITY_16 read_capacity16;
	SCSI_CBD_READ_10 read10;
	SCSI_CBD_WRITE_10 write10;
	SCSI_CBD_REQUEST_SENSE request_sense;
	SCSI_CBD_MODE_SENSE_6 mode_sense6;
	SCSI_CBD_MODE_SENSE_DATA_6 sense_data6;
	SCSI_CBD_TEST_UNIT_READY unit_ready;
	SCSI_CBD_START_STOP start_stop;
	SCSI_CBD_PREVENT_ALLOW_MEDIUM_REMOVAL medium_removal;
	uint8_t array[16];
};

#define SCSI_UNSUPPORTED_OPERATION	-1

#define MEDIA_READY					0
#define MEDIA_READ_ERROR 			-2
#define NO_MEDIA					-3
#define MEDIA_BUSY					-4

#endif /* SRC_SCSI_H_ */
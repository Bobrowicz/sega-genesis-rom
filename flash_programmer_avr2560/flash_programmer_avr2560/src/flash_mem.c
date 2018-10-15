/*
 * flash_mem.c
 *
 * Created: 10/11/2018 8:04:22 PM
 *  Author: peter
 */ 

#include <flash_mem.h>

static inline void write_address_to_port(const uint32_t *address);
static inline void write_data_to_port(const uint16_t  *data);
static inline void byte_load_sequence(struct SDP_command *byte_sequence);


/*
 * SST_39SF040 is a 4Mb memory and requires 19 address lines.
 * Address has to be spread out over three physical ports.
 */
static inline void write_address_to_port(const uint32_t *address)
{
	// Set up address on lines A0 - A7
	ADDR_A0_A7_PORT = *address;
	// Set up address on lines A8 - A15
	ADDR_A8_A15_PORT = (*address >> 8);
	// Set up address on lines A16 -A18
	ADDR_A16_A18_PORT |= ((*address >> 16) & ADDR_A16_A18_MASK);
}

/*
 * SST_39SF040 memory is organized as 512k x8.
 * Two chips are connected to common address bus.
 * Each chip receives half of a 16 bit word.
 */
static inline void write_data_to_port(const uint16_t  *data)
{
	// Low byte on D0 - D7
	DATA_D0_D7_PORT = *data;
	// High byte on D8 - D15
	DATA_D8_D15_PORT = (*data >> 8);
}

/*
 * SST_39SF040 uses Software Data Protection to prevent inadvertent writes.
 * Write/Erase operations must be preceded by 3/6 byte load sequence.
 */
static inline void byte_load_sequence(struct SDP_command *byte_sequence)
{
	// Struct member used to indicate length of an address/data arrays
	for (uint8_t i = 0; i < byte_sequence->byte_count; i++)
	{
		// Load address and data into port registers
		write_address_to_port(byte_sequence->address + i);
		// Sending same data to both memory chips
		DATA_D0_D7_PORT = *(byte_sequence->data + i);
		DATA_D8_D15_PORT = *(byte_sequence->data + i);
		_NOP();	// for sync
		
		// Address is latched into memory on falling edge of WE
		FLASH_CTRL_PORT &= ~(1 << WE_PIN);
		_NOP();	// Min pulse width low: 40ns
		
		// Data is latched into memory on rising edge of WE
		FLASH_CTRL_PORT |= (1 << WE_PIN);
		_NOP();	// Min pulse width high: 30ns
	}
}

uint8_t flash_check_id_SST_39SF040(void)
{
	// Byte load sequence for device id
	// 1st byte cycle: Address: 0x5555, Data: 0xAA
	// 2nd byte cycle: Address: 0x2AAA, Data: 0x55
	// 3rd byte cycle: Address: 0x5555, Data: 0x90
	const uint32_t address_sequence[] = {0x5555, 0x2AAA, 0x5555};
	const uint8_t data_sequence[] = {0xAA, 0x55, 0x90};
		
	struct SDP_command id_entry;
	id_entry.address = address_sequence;
	id_entry.data = data_sequence;
	id_entry.byte_count = sizeof(data_sequence) / sizeof(data_sequence[0]);
	
	// Switch data ports to output mode so we can issue id code to memory
	DATA_D0_D7_DDR = 0xFF;
	DATA_D8_D15_DDR = 0xFF;
	
	// Sequence should start with CE low and OE high
	FLASH_CTRL_PORT &= ~(1 << CE_PIN);
	FLASH_CTRL_PORT |= (1 << OE_PIN);
	
	byte_load_sequence(&id_entry);
	
	// Switch data ports to input to read product id
	DATA_D0_D7_DDR = 0x00;
	DATA_D8_D15_DDR = 0x00;
	
	// Pull CE high to to finish sequence and delay 150ns
	FLASH_CTRL_PORT |= (1 << CE_PIN);
	_NOP();
	_NOP();
	_NOP();
	
	// Pull CE and OE low to re-enable chip and read product id
	FLASH_CTRL_PORT &= ~(1 << CE_PIN);
	FLASH_CTRL_PORT &= ~(1 << OE_PIN);
	
	// Manufacturer id is at address 0x0000;
	uint32_t dev_id_address = 0x00;
	write_address_to_port(&dev_id_address);
	_NOP();
	// Read manufacturer id
	uint8_t id_1 = DATA_D0_D7_PIN;
	id_1 = id_1 & DATA_D8_D15_PIN;
	
	// Device id is at address 0x0001;
	dev_id_address++;
	write_address_to_port(&dev_id_address);
	_NOP();
	// Read device id
	uint8_t id_2 = DATA_D0_D7_PIN;
	id_2 = id_2 & DATA_D8_D15_PIN;
	
	// Completed memory access, pull CE high
	FLASH_CTRL_PORT |= (1 << CE_PIN);
	
	// Return True if correct memory detected and False otherwise
	uint8_t return_status = 0;
	
	if ((id_1 == SST_ID) && (id_2 == SST_39SF040))
	{
		return_status = TRUE;
	} 
	else 
	{
		return_status = FALSE;
	}
	
	// Issue exit code and return memory to read operating mode
	flash_exit_id_SST_39SF040();
	
	return return_status;
}

void flash_exit_id_SST_39SF040(void)
{
	// Byte sequence for id exit and reset
	// 1st byte cycle: Address: 0x5555, Data: 0xAA
	// 2nd byte cycle: Address: 0x2AAA, Data: 0x55
	// 3rd byte cycle: Address: 0x5555, Data: 0xF0
	uint32_t address_sequence[] = {0x5555, 0x2AAA, 0x5555};
	uint8_t data_sequence[] = {0xAA, 0x55, 0xF0};
		
	struct SDP_command id_exit;
	id_exit.address = address_sequence;
	id_exit.data = data_sequence;
	id_exit.byte_count = sizeof(data_sequence) / sizeof(data_sequence[0]);
	
	// Switch data ports to output mode so we can issue id code to memory
	DATA_D0_D7_DDR = 0xFF;
	DATA_D8_D15_DDR = 0xFF;
	
	// Sequence should start with CE low and OE high
	FLASH_CTRL_PORT &= ~(1 << CE_PIN);
	FLASH_CTRL_PORT |= (1 << OE_PIN);
	
	byte_load_sequence(&id_exit);
	
	// Pulse CE high for 150ns to finish sequence
	FLASH_CTRL_PORT |= (1 << WE_PIN);
	_NOP();
	_NOP();
	_NOP();
	FLASH_CTRL_PORT &= ~(1 << CE_PIN);
}

void flash_program_one_word(uint32_t *address, uint16_t *word)
{
	// Byte program sequence
	// 1st byte cycle: Address: 0x5555, Data: 0xAA
	// 2nd byte cycle: Address: 0x2AAA, Data: 0x55
	// 3rd byte cycle: Address: 0x5555, Data: 0xA0
	const uint32_t address_sequence[] = {0x5555, 0x2AAA, 0x5555};
	const uint8_t data_sequence[] = {0xAA, 0x55, 0xA0};
		
	struct SDP_command prog_byte;
	prog_byte.address = address_sequence;
	prog_byte.data = data_sequence;
	prog_byte.byte_count = sizeof(data_sequence) / sizeof(data_sequence[0]);
	
	// Set data ports to output in case they aren't
	DATA_D0_D7_DDR = 0xFF;
	DATA_D8_D15_DDR = 0xFF;
	
	// Set CE low in case it isn't
	FLASH_CTRL_PORT &= ~(1 << CE_PIN);
		
	// Set OE high to disable output
	FLASH_CTRL_PORT |= (1 << OE_PIN);
	
	byte_load_sequence(&prog_byte);
	
	// Load byte address and data.
	write_address_to_port(address);
	write_data_to_port(word);
	_NOP(); // Address setup time
	
	// Pull WE low to latch the address
	FLASH_CTRL_PORT &= ~(1 << WE_PIN);
	_NOP(); // WE pulse width low
	
	// Pull WE high to latch data
	FLASH_CTRL_PORT |= (1 << WE_PIN);
	_NOP(); // WE pulse width high
}

void flash_read_one_word(uint32_t *address, uint16_t *word)
{
	// Set data ports to input in case they aren't
	DATA_D0_D7_DDR = 0x00;
	DATA_D8_D15_DDR = 0x00;
	
	// WE should be high for read operation
	FLASH_CTRL_PORT |= (1 << WE_PIN);
	
	// Put address on the bus
	write_address_to_port(address);
	
	// Pull CE low to enable chip in case it isn't
	FLASH_CTRL_PORT &= ~(1 << CE_PIN);
	
	// Pull OE low to activate output
	FLASH_CTRL_PORT &= ~(1 << OE_PIN);
	
	// Wait for address access time min 70ns
	_NOP(); // one NOP should be 62.5ns at 16MHz
	//_NOP();
	
	// Read high byte from data lines D15 - D8
	*word = (DATA_D8_D15_PIN << 8);
	// Read low byte from data lines D7 - D0
	*word |= DATA_D0_D7_PIN;
	
	// Pull OE high to disable output
	FLASH_CTRL_PORT |= (1 << OE_PIN);
}

void flash_erase_entire_chip(void)
{
	// Byte program sequence
	// 1st byte cycle: Address: 0x5555, Data: 0xAA
	// 2nd byte cycle: Address: 0x2AAA, Data: 0x55
	// 3rd byte cycle: Address: 0x5555, Data: 0x80
	// 4th byte cycle: Address: 0x5555, Data: 0xAA
	// 5th byte cycle: Address: 0x2AAA, Data: 0x55
	// 6th byte cycle: Address: 0x5555, Data: 0x10
	const uint32_t address_sequence[] = {0x5555, 0x2AAA, 0x5555, 0x5555, 0x2AAA, 0x5555};
	const uint8_t data_sequence[] = {0xAA, 0x55, 0x80, 0xAA, 0x55, 0x10};
		
	struct SDP_command erase_chip;
	erase_chip.address = address_sequence;
	erase_chip.data = data_sequence;
	erase_chip.byte_count = sizeof(data_sequence) / sizeof(data_sequence[0]);
	
	// Set data ports to output in case they aren't
	DATA_D0_D7_DDR = 0xFF;
	DATA_D8_D15_DDR = 0xFF;
	
	// Set CE low in case it isn't
	FLASH_CTRL_PORT &= ~(1 << CE_PIN);
	
	// Set OE high to disable output
	FLASH_CTRL_PORT |= (1 << OE_PIN);
	
	byte_load_sequence(&erase_chip);
	
	// Chip-Erase time
	_delay_ms(100);
}
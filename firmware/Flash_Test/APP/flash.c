#include "flash.h"\n

/* Private Variable */
static uint32_t rebootCounter = 0;
static uint32_t rebootCounterFlag = 0;
uint32_t EraseCounter = 0x0, Address = 0x0;
uint16_t Data = 0xAAAA;
uint32_t WRPR_Value = 0xFFFFFFFF, ProtectedPages = 0x0;
uint32_t NbrOfPage;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;
volatile TestStatus MemoryEraseStatus = PASSED;
u32 buf[64];

void Flash_Init(void)
{
    FLASH_Unlock();
    // Attempt to read rebootCounterFlag from flash
    rebootCounterFlag = *((uint32_t *)REBOOT_COUNTER_ADDRESS_FLAG);
    // printf("Reboot Counter Flag: %d\r\n", rebootCounterFlag);

    // Check if the value is not valid (initialized)
    if (rebootCounterFlag != 1)
    {
        // Initialize the value
        // printf("Reboot Counter Flag is not valid\r\n");
        FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS);
        rebootCounter = 0;
        rebootCounterFlag = 1;
        FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS, rebootCounter);
        FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS_FLAG, rebootCounterFlag);
    }
    else if (rebootCounterFlag)
    {
        rebootCounter = *((uint32_t *)REBOOT_COUNTER_ADDRESS);
    }

    rebootCounter++;
    // printf("Reboot Counter: %d\r\n", rebootCounter);
    FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS, rebootCounter);

    // Erase the page
    // FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS_FLAG);

    FLASH_Lock();
}

void Flash_Erase(void)
{
    FLASH_Unlock();
    // FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS);
    FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS_FLAG);
    FLASH_Lock();
}

uint32_t Flash_Get_Reboot_Counter()
{
    return rebootCounter;
}

/*********************************************************************
 * @fn      Flash_Test
 *
 * @brief   Flash Program Test.
 *
 * @return  none
 */
void Flash_Test(void)
{
    printf("FLASH Test\n");

    FLASH_Unlock();

    NbrOfPage = (PAGE_WRITE_END_ADDR - PAGE_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP |FLASH_FLAG_WRPRTERR);

    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(PAGE_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));  //Erase 4KB

      if(FLASHStatus != FLASH_COMPLETE)
      {
        printf("FLASH Erase Fail\r\n");
        return;
      }
      printf("FLASH Erase Suc\r\n");
    }

    Address = PAGE_WRITE_START_ADDR;
    printf("Programing...\r\n");
    while((Address < PAGE_WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
    {
      FLASHStatus = FLASH_ProgramHalfWord(Address, Data);
      Address = Address + 2;
    }

    Address = PAGE_WRITE_START_ADDR;

    printf("Program Cheking...\r\n");
    while((Address < PAGE_WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
    {
      if((*(__IO uint16_t*) Address) != Data)
      {
        MemoryProgramStatus = FAILED;
      }
      Address += 2;
    }

    if(MemoryProgramStatus == FAILED)
    {
       printf("Memory Program FAIL!\r\n");
    }
    else
    {
       printf("Memory Program PASS!\r\n");
    }


     FLASH_Lock();

}

/*********************************************************************
 * @fn      Flash_Test_Fast
 *
 * @brief   Flash Fast Program Test.
 *
 * @return  none
 */
void Flash_Test_Fast(void)
{
	u16 i,j,flag;

    for(i=0; i<64; i++){
        buf[i] = i;
    }

    printf("FLASH Fast Mode Test\n");

	FLASH_Unlock_Fast();

	FLASH_EraseBlock_32K_Fast(FAST_FLASH_PROGRAM_START_ADDR);

	printf("Program 32KByte start\n");
	for(i=0; i<128; i++){
	    FLASH_ProgramPage_Fast(FAST_FLASH_PROGRAM_START_ADDR + 256*i, buf);
	}

	for(i=0; i<128; i++){
	    for(j=0; j<64; j++){
	        if(*(u32*)(FAST_FLASH_PROGRAM_START_ADDR+256*i+4*j) != j){
	            flag = 0;
	            break;
	        }
	        else{
	            flag = 1;
	        }
	    }

	}

	if(flag){
	    printf("Program 32KByte suc\n");
	}
	else printf("Program fail\n");

	printf("Erase 256Byte...\n");
	FLASH_ErasePage_Fast(FAST_FLASH_PROGRAM_START_ADDR);

	printf("Read 4KByte...\n");
	for(i=0;i<1024; i++){
      printf("%08x ",*(u32*)(FAST_FLASH_PROGRAM_START_ADDR+4*i));

	}printf("\n");

    printf("Erase 4KByte...\n");
    FLASH_ErasePage(FAST_FLASH_PROGRAM_START_ADDR);

    printf("Read 8KByte...\n");
    for(i=0;i<2048; i++){
      printf("%08x ",*(u32*)(FAST_FLASH_PROGRAM_START_ADDR+4*i));

    }printf("\n");

    printf("Erase 32KByte...\n");
    FLASH_EraseBlock_32K_Fast(FAST_FLASH_PROGRAM_START_ADDR);

    printf("Read 32KByte...\n");
    for(i=0;i<(1024*9); i++){
      printf("%08x ",*(u32*)(FAST_FLASH_PROGRAM_START_ADDR+4*i));

    }printf("\n");


    FLASH_Lock_Fast();
    FLASH_Lock();
}

/*******************************************************************************
 *  @file TestToken.c
 *
 *  @brief c file to run functional test calling SPI API functions
 *
 *  @author KSolomon
 *  @date Sep 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/


/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include "stm32f2xx_ll_spi.h"

// Module Includes
#include "Menu.h"

// Utility Includes
#include "TypeDefs.h"

// Driver Includes
#include "Debug.h"
#include "Timer.h"
#include "Token.h"
#include "TokenEeprom.h"
#include "TokenFlash.h"
#include "Config.h"
#include "Test.h"
#include "Timer.h"
#include "TestToken.h"

/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/
#define TEST_TOKEN_RW_SIZE    256

#define TOK_E_WRITE             ((WriteAndVerifyHook) TokenEeprom_Write)
#define TOK_E_ERASE             ((EraseHook) TokenEeprom_Erase)
#define TOK_E_READ              ((WriteAndVerifyHook) TokenEeprom_Read)

#define TOK_F_WRITE             ((WriteAndVerifyHook) TokenFlash_Write)
#define TOK_F_ERASE             ((EraseHook) TokenFlash_Erase)
#define TOK_F_READ              ((WriteAndVerifyHook) TokenFlash_Read)

#define TEST_TOKEN_START_ADDR   0



static TOKEN_t m_tokenType = TOKEN_NONE;

#define TEST_TOKEN_PRINT(tokenType, expectedType, errCount, name) \
do { \
    if(tokenType == expectedType)\
    { \
        TEST_PRINT_RESULT(errCount, name);\
    }\
    else\
    {\
        TEST_PRINT_RESULT_UNKNOWN(name);\
    }\
} while(0)\


/*******************************************************************************
 * Private Function Prototypes

 ******************************************************************************/

// Allow Caller to envoke Flash API Calls outside of a test
static int testToken_moduleMenu(void);

// Verify token initialization
static int testToken_InitTest(void);

// Verify token is connected and is of valid type
static int testToken_GetDeviceTypeTest(void);

// Verify token can read
static int testToken_eeprom_readTest(void);

// Verify token can write
static int testToken_eeprom_writeTest(void);

// Verify token can enable writing
static int testToken_eeprom_writeAll(void);

// Verify token can erase
static int testToken_eeprom_eraseTest(void);

// Verify token can erase all memory through erase cmd
static int testToken_eeprom_eraseAllTest(void);

// Verify token can erase chip
static int testToken_eeprom_eraseChipTest(void);

// Verify token can protect all region combinations
static int testToken_eeprom_protectTest(void);

// Verify token can read
static int testToken_flash_readTest(void);

// Verify token can write
static int testToken_flash_writeTest(void);

// Verify token can enable writing
static int testToken_flash_writeAll(void);

// Verify token can erase
static int testToken_flash_eraseTest(void);

// Verify token can erase all memory through erase cmd
static int testToken_flash_eraseAllTest(void);

// Verify token can erase chip
static int testToken_flash_eraseChipTest(void);

// Verify token can protect all region combinations
static int testToken_flash_protectTest(void);



/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

// Menu header/module strings.
static const char menu_headerStr[] = "TEST MENU: TOKEN";
static const char menu_moduleStr[] = "TOKEN";

// Menu option strings.
static const char menu_moduleCalls[] = "Module Menu";
static const char menu_runAll[] = "Run all tests";

static const char menu_initStr[] = "Token_Init()";
static const char menu_deviceTypeStr[] = "Token_GetDeviceType()";

static const char menu_e_readStr[] = "TokenEeprom_Read()";
static const char menu_e_write1Str[] = "TokenEeprom_Write() 1";
static const char menu_e_write2Str[] = "TokenEeprom_Write() 2";
static const char menu_e_erase1Str[] = "TokenEeprom_Erase() 1";
static const char menu_e_erase2Str[] = "TokenEeprom_Erase() 2";
static const char menu_e_eraseAllStr[] = "TokenEeprom_EraseAll()";
static const char menu_e_protectStr[] = "TokenEeprom_ProtectRegion()";

static const char menu_f_readStr[] = "TokenFlash_Read()";
static const char menu_f_write1Str[] = "TokenFlash_Write() 1";
static const char menu_f_write2Str[] = "TokenFlash_Write() 2";
static const char menu_f_erase1Str[] = "TokenFlash_Erase() 1";
static const char menu_f_erase2Str[] = "TokenFlash_Erase() 2";
static const char menu_f_eraseAllStr[] = "TokenFlash_EraseAll()";
static const char menu_f_protectStr[] = "TokenFlash_ProtectRegion()";

// Menu items.
static const MenuItem_t m_menuItemsFLASH[] =
{
    { testToken_moduleMenu,             menu_moduleCalls },
    { TestToken_RunAll,                 menu_runAll },
    { testToken_InitTest,               menu_initStr },
    { testToken_GetDeviceTypeTest,      menu_deviceTypeStr },

    { testToken_flash_readTest,         menu_f_readStr},
    { testToken_flash_writeTest,        menu_f_write1Str},
    { testToken_flash_writeAll,         menu_f_write2Str},
    { testToken_flash_eraseTest,        menu_f_erase1Str},
    { testToken_flash_eraseAllTest,     menu_f_erase2Str },
    { testToken_flash_eraseChipTest,    menu_f_eraseAllStr },
    { testToken_flash_protectTest,      menu_f_protectStr },

    { NULL, NULL }
};

static const MenuItem_t m_menuItemsEEPROM[] =
{
    { testToken_moduleMenu,             menu_moduleCalls },
    { TestToken_RunAll,                 menu_runAll },
    { testToken_InitTest,               menu_initStr },
    { testToken_GetDeviceTypeTest,      menu_deviceTypeStr },

    { testToken_eeprom_readTest,       menu_e_readStr},
    { testToken_eeprom_writeTest,      menu_e_write1Str},
    { testToken_eeprom_writeAll,       menu_e_write2Str},
    { testToken_eeprom_eraseTest,      menu_e_erase1Str},
    { testToken_eeprom_eraseAllTest,   menu_e_erase2Str },
    { testToken_eeprom_eraseChipTest,  menu_e_eraseAllStr },
    { testToken_eeprom_protectTest,    menu_e_protectStr },

    { NULL, NULL }
};


// Menu.
static const Menu_t m_menuEEPROM =
{
    .menuStr   = menu_headerStr,
    .moduleStr = menu_moduleStr,
    .mItems    = m_menuItemsEEPROM,
    .hook      = NULL
};

static const Menu_t m_menuFLASH =
{
    .menuStr   = menu_headerStr,
    .moduleStr = menu_moduleStr,
    .mItems    = m_menuItemsFLASH,
    .hook      = NULL
};


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief TestTOKEN_Menu
 *
 * Run Functional Test & Debugging for TOKEN. Setup such that tests run for
 * either EEPROM/Flash token pending on which is inserted.
 *
 * @param  None
 *
 * @return None
 *
 ******************************************************************************/
int TestToken_Menu(void)
{
    int errCount = 0;
    m_tokenType = Token_GetDeviceType();
    if(m_tokenType == TOKEN_EEPROM)
    {
        Menu_MenuInit(&m_menuEEPROM);
    }
    else if(m_tokenType == TOKEN_FLASH)
    {
        Menu_MenuInit(&m_menuFLASH);
    }
    else
    {
        TEST_PRINT_ERROR_NO_CODE("No Token Inserted, cannot run tests");
        errCount++;
    }
    return errCount;
}

/*******************************************************************************
 * @brief TestTOKEN_RunAll
 *
 * Runs all TOKEN Tests.
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
int TestToken_RunAll(void)
{
    m_tokenType = Token_GetDeviceType();

    int errCount = 0;
    if(m_tokenType == TOKEN_EEPROM)
    {
        MENU_RUN_ALL_TESTS(&errCount, m_menuItemsEEPROM);
    }
    else if(m_tokenType == TOKEN_FLASH)
    {
        MENU_RUN_ALL_TESTS(&errCount, m_menuItemsFLASH);
    }
    else
    {
        TEST_PRINT_ERROR_NO_CODE("Tried to run token test, but no token is inserted");
    }
    return errCount;
}

/*******************************************************************************
 * @brief TestToken_FunctionalTest
 *
 * Runs functional test
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
int TestToken_FunctionalTest(void)
{
    return testToken_GetDeviceTypeTest();
}


/*******************************************************************************
 * Private Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief testToken_moduleMenu
 *
 * Allow Caller to envoke Flash API Calls outside of a test
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_moduleMenu(void)
{
    Menu_DUMMY_FUNCTION();
    // TODO: add module menu & calls when able -- Menu_MenuInit(&m_menuMod);
    return 0;
}

/*******************************************************************************
 * @brief testToken_Init
 *
 * Verify token initialization
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_InitTest(void)
{
    int errCount = 0;
    Token_Init();
    TEST_PRINT_RESULT(errCount, menu_initStr);
    return 0;
}

/*******************************************************************************
 * @brief testToken_GetDeviceTypeTest
 *
 * Verify token is connected and is of valid type
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_GetDeviceTypeTest(void)
{
    int errCount = 0;
    TOKEN_t type = Token_GetDeviceType();
    if(type == TOKEN_NONE)
    {
        errCount++;
        TEST_PRINT_ERROR_NO_CODE("No Token Connected");
    }
        TEST_PRINT_RESULT(errCount, menu_deviceTypeStr);
    return errCount;
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can read
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_flash_readTest(void)
{
    int errCount = 0;
    uint8_t tmpReadBuffer[TOKEN_FLASH_PAGE_LEN] = {0};
    TOKEN_ErrCode_t err = (TOKEN_ErrCode_t) TokenFlash_Read(TEST_TOKEN_START_ADDR, tmpReadBuffer, TOKEN_FLASH_PAGE_LEN);
    if(err != TOKEN_ERR_OK)
    {
        errCount++;
        TEST_PRINT_ERROR(err, "");
    }
    TEST_TOKEN_PRINT(m_tokenType, TOKEN_FLASH, errCount, menu_f_readStr);
    return errCount;
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can write
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_flash_writeTest(void)
{
    int errCount = 0;
    TOKEN_ErrCode_t err = TokenFlash_Erase(TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN);
    if(!Test_WriteAndVerify(TOK_F_WRITE, TOK_F_READ, TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN))
    {
        errCount++;
        TEST_PRINT_ERROR(err, "Verify failed");
    }
    TOK_F_ERASE(TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN);
    TEST_TOKEN_PRINT(m_tokenType, TOKEN_FLASH, errCount, menu_f_write1Str);
    return errCount;
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can enable writing
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_flash_writeAll(void)
{
    int errCount = 0;
    TOKEN_ErrCode_t err = TokenFlash_Erase(0, TOKEN_FLASH_MEM_SIZE);
    if(!Test_WriteAndVerify(TOK_F_WRITE, TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        errCount++;
        TEST_PRINT_ERROR(err, "Verify failed");
    }
    TOK_F_ERASE(0, TOKEN_FLASH_MEM_SIZE);
    TEST_TOKEN_PRINT(m_tokenType, TOKEN_FLASH, errCount, menu_f_write2Str);
    return errCount;
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can erase
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_flash_eraseTest(void)
{
    int errCount = 0;
    TOKEN_ErrCode_t err = TokenFlash_Erase(TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN);
    if(err != TOKEN_ERR_OK)
    {
        errCount++;
        TEST_PRINT_ERROR(err, "Erase failed");
    }
    if(!Test_VerifyErased(TOK_F_READ, TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN))
    {
        errCount++;
        TEST_PRINT_ERROR(err, "Verify failed");
    }
    TEST_TOKEN_PRINT(m_tokenType, TOKEN_FLASH, errCount, menu_f_erase1Str);
    return errCount;
}

/*******************************************************************************
 * @brief testToken_flash_eraseAllTest
 *
 * Verify token can erase all memory through erase cmd
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_flash_eraseAllTest(void)
{
    int errCount = 0;
    TOKEN_ErrCode_t err = TokenFlash_Erase(TEST_TOKEN_START_ADDR, TOKEN_FLASH_MEM_SIZE);
    if(err != TOKEN_ERR_OK)
    {
        errCount++;
        TEST_PRINT_ERROR(err, "");
    }
    if(!Test_VerifyErased(TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        errCount++;
        TEST_PRINT_ERROR(err, "Verify failed");
    }
    TEST_TOKEN_PRINT(m_tokenType, TOKEN_FLASH, errCount, menu_f_erase2Str);
    return errCount;
}

/*******************************************************************************
 * @brief testToken_flash_eraseChipTest
 *
 * Verify token can erase chip
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_flash_eraseChipTest(void)
{
    int errCount = 0;
    TOKEN_ErrCode_t err = TokenFlash_EraseAll();
    Timer0_Sleep(TOKEN_FLASH_ERASE_ALL_TIME);
    if(err != TOKEN_ERR_OK)
    {
        errCount++;
        TEST_PRINT_ERROR(err, "");
    }
    if(!Test_VerifyErased(TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        errCount++;
        TEST_PRINT_ERROR(err, "Verify failed");
    }
    TEST_TOKEN_PRINT(m_tokenType, TOKEN_FLASH, errCount, menu_f_eraseAllStr);
    return errCount;
}

/*******************************************************************************
 * @brief testToken_flash_protectTest
 *
 * Verify token can protect all region combinations
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static int testToken_flash_protectTest(void)
{
    int errCount = 0;
    uint8_t protectedRegion = 0;
    TOKEN_ErrCode_t err = TOKEN_ERR_OK;
    for(uint8_t i = 0; i < (uint8_t) TOKEN_FLASH_PROTECT_COUNT; i++)
    {
        err = TokenFlash_ProtectRegion((TOKEN_FlashProtect_t) i);
        protectedRegion = (uint8_t) TokenFlash_GetProtectedRegion();
        if(i != protectedRegion)
        {
            errCount++;
            TEST_PRINT_ERROR(err, "Protection failed. Expected = %d, actual = %d", i, protectedRegion);
            break;
        }
    }
    TokenFlash_ProtectRegion(TOKEN_FLASH_PROTECT_NONE);

    TEST_TOKEN_PRINT(m_tokenType, TOKEN_FLASH, errCount, menu_f_protectStr);
    return errCount;
}

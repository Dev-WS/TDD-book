//- ------------------------------------------------------------------
//-    Copyright (c) James W. Grenning -- All Rights Reserved         
//-    For use by owners of Test-Driven Development for Embedded C,   
//-    and attendees of Renaissance Software Consulting, Co. training 
//-    classes.                                                       
//-                                                                   
//-    Available at http://pragprog.com/titles/jgade/                 
//-        ISBN 1-934356-62-X, ISBN13 978-1-934356-62-3               
//-                                                                   
//-    Authorized users may use this source code in your own          
//-    projects, however the source code may not be used to           
//-    create training material, courses, books, articles, and        
//-    the like. We make no guarantees that this source code is       
//-    fit for any purpose.                                           
//-                                                                   
//-    www.renaissancesoftware.net james@renaissancesoftware.net      
//- ------------------------------------------------------------------


#include "CppUTest/TestHarness.h"

extern "C"
{
#include "LedDriver.h"
#include "RuntimeErrorStub.h"
}

#if 0 //START: StartingPoint
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "LedDriver.h"
}

TEST_GROUP(LedDriver)
{
    void setup()
    {
      LedDriver_Create();
    }

    void teardown()
    {
       LedDriver_Destroy();
    }
};

TEST(LedDriver, Create)
{
  FAIL("Start here");
}
//END: StartingPoint

//START: NoFailStartingPoint
TEST_GROUP(LedDriver)
{
    void setup()
    {
       LedDriver_Create();
    }

    void teardown()
    {
       LedDriver_Destroy();
    }
};

TEST(LedDriver, Create)
{
}
//END: NoFailStartingPoint

#endif

#if 0 //START: FirstThreeTests
TEST_GROUP(LedDriver)
{
    uint16_t virtualLeds;
    void setup()
    {
        LedDriver_Create(&virtualLeds);
    }
    void teardown()
    {
        LedDriver_Destroy();
    }
};

//END: FirstThreeTests
#endif

//START:final
//START: RevisedTestGroupPlusCreate
TEST_GROUP(LedDriver)
{
    uint16_t virtualLeds; // @todo make static?
    BOOL ledsActiveHigh;

    static uint16_t reverse16(uint16_t in)
    {
    	uint16_t out = 0;
    	int i = 16;
    	if (in)
    		while (i--)
    		{
    			out <<= 1;
    			out |= in % 2;
    			in  >>= 1;
    		}
    	return out;
    }

    void virtualLeds_Set(uint16_t value)
    {
    	virtualLeds = reverse16(TRUE == ledsActiveHigh ? value : ~value);
    }
    uint16_t virtualLeds_Get()
    {
    	return reverse16(TRUE == ledsActiveHigh ? virtualLeds : ~virtualLeds);
    }

    void setup()
    {
    	ledsActiveHigh = TRUE;
        LedDriver_Create(&virtualLeds, ledsActiveHigh);
    }
    void teardown()
    {
        LedDriver_Destroy();
    }
};
//END: RevisedTestGroupPlusCreate

//START: FirstThreeTests
//START: LedsAreOffAfterInitialization
TEST(LedDriver, LedsAreOffAfterCreate)
{
	virtualLeds_Set( 0xffff);
    LedDriver_Create(&virtualLeds, ledsActiveHigh);
    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: LedsAreOffAfterInitialization

//START: TurnOnLedZero
TEST(LedDriver, TurnOnLedOne)
{
    LedDriver_TurnOn(1);
    LONGS_EQUAL(1, virtualLeds_Get());
}
//END: TurnOnLedZero

//START: TurnOffLedOne
TEST(LedDriver, TurnOffLedOne)
{
    LedDriver_TurnOn(1);
    LedDriver_TurnOff(1);
    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: TurnOffLedOne
//END: FirstThreeTests

//START: TurnOnMultipleLeds
TEST(LedDriver, TurnOnMultipleLeds)
{
    LedDriver_TurnOn(9);
    LedDriver_TurnOn(8);
    LONGS_EQUAL(0x180, virtualLeds_Get());
}
//END: TurnOnMultipleLeds

//START: TurnOffAnyLedSecondTry
TEST(LedDriver, TurnOffAnyLed)
{
    LedDriver_TurnAllOn();
    LedDriver_TurnOff(8);
    LONGS_EQUAL(0xff7f, virtualLeds_Get());
}
//END: TurnOffAnyLedSecondTry

//START: LedMemoryIsNotReadable
TEST(LedDriver, LedMemoryIsNotReadable)
{
	virtualLeds_Set( 0xffff);
    LedDriver_TurnOn(8);
    LONGS_EQUAL(0x80, virtualLeds_Get());
}
//END: LedMemoryIsNotReadable

//START: UpperAndLowerBounds
TEST(LedDriver, UpperAndLowerBounds)
{
    LedDriver_TurnOn(1);
    LedDriver_TurnOn(16);
    LONGS_EQUAL(0x8001, virtualLeds_Get());
}
//END: UpperAndLowerBounds

//START: OutOfBoundsTurnOnDoesNoHarm
TEST(LedDriver, OutOfBoundsTurnOnDoesNoHarm)
{
    LedDriver_TurnOn(-1);
    LedDriver_TurnOn(0);
    LedDriver_TurnOn(17);
    LedDriver_TurnOn(3141);

    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: OutOfBoundsTurnOnDoesNoHarm

//START: OutOfBoundsTurnOffDoesNoHarm
TEST(LedDriver, OutOfBoundsTurnOffDoesNoHarm)
{
    LedDriver_TurnAllOn();

    LedDriver_TurnOff(-1);
    LedDriver_TurnOff(0);
    LedDriver_TurnOff(17);
    LedDriver_TurnOff(3141);

    LONGS_EQUAL(0xffff, virtualLeds_Get());
}
//END: OutOfBoundsTurnOffDoesNoHarm

//START: OutOfBoundsToDo
IGNORE_TEST(LedDriver, OutOfBoundsToDo)
{
    //TODO: what should we do during runtime?
}
//END: OutOfBoundsToDo

//START: OutOfBoundsProducesRuntimeError
TEST(LedDriver, OutOfBoundsProducesRuntimeError)
{
    LedDriver_TurnOn(-1);
    STRCMP_EQUAL("LED Driver: out-of-bounds LED", RuntimeErrorStub_GetLastError());
}
//END: OutOfBoundsProducesRuntimeError

//START: IsOn
TEST(LedDriver, IsOn)
{
    CHECK_EQUAL(FALSE, LedDriver_IsOn(1));
    LedDriver_TurnOn(1);
    CHECK_EQUAL(TRUE, LedDriver_IsOn(1));
}
//END: IsOn

//START: IsOff
TEST(LedDriver, IsOff)
{
    CHECK_EQUAL(TRUE, LedDriver_IsOff(12));
    LedDriver_TurnOn(12);
    CHECK_EQUAL(FALSE, LedDriver_IsOff(12));
}
//END: IsOff

//START: OutOfBoundsLedsAreAlwaysOff2
TEST(LedDriver, OutOfBoundsLedsAreAlwaysOff)
{
    CHECK_EQUAL(TRUE, LedDriver_IsOff(0));
    CHECK_EQUAL(TRUE, LedDriver_IsOff(17));
    CHECK_EQUAL(FALSE, LedDriver_IsOn(0));
    CHECK_EQUAL(FALSE, LedDriver_IsOn(17));
}
//END: OutOfBoundsLedsAreAlwaysOff2

//START: TurnAllOn
TEST(LedDriver, AllOn)
{
    LedDriver_TurnAllOn();
    LONGS_EQUAL(0xffff, virtualLeds_Get());
}
//END: TurnAllOn

#if 0 //START: TurnAllOnRevised
TEST(LedDriver, AllOn)
{
    LedDriver_TurnAllOn();
    CHECK_EQUAL(true, LedDriver_IsOn(1));
    CHECK_EQUAL(true, LedDriver_IsOn(8));
    CHECK_EQUAL(true, LedDriver_IsOn(16));
    LONGS_EQUAL(0xffff, virtualLeds_Get());
}
#endif //END: TurnAllOnRevised


//START: TurnAllOff
TEST(LedDriver, AllOff)
{
    LedDriver_TurnAllOn();
    LedDriver_TurnAllOff();
    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: TurnAllOff
//END: final



#if 0
//START: TurnOffAnyLedFirstTry
TEST(LedDriver, TurnOffAnyLed)
{
    LedDriver_TurnOn(9);
    LedDriver_TurnOn(8);
    LedDriver_TurnOff(8);
    LONGS_EQUAL(0x100, virtualLeds_Get());
}
//END: TurnOffAnyLedFirstTry

//START: RevisedTestGroupPlusCreate
TEST(LedDriver, Create)
{
    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: RevisedTestGroupPlusCreate
//START: OutOfBoundsChangesNothing
TEST(LedDriver, OutOfBoundsChangesNothing)
{
    LedDriver_TurnOn(-1);
    LedDriver_TurnOn(0);
    LedDriver_TurnOn(17);
    LedDriver_TurnOn(3141);

    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: OutOfBoundsChangesNothing
//START: Led3141
TEST(LedDriver, OutOfBoundsChangesNothing)
{
    LedDriver_TurnOn(-1);
    LONGS_EQUAL(0, virtualLeds_Get());
    LedDriver_TurnOn(0);
    LONGS_EQUAL(0, virtualLeds_Get());
    LedDriver_TurnOn(17);
    LONGS_EQUAL(0, virtualLeds_Get());
    LedDriver_TurnOn(33);
    LONGS_EQUAL(0, virtualLeds_Get());
    LedDriver_TurnOn(3141);

    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: Led3141
//START: OutOfBoundsTurnOffDoesNoHarm1
TEST(LedDriver, OutOfBoundsTurnOffDoesNoHarm)
{
    LedDriver_TurnOff(-1);
    LedDriver_TurnOff(0);
    LedDriver_TurnOff(17);
    LedDriver_TurnOff(3141);

    LONGS_EQUAL(0, virtualLeds_Get());
}
//END: OutOfBoundsTurnOffDoesNoHarm1
//START: OutOfBoundsTurnOffDoesNoHarm2
TEST(LedDriver, OutOfBoundsTurnOffDoesNoHarm)
{
    LedDriver_TurnAllOn();

    LedDriver_TurnOff(-1);
    LedDriver_TurnOff(0);
    LedDriver_TurnOff(17);
    LedDriver_TurnOff(3141);

    LONGS_EQUAL(0xffff, virtualLeds_Get());
}
//END: OutOfBoundsTurnOffDoesNoHarm2

//START: OutOfBoundsLedsAreAlwaysOff1
TEST(LedDriver, OutOfBoundsLedsAreAlwaysOff)
{
    CHECK_EQUAL(false, LedDriver_IsOn(0));
    CHECK_EQUAL(false, LedDriver_IsOn(17));
}
//END: OutOfBoundsLedsAreAlwaysOff1


#endif


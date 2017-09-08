library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity GAL_addrSelect is
    Port (
	gate	    : In std_logic;
        a11         : In std_logic;
        a10         : In std_logic;
        a9          : In std_logic;
        a8          : In std_logic;
	
        ram_cs       : Out std_logic;
        adc_cs       : Out std_logic;
        oled_cs      : Out std_logic;
        gen_io0_cs   : Out std_logic;
	gen_io1_cs   : Out std_logic
    );
    
    attribute LOC : string;
    
    attribute LOC of ram_cs        : signal is "P19";
    attribute LOC of adc_cs        : signal is "P18";
    attribute LOC of oled_cs   	   : signal is "P17";
    attribute LOC of gen_io0_cs    : signal is "P16";	
    attribute LOC of gen_io1_cs    : signal is "P15";

    attribute LOC of gate	: signal is "P6";
    attribute LOC of a11        : signal is "P5";
    attribute LOC of a10        : signal is "P4";
    attribute LOC of a9         : signal is "P3";
    attribute LOC of a8         : signal is "P2";
    
end;

architecture behavioral of GAL_addrSelect is
begin	

	oled_cs <= a8 OR a9 OR a10 OR a11 OR gate;
	
	adc_cs <= (NOT a8) OR a9 OR a10 OR a11 OR gate;

	gen_io0_cs <= a8 OR (NOT a9) OR a10 OR a11 OR gate;
	gen_io1_cs <= (NOT a8) OR (NOT a9) OR a10 OR a11 OR gate;

	ram_cs <= (a10 NOR a11) OR gate;

	
end behavioral;

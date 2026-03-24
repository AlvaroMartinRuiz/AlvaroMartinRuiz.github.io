library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sine_rom is
  port(
    addr : in  unsigned(3 downto 0); -- address
    dout : out signed(7 downto 0) -- digital output
  );
end sine_rom;

architecture Behavioral of sine_rom is

  -- 16 samples of one sine period, with max/min values equal to ±127
  -- sine value is sin(2π * index/16)*127, where index goes from 0 to 15
  type rom_type is array (0 to 15) of signed(7 downto 0); -- an array of 16 positions of 8 bits to store the value of the leds
  constant ROM : rom_type := (
    to_signed(   0, 8),  -- index=0
    to_signed(  49, 8),  -- index=1
    to_signed(  90, 8),  -- index=2
    to_signed( 118, 8),  -- index=3
    to_signed( 127, 8),  -- index=4
    to_signed( 118, 8),  -- index=5
    to_signed(  90, 8),  -- index=6
    to_signed(  49, 8),  -- index=7
    to_signed(   0, 8),  -- index=8
    to_signed( -49, 8),  -- index=9
    to_signed( -90, 8),  -- index=10
    to_signed(-118, 8),  -- index=11
    to_signed(-127, 8),  -- index=12
    to_signed(-118, 8),  -- index=13
    to_signed( -90, 8),  -- index=14
    to_signed( -49, 8)   -- index=15
  );

begin
  -- Combinational ROM read
  dout <= ROM(to_integer(addr)); -- we select the value at position addr and pass it to dout. dout is connected to sin_val in the ohter code

end Behavioral;
